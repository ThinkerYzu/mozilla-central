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

#ifndef __NSSENSORMANAGER_H_
#define __NSSENSORMANAGER_H_

#include "nsISensorManager.h"
#include "nsTArray.h"
#include "mozilla/Sensor.h"


#define NS_SENSOR_MANAGER_CID					\
    { 0xdba25685, 0x036e, 0x11e1,				\
	    { 0x83, 0xa1, 0x00, 0x22, 0x15, 0xd2, 0x27, 0xb8 }}

#define NS_SENSOR_MANAGER_CONTRACTID "@mozilla.org/hal/sensormanager;1"

class nsSensorObserver;

class nsSensorManager : public nsISensorManager {
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISENSORMANAGER
    
    nsSensorManager();
    virtual ~nsSensorManager();

private:
    nsTArray<nsSensorObserver *> mObservers;

    void _newObserver(mozilla::hal_android::SensorType type,
		      nsISensorListener *listener);
    void _delObserver(int idx);
};


extern nsISensorManager *gSensorManager; // Singleton of nsSensorManager


#endif /* __NSSENSORMANAGER_H_ */
