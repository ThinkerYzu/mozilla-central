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
 * The Original Code is RIL JS Worker.
 *
 * The Initial Developer of the Original Code is
 * the Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Kyle Machulis <kyle@nonpolynomial.com>
 *   Philipp von Weitershausen <philipp@weitershausen.de>
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

const REQUEST_GET_SIM_STATUS = 1;
const REQUEST_ENTER_SIM_PIN = 2;
const REQUEST_ENTER_SIM_PUK = 3;
const REQUEST_ENTER_SIM_PIN2 = 4;
const REQUEST_ENTER_SIM_PUK2 = 5;
const REQUEST_CHANGE_SIM_PIN = 6;
const REQUEST_CHANGE_SIM_PIN2 = 7;
const REQUEST_ENTER_NETWORK_DEPERSONALIZATION = 8;
const REQUEST_GET_CURRENT_CALLS = 9;
const REQUEST_DIAL = 10;
const REQUEST_GET_IMSI = 11;
const REQUEST_HANGUP = 12;
const REQUEST_HANGUP_WAITING_OR_BACKGROUND = 13;
const REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND = 14;
const REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE = 15;
const REQUEST_SWITCH_HOLDING_AND_ACTIVE = 15;
const REQUEST_CONFERENCE = 16;
const REQUEST_UDUB = 17;
const REQUEST_LAST_CALL_FAIL_CAUSE = 18;
const REQUEST_SIGNAL_STRENGTH = 19;
const REQUEST_REGISTRATION_STATE = 20;
const REQUEST_GPRS_REGISTRATION_STATE = 21;
const REQUEST_OPERATOR = 22;
const REQUEST_RADIO_POWER = 23;
const REQUEST_DTMF = 24;
const REQUEST_SEND_SMS = 25;
const REQUEST_SEND_SMS_EXPECT_MORE = 26;
const REQUEST_SETUP_DATA_CALL = 27;
const REQUEST_SIM_IO = 28;
const REQUEST_SEND_USSD = 29;
const REQUEST_CANCEL_USSD = 30;
const REQUEST_GET_CLIR = 31;
const REQUEST_SET_CLIR = 32;
const REQUEST_QUERY_CALL_FORWARD_STATUS = 33;
const REQUEST_SET_CALL_FORWARD = 34;
const REQUEST_QUERY_CALL_WAITING = 35;
const REQUEST_SET_CALL_WAITING = 36;
const REQUEST_SMS_ACKNOWLEDGE = 37;
const REQUEST_GET_IMEI = 38;
const REQUEST_GET_IMEISV = 39;
const REQUEST_ANSWER = 40;
const REQUEST_DEACTIVATE_DATA_CALL = 41;
const REQUEST_QUERY_FACILITY_LOCK = 42;
const REQUEST_SET_FACILITY_LOCK = 43;
const REQUEST_CHANGE_BARRING_PASSWORD = 44;
const REQUEST_QUERY_NETWORK_SELECTION_MODE = 45;
const REQUEST_SET_NETWORK_SELECTION_AUTOMATIC = 46;
const REQUEST_SET_NETWORK_SELECTION_MANUAL = 47;
const REQUEST_QUERY_AVAILABLE_NETWORKS = 48;
const REQUEST_DTMF_START = 49;
const REQUEST_DTMF_STOP = 50;
const REQUEST_BASEBAND_VERSION = 51;
const REQUEST_SEPARATE_CONNECTION = 52;
const REQUEST_SET_MUTE = 53;
const REQUEST_GET_MUTE = 54;
const REQUEST_QUERY_CLIP = 55;
const REQUEST_LAST_DATA_CALL_FAIL_CAUSE = 56;
const REQUEST_DATA_CALL_LIST = 57;
const REQUEST_RESET_RADIO = 58;
const REQUEST_OEM_HOOK_RAW = 59;
const REQUEST_OEM_HOOK_STRINGS = 60;
const REQUEST_SCREEN_STATE = 61;
const REQUEST_SET_SUPP_SVC_NOTIFICATION = 62;
const REQUEST_WRITE_SMS_TO_SIM = 63;
const REQUEST_DELETE_SMS_ON_SIM = 64;
const REQUEST_SET_BAND_MODE = 65;
const REQUEST_QUERY_AVAILABLE_BAND_MODE = 66;
const REQUEST_STK_GET_PROFILE = 67;
const REQUEST_STK_SET_PROFILE = 68;
const REQUEST_STK_SEND_ENVELOPE_COMMAND = 69;
const REQUEST_STK_SEND_TERMINAL_RESPONSE = 70;
const REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM = 71;
const REQUEST_EXPLICIT_CALL_TRANSFER = 72;
const REQUEST_SET_PREFERRED_NETWORK_TYPE = 73;
const REQUEST_GET_PREFERRED_NETWORK_TYPE = 74;
const REQUEST_GET_NEIGHBORING_CELL_IDS = 75;
const REQUEST_SET_LOCATION_UPDATES = 76;
const REQUEST_CDMA_SET_SUBSCRIPTION = 77;
const REQUEST_CDMA_SET_ROAMING_PREFERENCE = 78;
const REQUEST_CDMA_QUERY_ROAMING_PREFERENCE = 79;
const REQUEST_SET_TTY_MODE = 80;
const REQUEST_QUERY_TTY_MODE = 81;
const REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE = 82;
const REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE = 83;
const REQUEST_CDMA_FLASH = 84;
const REQUEST_CDMA_BURST_DTMF = 85;
const REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY = 86;
const REQUEST_CDMA_SEND_SMS = 87;
const REQUEST_CDMA_SMS_ACKNOWLEDGE = 88;
const REQUEST_GSM_GET_BROADCAST_SMS_CONFIG = 89;
const REQUEST_GSM_SET_BROADCAST_SMS_CONFIG = 90;
const REQUEST_GSM_SMS_BROADCAST_ACTIVATION = 91;
const REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG = 92;
const REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG = 93;
const REQUEST_CDMA_SMS_BROADCAST_ACTIVATION = 94;
const REQUEST_CDMA_SUBSCRIPTION = 95;
const REQUEST_CDMA_WRITE_SMS_TO_RUIM = 96;
const REQUEST_CDMA_DELETE_SMS_ON_RUIM = 97;
const REQUEST_DEVICE_IDENTITY = 98;
const REQUEST_EXIT_EMERGENCY_CALLBACK_MODE = 99;
const REQUEST_GET_SMSC_ADDRESS = 100;
const REQUEST_SET_SMSC_ADDRESS = 101;
const REQUEST_REPORT_SMS_MEMORY_STATUS = 102;
const REQUEST_REPORT_STK_SERVICE_IS_RUNNING = 103;

const RESPONSE_TYPE_SOLICITED = 0;
const RESPONSE_TYPE_UNSOLICITED = 1;

const UNSOLICITED_RESPONSE_BASE = 1000;
const UNSOLICITED_RESPONSE_RADIO_STATE_CHANGED = 1000;
const UNSOLICITED_RESPONSE_CALL_STATE_CHANGED = 1001;
const UNSOLICITED_RESPONSE_NETWORK_STATE_CHANGED = 1002;
const UNSOLICITED_RESPONSE_NEW_SMS = 1003;
const UNSOLICITED_RESPONSE_NEW_SMS_STATUS_REPORT = 1004;
const UNSOLICITED_RESPONSE_NEW_SMS_ON_SIM = 1005;
const UNSOLICITED_ON_USSD = 1006;
const UNSOLICITED_ON_USSD_REQUEST = 1007;
const UNSOLICITED_NITZ_TIME_RECEIVED = 1008;
const UNSOLICITED_SIGNAL_STRENGTH = 1009;
const UNSOLICITED_DATA_CALL_LIST_CHANGED = 1010;
const UNSOLICITED_SUPP_SVC_NOTIFICATION = 1011;
const UNSOLICITED_STK_SESSION_END = 1012;
const UNSOLICITED_STK_PROACTIVE_COMMAND = 1013;
const UNSOLICITED_STK_EVENT_NOTIFY = 1014;
const UNSOLICITED_STK_CALL_SETUP = 1015;
const UNSOLICITED_SIM_SMS_STORAGE_FULL = 1016;
const UNSOLICITED_SIM_REFRESH = 1017;
const UNSOLICITED_CALL_RING = 1018;
const UNSOLICITED_RESPONSE_SIM_STATUS_CHANGED = 1019;
const UNSOLICITED_RESPONSE_CDMA_NEW_SMS = 1020;
const UNSOLICITED_RESPONSE_NEW_BROADCAST_SMS = 1021;
const UNSOLICITED_CDMA_RUIM_SMS_STORAGE_FULL = 1022;
const UNSOLICITED_RESTRICTED_STATE_CHANGED = 1023;
const UNSOLICITED_ENTER_EMERGENCY_CALLBACK_MODE = 1024;
const UNSOLICITED_CDMA_CALL_WAITING = 1025;
const UNSOLICITED_CDMA_OTA_PROVISION_STATUS = 1026;
const UNSOLICITED_CDMA_INFO_REC = 1027;
const UNSOLICITED_OEM_HOOK_RAW = 1028;
const UNSOLICITED_RINGBACK_TONE = 1029;
const UNSOLICITED_RESEND_INCALL_MUTE = 1030;

const RADIO_STATE_OFF = 0;
const RADIO_STATE_UNAVAILABLE = 1;
const RADIO_STATE_SIM_NOT_READY = 2;
const RADIO_STATE_SIM_LOCKED_OR_ABSENT = 3;
const RADIO_STATE_SIM_READY = 4;
const RADIO_STATE_RUIM_NOT_READY = 5;
const RADIO_STATE_RUIM_READY = 6;
const RADIO_STATE_RUIM_LOCKED_OR_ABSENT = 7;
const RADIO_STATE_NV_NOT_READY = 8;
const RADIO_STATE_NV_READY = 9;

const CARD_MAX_APPS = 8;

const CALL_STATE_ACTIVE = 0;
const CALL_STATE_HOLDING = 1;
const CALL_STATE_DIALING = 2;
const CALL_STATE_ALERTING = 3;
const CALL_STATE_INCOMING = 4;
const CALL_STATE_WAITING = 5;

const TOA_INTERNATIONAL = 0x91;
const TOA_UNKNOWN = 0x81;

const CALL_PRESENTATION_ALLOWED = 0;
const CALL_PRESENTATION_RESTRICTED = 1;
const CALL_PRESENTATION_UNKNOWN = 2;
const CALL_PRESENTATION_PAYPHONE = 3;


/**
 * DOM constants
 */

const DOM_RADIOSTATE_UNAVAILABLE   = "unavailable";
const DOM_RADIOSTATE_OFF           = "off";
const DOM_RADIOSTATE_READY         = "ready";

const DOM_CARDSTATE_UNAVAILABLE    = "unavailable";
const DOM_CARDSTATE_ABSENT         = "absent";
const DOM_CARDSTATE_PIN_REQUIRED   = "pin_required";
const DOM_CARDSTATE_PUK_REQUIRED   = "puk_required";
const DOM_CARDSTATE_NETWORK_LOCKED = "network_locked";
const DOM_CARDSTATE_NOT_READY      = "not_ready";
const DOM_CARDSTATE_READY          = "ready";

const DOM_CALL_READYSTATE_DIALING        = "dialing";
const DOM_CALL_READYSTATE_RINGING        = "ringing";
const DOM_CALL_READYSTATE_BUSY           = "busy";
const DOM_CALL_READYSTATE_CONNECTING     = "connecting";
const DOM_CALL_READYSTATE_CONNECTED      = "connected";
const DOM_CALL_READYSTATE_DISCONNECTING  = "disconnecting";
const DOM_CALL_READYSTATE_DISCONNECTED   = "disconnected";
const DOM_CALL_READYSTATE_INCOMING       = "incoming";
const DOM_CALL_READYSTATE_HOLDING        = "holding";
const DOM_CALL_READYSTATE_HELD           = "held";

const RIL_TO_DOM_CALL_STATE = [
  DOM_CALL_READYSTATE_CONNECTED, // CALL_READYSTATE_ACTIVE
  DOM_CALL_READYSTATE_HELD,      // CALL_READYSTATE_HOLDING
  DOM_CALL_READYSTATE_DIALING,   // CALL_READYSTATE_DIALING
  DOM_CALL_READYSTATE_RINGING,   // CALL_READYSTATE_ALERTING
  DOM_CALL_READYSTATE_INCOMING,  // CALL_READYSTATE_INCOMING
  DOM_CALL_READYSTATE_HELD       // CALL_READYSTATE_WAITING (XXX is this right?)
];

const DATACALL_RADIOTECHONLOGY_CDMA = 0;
const DATACALL_RADIOTECHONLOGY_GSM = 1;

const DATACALL_AUTH_NONE = 0;
const DATACALL_AUTH_PAP = 1;
const DATACALL_AUTH_CHAP = 2;
const DATACALL_AUTH_PAP_OR_CHAP = 3;

const DATACALL_PROFILE_DEFAULT = 0;
const DATACALL_PROFILE_TETHERED = 1;
const DATACALL_PROFILE_OEM_BASE = 1000;

const DATACALL_DEACTIVATE_NO_REASON = 0;
const DATACALL_DEACTIVATE_RADIO_SHUTDOWN = 1;

const DATACALL_STATE_CONNECTING = "connecting";
const DATACALL_STATE_CONNECTED = "connected";
const DATACALL_STATE_DISCONNECTING = "disconnecting";
const DATACALL_STATE_DISCONNECTED = "disconnected";

const DATACALL_INACTIVE = 0;
const DATACALL_ACTIVE_DOWN = 1;
const DATACALL_ACTIVE_UP = 2;
