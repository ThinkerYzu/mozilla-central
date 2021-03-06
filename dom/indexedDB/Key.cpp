/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Indexed Database.
 *
 * The Initial Developer of the Original Code is The Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Jan Varga <Jan.Varga@gmail.com>
 *   Jonas Sicking <jonas@sicking.cc>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "Key.h"
#include "nsIStreamBufferAccess.h"
#include "jsdate.h"
#include "nsContentUtils.h"
#include "nsJSUtils.h"
#include "xpcprivate.h"
#include "XPCQuickStubs.h"

USING_INDEXEDDB_NAMESPACE

/*
 Here's how we encode keys:

 Basic strategy is the following

 Numbers: 1 n n n n n n n n    ("n"s are encoded 64bit float)
 Dates:   2 n n n n n n n n    ("n"s are encoded 64bit float)
 Strings: 3 s s s ... 0        ("s"s are encoded unicode bytes)
 Arrays:  4 i i i ... 0        ("i"s are encoded array items)


 When encoding floats, 64bit IEEE 754 are almost sortable, except that
 positive sort lower than negative, and negative sort descending. So we use
 the following encoding:
 
 value < 0 ?
   (-to64bitInt(value)) :
   (to64bitInt(value) | 0x8000000000000000)


 When encoding strings, we use variable-size encoding per the following table
 
 Chars 0         - 7E           are encoded as 0xxxxxxx with 1 added
 Chars 7F        - (3FFF+7F)    are encoded as 10xxxxxx xxxxxxxx with 7F subtracted
 Chars (3FFF+80) - FFFF         are encoded as 11xxxxxx xxxxxxxx xx000000

 This ensures that the first byte is never encoded as 0, which means that the
 string terminator (per basic-stategy table) sorts before any character.
 The reason that (3FFF+80) - FFFF is encoded "shifted up" 6 bits is to maximize
 the chance that the last character is 0. See below for why.


 When encoding Arrays, we use an additional trick. Rather than adding a byte
 containing the value '4' to indicate type, we instead add 4 to the next byte.
 This is usually the byte containing the type of the first item in the array.
 So simple examples are

 ["foo"]       7 s s s 0 0                              // 7 is 3 + 4
 [1, 2]        5 n n n n n n n n 1 n n n n n n n n 0    // 5 is 1 + 4

 Whe do this iteratively if the first item in the array is also an array

 [["foo"]]    11 s s s 0 0 0

 However, to avoid overflow in the byte, we only do this 3 times. If the first
 item in an array is an array, and that array also has an array as first item,
 we simply write out the total value accumulated so far and then follow the
 "normal" rules.

 [[["foo"]]]  12 3 s s s 0 0 0 0

 There is another edge case that can happen though, which is that the array
 doesn't have a first item to which we can add 4 to the type. Instead the
 next byte would normally be the array terminator (per basic-strategy table)
 so we simply add the 4 there.

 [[]]         8 0             // 8 is 4 + 4 + 0
 []           4               // 4 is 4 + 0
 [[], "foo"]  8 3 s s s 0 0   // 8 is 4 + 4 + 0

 Note that the max-3-times rule kicks in before we get a chance to add to the
 array terminator

 [[[]]]       12 0 0 0        // 12 is 4 + 4 + 4

 We could use a much higher number than 3 at no complexity or performance cost,
 however it seems unlikely that it'll make a practical difference, and the low
 limit makes testing eaiser.


 As a final optimization we do a post-encoding step which drops all 0s at the
 end of the encoded buffer.
 
 "foo"         // 3 s s s
 1             // 1 bf f0
 ["a", "b"]    // 7 s 3 s
 [1, 2]        // 5 bf f0 0 0 0 0 0 0 1 c0
 [[]]          // 8
*/

const int MaxArrayCollapse = 3;

nsresult
Key::EncodeJSVal(JSContext* aCx, const jsval aVal, PRUint8 aTypeOffset)
{
  PR_STATIC_ASSERT(eMaxType * MaxArrayCollapse < 256);

  if (JSVAL_IS_STRING(aVal)) {
    nsDependentJSString str;
    if (!str.init(aCx, aVal)) {
      return NS_ERROR_OUT_OF_MEMORY;
    }
    EncodeString(str, aTypeOffset);
    return NS_OK;
  }

  if (JSVAL_IS_INT(aVal)) {
    EncodeNumber((double)JSVAL_TO_INT(aVal), eFloat + aTypeOffset);
    return NS_OK;
  }

  if (JSVAL_IS_DOUBLE(aVal)) {
    double d = JSVAL_TO_DOUBLE(aVal);
    if (DOUBLE_IS_NaN(d)) {
      return NS_ERROR_DOM_INDEXEDDB_DATA_ERR;
    }
    EncodeNumber(d, eFloat + aTypeOffset);
    return NS_OK;
  }

  if (!JSVAL_IS_PRIMITIVE(aVal)) {
    JSObject* obj = JSVAL_TO_OBJECT(aVal);
    if (JS_IsArrayObject(aCx, obj)) {
      aTypeOffset += eMaxType;

      if (aTypeOffset == eMaxType * MaxArrayCollapse) {
        mBuffer.Append(aTypeOffset);
        aTypeOffset = 0;
      }
      NS_ASSERTION((aTypeOffset % eMaxType) == 0 &&
                   aTypeOffset < (eMaxType * MaxArrayCollapse),
                   "Wrong typeoffset");

      jsuint length;
      if (!JS_GetArrayLength(aCx, obj, &length)) {
        return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
      }

      for (jsuint index = 0; index < length; index++) {
        jsval val;
        if (!JS_GetElement(aCx, obj, index, &val)) {
          return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
        }

        nsresult rv = EncodeJSVal(aCx, val, aTypeOffset);
        NS_ENSURE_SUCCESS(rv, rv);

        aTypeOffset = 0;
      }

      mBuffer.Append(eTerminator + aTypeOffset);

      return NS_OK;
    }

    if (JS_ObjectIsDate(aCx, obj)) {
      EncodeNumber(js_DateGetMsecSinceEpoch(aCx, obj), eDate + aTypeOffset);
      return NS_OK;
    }
  }

  return NS_ERROR_DOM_INDEXEDDB_DATA_ERR;
}

// static
nsresult
Key::DecodeJSVal(const unsigned char*& aPos, const unsigned char* aEnd,
                 JSContext* aCx, PRUint8 aTypeOffset, jsval* aVal)
{
  if (*aPos - aTypeOffset >= eArray) {
    JSObject* array = JS_NewArrayObject(aCx, 0, nsnull);
    if (!array) {
      NS_WARNING("Failed to make array!");
      return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
    }

    aTypeOffset += eMaxType;

    if (aTypeOffset == eMaxType * MaxArrayCollapse) {
      ++aPos;
      aTypeOffset = 0;
    }

    jsuint index = 0;
    while (aPos < aEnd && *aPos - aTypeOffset != eTerminator) {
      jsval val;
      nsresult rv = DecodeJSVal(aPos, aEnd, aCx, aTypeOffset, &val);
      NS_ENSURE_SUCCESS(rv, rv);

      aTypeOffset = 0;

      if (!JS_SetElement(aCx, array, index++, &val)) {
        NS_WARNING("Failed to set array element!");
        return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
      }
    }

    NS_ASSERTION(aPos >= aEnd || (*aPos % eMaxType) == eTerminator,
                 "Should have found end-of-array marker");
    ++aPos;

    *aVal = OBJECT_TO_JSVAL(array);
  }
  else if (*aPos - aTypeOffset == eString) {
    nsString key;
    DecodeString(aPos, aEnd, key);
    if (!xpc_qsStringToJsval(aCx, key, aVal)) {
      return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
    }
  }
  else if (*aPos - aTypeOffset == eDate) {
    jsdouble msec = static_cast<jsdouble>(DecodeNumber(aPos, aEnd));
    JSObject* date = JS_NewDateObjectMsec(aCx, msec);
    if (!date) {
      NS_WARNING("Failed to make date!");
      return NS_ERROR_DOM_INDEXEDDB_UNKNOWN_ERR;
    }

    *aVal = OBJECT_TO_JSVAL(date);
  }
  else if (*aPos - aTypeOffset == eFloat) {
    *aVal = DOUBLE_TO_JSVAL(DecodeNumber(aPos, aEnd));
  }
  else {
    NS_NOTREACHED("Unknown key type!");
  }

  return NS_OK;
}


#define ONE_BYTE_LIMIT 0x7E
#define TWO_BYTE_LIMIT (0x3FFF+0x7F)

#define ONE_BYTE_ADJUST 1
#define TWO_BYTE_ADJUST (-0x7F)
#define THREE_BYTE_SHIFT 6

void
Key::EncodeString(const nsAString& aString, PRUint8 aTypeOffset)
{
  // First measure how long the encoded string will be.

  // The +2 is for initial 3 and trailing 0. We'll compensate for multi-byte
  // chars below.
  PRUint32 size = aString.Length() + 2;
  
  const PRUnichar* start = aString.BeginReading();
  const PRUnichar* end = aString.EndReading();
  for (const PRUnichar* iter = start; iter < end; ++iter) {
    if (*iter > ONE_BYTE_LIMIT) {
      size += *iter > TWO_BYTE_LIMIT ? 2 : 1;
    }
  }

  // Allocate memory for the new size
  PRUint32 oldLen = mBuffer.Length();
  char* buffer;
  if (!mBuffer.GetMutableData(&buffer, oldLen + size)) {
    return;
  }
  buffer += oldLen;

  // Write type marker
  *(buffer++) = eString + aTypeOffset;

  // Encode string
  for (const PRUnichar* iter = start; iter < end; ++iter) {
    if (*iter <= ONE_BYTE_LIMIT) {
      *(buffer++) = *iter + ONE_BYTE_ADJUST;
    }
    else if (*iter <= TWO_BYTE_LIMIT) {
      PRUnichar c = PRUnichar(*iter) + TWO_BYTE_ADJUST + 0x8000;
      *(buffer++) = (char)(c >> 8);
      *(buffer++) = (char)(c & 0xFF);
    }
    else {
      PRUint32 c = (PRUint32(*iter) << THREE_BYTE_SHIFT) | 0x00C00000;
      *(buffer++) = (char)(c >> 16);
      *(buffer++) = (char)(c >> 8);
      *(buffer++) = (char)c;
    }
  }

  // Write end marker
  *(buffer++) = eTerminator;
  
  NS_ASSERTION(buffer == mBuffer.EndReading(), "Wrote wrong number of bytes");
}

// static
void
Key::DecodeString(const unsigned char*& aPos, const unsigned char* aEnd,
                  nsString& aString)
{
  NS_ASSERTION(*aPos % eMaxType == eString, "Don't call me!");

  const unsigned char* buffer = aPos + 1;

  // First measure how big the decoded string will be.
  PRUint32 size = 0;
  const unsigned char* iter; 
  for (iter = buffer; iter < aEnd && *iter != eTerminator; ++iter) {
    if (*iter & 0x80) {
      iter += (*iter & 0x40) ? 2 : 1;
    }
    ++size;
  }
  
  // Set end so that we don't have to check for null termination in the loop
  // below
  if (iter < aEnd) {
    aEnd = iter;
  }

  PRUnichar* out;
  if (size && !aString.GetMutableData(&out, size)) {
    return;
  }

  for (iter = buffer; iter < aEnd;) {
    if (!(*iter & 0x80)) {
      *out = *(iter++) - ONE_BYTE_ADJUST;
    }
    else if (!(*iter & 0x40)) {
      PRUnichar c = (PRUnichar(*(iter++)) << 8);
      if (iter < aEnd) {
        c |= *(iter++);
      }
      *out = c - TWO_BYTE_ADJUST - 0x8000;
    }
    else {
      PRUint32 c = PRUint32(*(iter++)) << (16 - THREE_BYTE_SHIFT);
      if (iter < aEnd) {
        c |= PRUint32(*(iter++)) << (8 - THREE_BYTE_SHIFT);
      }
      if (iter < aEnd) {
        c |= *(iter++) >> THREE_BYTE_SHIFT;
      }
      *out = (PRUnichar)c;
    }
    
    ++out;
  }
  
  NS_ASSERTION(!size || out == aString.EndReading(),
               "Should have written the whole string");
  
  aPos = iter + 1;
}

union Float64Union {
  double d;
  PRUint64 u;
}; 

void
Key::EncodeNumber(double aFloat, PRUint8 aType)
{
  // Allocate memory for the new size
  PRUint32 oldLen = mBuffer.Length();
  char* buffer;
  if (!mBuffer.GetMutableData(&buffer, oldLen + 1 + sizeof(double))) {
    return;
  }
  buffer += oldLen;

  *(buffer++) = aType;

  Float64Union pun;
  pun.d = aFloat;
  PRUint64 number = pun.u & PR_UINT64(0x8000000000000000) ?
                    -pun.u :
                    (pun.u | PR_UINT64(0x8000000000000000));

  *reinterpret_cast<PRUint64*>(buffer) = NS_SWAP64(number);
}

// static
double
Key::DecodeNumber(const unsigned char*& aPos, const unsigned char* aEnd)
{
  NS_ASSERTION(*aPos % eMaxType == eFloat ||
               *aPos % eMaxType == eDate, "Don't call me!");

  ++aPos;
  PRUint64 number = 0;
  for (PRInt32 n = 7; n >= 0; --n) {
    number <<= 8;
    if (aPos < aEnd) {
      number |= *(aPos++);
    }
    else {
      number <<= 8 * n;
      break;
    }
  }

  Float64Union pun;
  pun.u = number & PR_UINT64(0x8000000000000000) ?
          (number & ~PR_UINT64(0x8000000000000000)) :
          -number;

  return pun.d;
}
