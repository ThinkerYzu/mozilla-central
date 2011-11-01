/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vim: sw=4 ts=8 et ft=cpp : */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Code.
 *
 * The Initial Developer of the Original Code is
 *   The Mozilla Foundation
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Sinker Li <thinker@codemud.net>
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

#include "Sensor.h"
#include "nsTArray.h"
#include "nspr/prtime.h"
#include "AndroidBridge.h"

namespace mozilla {
namespace hal_android {

static nsTArray<ISensorObserver *> *hal_observers;

static nsTArray<ISensorObserver *> &
_get_observers(SensorType sensor_type) {
    if(hal_observers == NULL)
	hal_observers = new nsTArray<ISensorObserver *>[NumSensorType];
    return hal_observers[sensor_type];
}

static int sensor_type_2_android_map[][2] = {
    // Must be consistent with definitions in
    // embedding/android/GeckoAppShell.java
    { SENSOR_ORIENTATION, 1 },
    { SENSOR_ACCELERATION, 2 },
    { SENSOR_PROXIMITY, 3 },
    { -1, -1 }
};

/* Translate ID of sensor type from Sensor service to Android */
static int
_map_sensor_type(int sensor_type) {
    int i;
    
    for(i = 0; sensor_type_2_android_map[i][0] != -1; i++) {
        if(sensor_type_2_android_map[i][0] == sensor_type)
            return sensor_type_2_android_map[i][1];
    }
    return -1;
}

void
RegisterSensorObserver(SensorType sensor, ISensorObserver *observer) {
    nsTArray<ISensorObserver *> &observers = _get_observers(sensor);
    int android_sensor = _map_sensor_type(sensor);

    observers.AppendElement(observer);
    if(observers.Length() == 1) {
        AndroidBridge::Bridge()->EnableSensor(android_sensor);
    }
}

void
UnregisterSensorObserver(SensorType sensor, ISensorObserver *observer) {
    nsTArray<ISensorObserver *> &observers = _get_observers(sensor);
    int android_sensor = _map_sensor_type(sensor);

    observers.RemoveElement(observer);
    if(observers.Length() == 0) {
        AndroidBridge::Bridge()->DisableSensor(android_sensor);
    }
}

void PostProximityEvent(double distance) {
    nsTArray<ISensorObserver *> &observers = _get_observers(SENSOR_PROXIMITY);
    SensorData *sdata;
    ISensorObserver *observer;
    int i;
    
    sdata = new SensorData();
    sdata->sensor = SENSOR_PROXIMITY;
    sdata->timestamp = PR_Now();
    sdata->numValues = 1;
    sdata->values[0] = distance;
    for(i = 0; i < observers.Length(); i++) {
        observer = observers[i];
        observer->observe(sdata);
    }
}

}
}
