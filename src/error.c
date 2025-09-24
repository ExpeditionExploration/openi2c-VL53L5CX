
#include "error.h"

#include <node_api.h>

const char* INIT_ERROR = "initialization error";
const char* INVALID_PARAM_ERROR = "invalid param";
const char* XTALK_ERROR = "xtalk failed";
const char* CORRUPTED_FRAME_ERROR = "corrupted frame";
const char* TIMEOUT_ERROR = "timeout error";
const char* CRC_CHECKSUM_ERROR = "crc checksum error";
const char* MCU_ERROR = "mcu error";
const char* ERROR = "unknown generic error";

const char* INIT_COMMS_ERROR = "connection error";

const char* MODULE_INIT_ERROR = "error initializing module";
const char* ARGUMENT_ERROR = "invalid arguments";
const char* INFO_ERROR = "could not parse cb info";

const char* NO_SENSOR = "no sensor";
const char* VALUE_NAPI_ERROR = "couldn't create napi value";
const char* NAMED_PROPERTY_NOT_SET = "couldn't set named property";
const char* ARRAY_VALUE_NOT_SET = "couldn't set array napi value";
const char* UNKNOWN_ERROR = "unknown error";

const char* ERROR_CHANGING_SETTING = "change setting error";
const char* ERROR_GETTING_VALUE = "can't read value from sensor";
const char* XTALK_CALIBRATION_FAILED = "xtalk calibration failed";
const char* XTALK_CANNOT_FETCH_DATA = "cannot fetch xtalk calibration";
const char* XTALK_CANNOT_STORE_DATA = "cannot store xtalk calibration";
