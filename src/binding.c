#include <string.h>
#include <stdio.h>

#include <node_api.h>
#include <vl53l5cx_api.h>

#include "constants.h"
#include "funcs.h"


/**
 * Assign pre-processor constants from vl53l5cx_api.h to the array for them
 * to have addresses to point to. For their meanings, see vl53l5cx_api.h.
 */

typedef struct {
    const char* name;
    binding_constant_type_t type;
    size_t value; // To be cast to proper type when making the value
} __constants_t;

static const __constants_t CONSTS[] = {
    {
        .name = "VL53L5CX_API_REVISION",
        .type = CT_UTF_8_STRING,
        .value = (size_t)VL53L5CX_API_REVISION
    }, {
        .name = "VL53L5CX_DEFAULT_I2C_ADDRESS",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_DEFAULT_I2C_ADDRESS
    }, {
        .name = "VL53L5CX_RESOLUTION_4X4",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_RESOLUTION_4X4
    }, {
        .name = "VL53L5CX_RESOLUTION_8X8",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_RESOLUTION_8X8
    }, {
        .name = "VL53L5CX_TARGET_ORDER_CLOSEST",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_TARGET_ORDER_CLOSEST
    }, {
        .name = "VL53L5CX_TARGET_ORDER_STRONGEST",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_TARGET_ORDER_STRONGEST
    }, {
        .name = "VL53L5CX_RANGING_MODE_AUTONOMOUS",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_RANGING_MODE_AUTONOMOUS
    }, {
        .name = "VL53L5CX_RANGING_MODE_CONTINUOUS",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_RANGING_MODE_CONTINUOUS
    }, {
        .name = "VL53L5CX_POWER_MODE_SLEEP",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_POWER_MODE_SLEEP
    }, {
        .name = "VL53L5CX_POWER_MODE_WAKEUP",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_POWER_MODE_WAKEUP
    }, {
        .name = "VL53L5CX_STATUS_OK",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_OK
    }, {
        .name = "VL53L5CX_STATUS_TIMEOUT_ERROR",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_TIMEOUT_ERROR
    }, {
        .name = "VL53L5CX_STATUS_CORRUPTED_FRAME",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_CORRUPTED_FRAME
    }, {
        .name = "VL53L5CX_STATUS_CRC_CSUM_FAILED",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_CRC_CSUM_FAILED
    }, {
        .name = "VL53L5CX_STATUS_XTALK_FAILED",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_XTALK_FAILED
    }, {
        .name = "VL53L5CX_MCU_ERROR",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_MCU_ERROR
    }, {
        .name = "VL53L5CX_STATUS_INVALID_PARAM",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_INVALID_PARAM
    }, {
        .name = "VL53L5CX_STATUS_ERROR",
        .type = CT_UINT8_INTEGER,
        .value = VL53L5CX_STATUS_ERROR
    }, {
        .name = "VL53L5CX_START_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_START_BH
    }, {
        .name = "VL53L5CX_METADATA_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_METADATA_BH
    }, {
        .name = "VL53L5CX_COMMONDATA_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_COMMONDATA_BH
    }, {
        .name = "VL53L5CX_AMBIENT_RATE_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_AMBIENT_RATE_BH
    }, {
        .name = "VL53L5CX_SPAD_COUNT_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_SPAD_COUNT_BH
    }, {
        .name = "VL53L5CX_NB_TARGET_DETECTED_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_NB_TARGET_DETECTED_BH
    }, {
        .name = "VL53L5CX_SIGNAL_RATE_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_SIGNAL_RATE_BH
    }, {
        .name = "VL53L5CX_RANGE_SIGMA_MM_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_RANGE_SIGMA_MM_BH
    }, {
        .name = "VL53L5CX_DISTANCE_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_DISTANCE_BH
    }, {
        .name = "VL53L5CX_REFLECTANCE_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_REFLECTANCE_BH
    }, {
        .name = "VL53L5CX_TARGET_STATUS_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_TARGET_STATUS_BH
    }, {
        .name = "VL53L5CX_MOTION_DETECT_BH",
        .type = CT_UINT32_INTEGER,
        .value = VL53L5CX_MOTION_DETECT_BH
    }, {
        .name = "VL53L5CX_METADATA_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_METADATA_IDX
    }, {
        .name = "VL53L5CX_SPAD_COUNT_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_SPAD_COUNT_IDX
    }, {
        .name = "VL53L5CX_AMBIENT_RATE_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_AMBIENT_RATE_IDX
    }, {
        .name = "VL53L5CX_NB_TARGET_DETECTED_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_NB_TARGET_DETECTED_IDX
    }, {
        .name = "VL53L5CX_SIGNAL_RATE_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_SIGNAL_RATE_IDX
    }, {
        .name = "VL53L5CX_RANGE_SIGMA_MM_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_RANGE_SIGMA_MM_IDX
    }, {
        .name = "VL53L5CX_DISTANCE_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_DISTANCE_IDX
    }, {
        .name = "VL53L5CX_REFLECTANCE_EST_PC_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_REFLECTANCE_EST_PC_IDX
    }, {
        .name = "VL53L5CX_TARGET_STATUS_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_TARGET_STATUS_IDX
    }, {
        .name = "VL53L5CX_MOTION_DETEC_IDX",
        .type = CT_UINT16_INTEGER,
        .value = VL53L5CX_MOTION_DETEC_IDX
    }
};


/**
 * Up to 10 sensors
 */
static VL53L5CX_Configuration CONFS[10];

napi_value init(napi_env env, napi_value exports) {

    /**
     * Constants from vl53l5cx_api.h
     */

    for (size_t i = 0; i < sizeof(CONSTS) / sizeof(CONSTS[0]); i++) {
        register_constant(
            env,
            exports,
            CONSTS[i].name,
            CONSTS[i].type,
            CONSTS[i].type == CT_UTF_8_STRING 
                ? ((void*)CONSTS[i].value)
                : ((void*)&CONSTS[i].value)
        );
    }
    
    register_fn(CONFS, env, exports, "comms_init", cb_vl53l5cx_comms_init);
    register_fn(CONFS, env, exports, "is_alive", cb_vl53l5cx_is_alive);
    register_fn(CONFS, env, exports, "check_data_ready",
        cb_vl53l5cx_check_data_ready);
    register_fn(CONFS, env, exports, "start_ranging", 
        cb_vl53l5cx_start_ranging);
    register_fn(CONFS, env, exports, "stop_ranging", cb_vl53l5cx_stop_ranging);
    register_fn(CONFS, env, exports, "get_ranging_data", 
        cb_vl53l5cx_get_ranging_data);
    register_fn(CONFS, env, exports, "set_resolution", 
        cb_vl53l5cx_set_resolution);
    register_fn(CONFS, env, exports, "set_ranging_frequency_hz", 
        cb_vl53l5cx_set_ranging_frequency_hz);
    register_fn(CONFS, env, exports, "set_target_order", 
        cb_vl53l5cx_set_target_order);
    register_fn(CONFS, env, exports, "set_ranging_mode", 
        cb_vl53l5cx_set_ranging_mode);
    register_fn(CONFS, env, exports, "init", cb_vl53l5cx_init);
    register_fn(CONFS, env, exports, "get_integration_time_ms", 
        cb_vl53l5cx_get_integration_time_ms);
    register_fn(CONFS, env, exports, "calibrate_xtalk",
        cb_vl53l5cx_calibrate_xtalk);
    register_fn(CONFS, env, exports, "get_caldata_xtalk",
        cb_vl53l5cx_get_caldata_xtalk);
    register_fn(CONFS, env, exports, "set_caldata_xtalk",
        cb_vl53l5cx_set_caldata_xtalk);
    

    register_fn(CONFS, env, exports, "set_power_mode",
        cb_vl53l5cx_set_power_mode);
    
    return exports;
}

NAPI_MODULE(vl53l5cx_native, init)
