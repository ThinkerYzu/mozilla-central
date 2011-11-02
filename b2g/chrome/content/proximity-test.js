
function proximity() {
}

proximity.prototype = {
    onSensorChange: function(aData) {
	try {
	    this._onSensorChange(aData);
	} catch(e) {
	    alert(e);
	}
    },
    _onSensorChange: function(aData) {
	var Cc = Components.classes;
	var Ci = Components.interfaces;
	var sensor_manager = Cc["@mozilla.org/hal/sensormanager;1"];
	var service = sensor_manager.getService(Ci.nsISensorManager);
	var message, txt;
	var distance;
	
	switch(aData.sensor) {
	case service.SENSOR_PROXIMITY:
	    message = document.getElementById("message");
	    while(message.firstChild)
		message.removeChild(message.firstChild);
	    distance = 
		aData.values.queryElementAt(0 , Ci.nsISensorValue).value;
	    txt = document.createTextNode("Distance: " + distance);
	    message.appendChild(txt);
	    break;
	}
    },
    QueryInterface: function(aIID) {
	var Ci = Components.interfaces;

	if(!aIID.equals(Ci.nsISupports) &&
	   !aIID.equlas(Ci.nsISensorListener))
	    throw Components.results.NS_ERROR_NO_INTERFACE;
	return this;
    },
};

function init() {
    try {
	_init();
    } catch(e) {
	alert(e);
    }
}

var _proximity_obj;

function _init() {
    var Cc = Components.classes;
    var Ci = Components.interfaces;
    var sensor_manager = Cc["@mozilla.org/hal/sensormanager;1"];
    var iface = Ci.nsISensorManager;
    var service = sensor_manager.getService(iface);
    
    _proximity_obj = new proximity();
    
    service.addListener(service.SENSOR_PROXIMITY, _proximity_obj);
}
