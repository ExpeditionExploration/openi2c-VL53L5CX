#ifndef __ERROR_H
#define __ERROR_H

#include <node/node_api.h>
#include <stdint.h>

/**
 * Errors indicating a hardware problem.
 */

extern const char* INVALID_PARAM_ERROR;
extern const char* XTALK_ERROR;
extern const char* CORRUPTED_FRAME_ERROR;
extern const char* TIMEOUT_ERROR;
extern const char* CRC_CHECKSUM_ERROR;
extern const char* MCU_ERROR;
extern const char* ERROR;

extern const char* INIT_COMMS_ERROR;
extern const char* NO_SENSOR;

extern const char* ERROR_CHANGING_SETTING;
extern const char* ERROR_GETTING_VALUE;
extern const char* XTALK_CALIBRATION_FAILED;
extern const char* XTALK_CANNOT_FETCH_DATA;
extern const char* XTALK_CANNOT_STORE_DATA;


/**
 * Node-API side of errors.
 */
extern const char* MODULE_INIT_ERROR;
extern const char* ARGUMENT_ERROR;
extern const char* INFO_ERROR;
extern const char* VALUE_NAPI_ERROR;
extern const char* NAMED_PROPERTY_NOT_SET;
extern const char* ARRAY_VALUE_NOT_SET;
extern const char* UNKNOWN_ERROR;

#endif
