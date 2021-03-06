/*  GRAPHITE2 LICENSING

    Copyright 2010, SIL International
    All rights reserved.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should also have received a copy of the GNU Lesser General Public
    License along with this library in the file named "LICENSE".
    If not, write to the Free Software Foundation, 51 Franklin Street, 
    Suite 500, Boston, MA 02110-1335, USA or visit their web page on the 
    internet at http://www.fsf.org/licenses/lgpl.html.

Alternatively, the contents of this file may be used under the terms of the
Mozilla Public License (http://mozilla.org/MPL) or the GNU General Public
License, as published by the Free Software Foundation, either version 2
of the License or (at your option) any later version.
*/
#pragma once

#include "Main.h"
#include "XmlTraceLog.h"
#include "Main.h"
#include "Position.h"
#include "Sparse.h"

namespace graphite2 {

enum metrics {
    kgmetLsb = 0, kgmetRsb,
    kgmetBbTop, kgmetBbBottom, kgmetBbLeft, kgmetBbRight,
    kgmetBbHeight, kgmetBbWidth,
    kgmetAdvWidth, kgmetAdvHeight,
    kgmetAscent, kgmetDescent
};

class Rect
{
public :
    Rect() {}
    Rect(const Position& botLeft, const Position& topRight): bl(botLeft), tr(topRight) {}
    Rect widen(const Rect& other) { return Rect(Position(bl.x > other.bl.x ? other.bl.x : bl.x, bl.y > other.bl.y ? other.bl.y : bl.y), Position(tr.x > other.tr.x ? tr.x : other.tr.x, tr.y > other.tr.y ? tr.y : other.tr.y)); }
    Rect operator + (const Position &a) const { return Rect(Position(bl.x + a.x, bl.y + a.y), Position(tr.x + a.x, tr.y + a.y)); }
    Rect operator * (float m) const { return Rect(Position(bl.x, bl.y) * m, Position(tr.x, tr.y) * m); }

    Position bl;
    Position tr;
};

class XmlTraceLog;
class GlyphFaceCacheHeader;

class GlyphFace
{
private:
friend class GlyphFaceCache;
    GlyphFace(const GlyphFaceCacheHeader& hdr, unsigned short glyphid);
    ~GlyphFace() throw();

public:

    const Position    & theAdvance() const;
    const Rect &theBBox() const { return m_bbox; }
    uint16  getAttr(uint8 index) const { 
        if (m_attrs)
            return m_attrs[index];
#ifdef ENABLE_DEEP_TRACING
        XmlTraceLog::get().warning("No attributes for glyph attr %d", index);
#endif
        return 0;
    }
    uint16  getMetric(uint8 metric) const;

private:
//    static void readAttrs(const byte *glat, const byte * const glat_end, uint16 * attrs, size_t num, uint32 format);       //only called from constructor
    static void logAttr(const uint16 attrs[], const uint16 * attr);

private:
    Rect     m_bbox;        // bounding box metrics in design units
    Position m_advance;     // Advance width and height in design units
    sparse   m_attrs;
};


inline GlyphFace::~GlyphFace() throw() {
}

inline const Position & GlyphFace::theAdvance() const { 
    return m_advance;
}

} // namespace graphite2
