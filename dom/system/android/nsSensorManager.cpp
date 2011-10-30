/* -*- mode: c++; c-basic-offset: 4; tab-width: 8 -*- */
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
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Sinker Li <tlee@mozilla.com>
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

#include "nsSensorManager.h"
#include "prtypes.h"
#include "nsIArray.h"
#include "nsIMutableArray.h"
#include "nsTArray.h"
#include "nsComponentManagerUtils.h"
#include "nsAutoPtr.h"
#include "mozilla/Sensor.h"


/* Singleton for sensor manager */
nsISensorManager *gSensorManager = NULL;


class nsSensorValue : public nsISensorValue {
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISENSORVALUE
    
    nsSensorValue(float aValue) : mValue(aValue) {}
    virtual ~nsSensorValue();
    
private:
    float mValue;
};

NS_INTERFACE_MAP_BEGIN(nsSensorValue)
NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsISensorValue)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsSensorValue)
NS_IMPL_RELEASE(nsSensorValue)

nsSensorValue::~nsSensorValue() {
}

NS_IMETHODIMP
nsSensorValue::GetValue(float *value) {
    NS_ENSURE_ARG_POINTER(value);
    *value = mValue;
    return NS_OK;
}


class nsSensorData : public nsISensorData {
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISENSORDATA

    nsSensorData(PRUint16 aSensorType, PRUint64 aTimestamp,
		 float *aValues, int aNumValues);
    virtual ~nsSensorData();

private:
    PRUint16 mSensorType;
    PRUint64 mTimestamp;
    nsCOMPtr<nsIArray> mValues;
};

NS_INTERFACE_MAP_BEGIN(nsSensorData)
NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsISensorData)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsSensorData)
NS_IMPL_RELEASE(nsSensorData)

nsSensorData::nsSensorData(PRUint16 aSensorType, PRUint64 aTimestamp,
			   float *aValues, int aNumValues) :
    mSensorType(aSensorType), mTimestamp(aTimestamp) {
    nsCOMPtr<nsIMutableArray> values;
    nsSensorValue *value;
    int i;
    
    values = do_CreateInstance(NS_ARRAY_CONTRACTID);
    for(i = 0; i < aNumValues; i++) {
	value = new nsSensorValue(aValues[i]);
	values->AppendElement(value, false);
    }
    
    mValues = values;
}

nsSensorData::~nsSensorData() {
}

NS_IMETHODIMP
nsSensorData::GetSensor(PRUint32 *aSensorType) {
    if(aSensorType == NULL)
	return NS_ERROR_NULL_POINTER;
    *aSensorType = mSensorType;
    return NS_OK;
}

NS_IMETHODIMP
nsSensorData::GetTimestamp(PRUint64 *stamp) {
    if(stamp == NULL)
	return NS_ERROR_NULL_POINTER;
    *stamp = mTimestamp;
    return NS_OK;
}

NS_IMETHODIMP
nsSensorData::GetValues(nsIArray **aValues) {
    if(aValues == NULL || *aValues == NULL)
	return NS_ERROR_NULL_POINTER;
    
    *aValues = mValues;
    NS_ADDREF(*aValues);
    return NS_OK;
}


using namespace mozilla::hal_android;

class nsSensorObserver : public ISensorObserver {
public:
    nsSensorObserver(SensorType aSensorType, nsISensorListener *aListener) :
	mSensorType(aSensorType), mListener(aListener) {}
    ~nsSensorObserver();

    virtual void observe(SensorData *data);

    SensorType mSensorType;
    nsCOMPtr<nsISensorListener> mListener;
};

nsSensorObserver::~nsSensorObserver() {}

void
nsSensorObserver::observe(SensorData *data) {
    nsSensorData *sensordata;

    sensordata = new nsSensorData(data->sensor, data->timestamp,
				  data->values, data->numValues);
    mListener->OnSensorChange(sensordata);
}


class nsSensorManager : public nsISensorManager {
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISENSORMANAGER
    
    nsSensorManager();
    virtual ~nsSensorManager();

private:
    nsTArray<nsSensorObserver *> mObservers;
};

NS_INTERFACE_MAP_BEGIN(nsSensorManager)
NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsISensorManager)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsSensorManager)
NS_IMPL_RELEASE(nsSensorManager)

nsSensorManager::nsSensorManager() {
    gSensorManager = this;
}

nsSensorManager::~nsSensorManager() {
}

static struct {
    PRUint32 sdtype;		// defined by nsISensorData
    SensorType stype;		// defined by SensorType (in Sensor.h)
} type_map[] = {
    { SENSOR_ORIENTATION, mozilla::hal_android::SENSOR_ORIENTATION },
    { SENSOR_ACCELERATION, mozilla::hal_android::SENSOR_ACCELERATION },
    { SENSOR_PROXIMITY, mozilla::hal_android::SENSOR_PROXIMITY },
    { 0xffffffff, mozilla::hal_android::SENSOR_UNKNOWN }
};

static SensorType
_sensor_data_to_sensor_type(PRUint32 sdtype) {
    int i;
    
    for(i = 0; type_map[i].sdtype != 0xffffffff; i++) {
	if(type_map[i].sdtype == sdtype)
	    return type_map[i].stype;
    }
    return mozilla::hal_android::SENSOR_UNKNOWN;
}

NS_IMETHODIMP
nsSensorManager::AddListener(PRUint32 aSensorType,
			     nsISensorListener *aListener) {
    nsSensorObserver *observer;
    SensorType sensor_type;

    NS_ENSURE_ARG_POINTER(aListener);
    
    sensor_type = _sensor_data_to_sensor_type(aSensorType);
    if(sensor_type == mozilla::hal_android::SENSOR_UNKNOWN)
	return NS_ERROR_INVALID_ARG;
    
    observer = new nsSensorObserver(sensor_type, aListener);
    mObservers.AppendElement(observer);
    RegisterSensorObserver(sensor_type, observer);
	
    return NS_OK;
}

NS_IMETHODIMP
nsSensorManager::RemoveListener(PRUint32 aSensorType,
				nsISensorListener *aListener) {
    nsSensorObserver *observer;
    SensorType sensor_type;
    int i;

    sensor_type = _sensor_data_to_sensor_type(aSensorType);
    if(sensor_type == mozilla::hal_android::SENSOR_UNKNOWN)
	return NS_ERROR_INVALID_ARG;
    
    for(i = mObservers.Length() - 1; i >= 0; i--) {
	observer = mObservers[i];
	if(observer->mSensorType == sensor_type &&
	   observer->mListener.get() == aListener) {
	    UnregisterSensorObserver(sensor_type, observer);
	    mObservers.RemoveElementAt(i);
	    
	    delete observer;
	    return NS_OK;
	}
    }
    
    return NS_ERROR_INVALID_ARG;
}
