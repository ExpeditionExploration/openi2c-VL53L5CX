#include <fcntl.h>
#include <stdio.h>

#include <node_api.h>
#include <vl53l5cx_api.h>
#include "vl53l5cx_plugin_xtalk.h"

#include "funcs.h"

/* Maximum length of formatted error message. */
#define MAX_LEN_ERROR 80

/* Maximum number of arguments the binding function can take from Node */
#define MAX_ARGUMENTS 10

/**
 * Auxialiry func to parse napi_callback arguments from Node
 * 
 * @return success
 */
bool parse_args(
    napi_env env,
    napi_callback_info info,
    size_t* argc,
    napi_value *argv,
    napi_value* this,
    void** data,
    size_t argc_min,
    size_t argc_max
) {
    napi_status status = napi_get_cb_info(env, info, argc, argv, this, data);
    switch (status)
    {
    case napi_ok:
        break;

    case napi_invalid_arg:
        napi_throw_error(env, ARGUMENT_ERROR, "Invalid argument");
        return false;

    case napi_object_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Object expected");
        return false;

    case napi_string_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "String expected");
    return false;

    case napi_name_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Name expected");
        return false;

    case napi_function_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Function expected");
        return false;

    case napi_number_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Number expected");
        return false;

    case napi_boolean_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Boolean expected");
        return false;

    case napi_array_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Array expected");
        return false;

    case napi_generic_failure:
        napi_throw_error(env, ARGUMENT_ERROR, "Generic failure");
        return false;

    case napi_pending_exception:
        napi_throw_error(env, ARGUMENT_ERROR, "Pending exeption");
        return false;

    case napi_cancelled:
        napi_throw_error(env, ARGUMENT_ERROR, "Cancelled");
        return false;

    case napi_escape_called_twice:
        napi_throw_error(env, ARGUMENT_ERROR, "Escape called twice");
        return false;

    case napi_handle_scope_mismatch:
        napi_throw_error(env, ARGUMENT_ERROR, "Scope mismatch");
        return false;

    case napi_callback_scope_mismatch:
        napi_throw_error(env, ARGUMENT_ERROR, "Callback scope mismatch");
        return false;

    case napi_queue_full:
        napi_throw_error(env, ARGUMENT_ERROR, "Napi queue full");
        return false;

    case napi_closing:
        napi_throw_error(env, ARGUMENT_ERROR, "Closing");
        return false;

    case napi_bigint_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Bigint expected");
        return false;

    case napi_date_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Date expected");
        return false;

    case napi_arraybuffer_expected:
        napi_throw_error(env, ARGUMENT_ERROR, "Arraybuffer expected");
        return false;

    case napi_detachable_arraybuffer_expected:
        napi_throw_error(
            env, ARGUMENT_ERROR, "Detatchable arraybuffer expected"
        );
        return false;

    case napi_would_deadlock:  /* unused */
    case napi_no_external_buffers_allowed:
        napi_throw_error(env, ARGUMENT_ERROR, "No external buffers allowed");
        return false;

    case napi_cannot_run_js:
        napi_throw_error(env, ARGUMENT_ERROR, "=D");
        return false;

    default:
        napi_throw_error(
            env, UNKNOWN_ERROR, "A new, unexpected error happened."
        );
        return false;
    }

    if (*argc < argc_min || *argc > argc_max) {
        napi_throw_error(
            env, ARGUMENT_ERROR, "Too many or too few arguments"
        );
        return false;
    }
    return true;
}

bool register_fn(
    VL53L5CX_Configuration* conf, napi_env env, napi_value exports,
    const char* fn_name, napi_callback cb
) {
    napi_value fn;
    napi_status status;
    char error_msg[MAX_LEN_ERROR] = {0};

    status = napi_create_function(
        env, fn_name, strlen(fn_name), cb, conf, &fn
    );
    if (status != napi_ok) {
        snprintf(error_msg, MAX_LEN_ERROR-1,
            "Could not create JS func %s", fn_name);
        napi_throw_error(
            env, MODULE_INIT_ERROR, 
            error_msg
        );
        return false;
    }

    status = napi_set_named_property(env, exports, fn_name, fn);
    if (status != napi_ok) {
        snprintf(error_msg, MAX_LEN_ERROR-1,
            "Could not bind JS func %s to the module", fn_name);
        napi_throw_error(
            env, MODULE_INIT_ERROR,
            error_msg
        );
        return false;
    }
    return true;
}

/************
 * Comms init
 */

/// @brief Initialize comms. This method must be called exactly once for device.
/// @param env Node environment
/// @param info Device addr as optional arg
/// @return nothing
napi_value cb_vl53l5cx_comms_init(napi_env env, napi_callback_info info) {
    napi_value argv[MAX_ARGUMENTS];
    napi_value this;
    size_t argc = MAX_ARGUMENTS;

    void* data = NULL;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, NULL, &data);
    if (status != napi_ok) {
        napi_throw_error(env, UNKNOWN_ERROR, "cb_vl53l5cx_comms_init");
        return NULL;
    }

    // cfg slot
    uint32_t device_ndx;
    status = napi_get_value_uint32(env, argv[0], &device_ndx);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR, "Couldn't get cfg_slot");
        return NULL;
    }
    uint32_t bus = 0;
    if (argc > 1) {
        // bus number
        status = napi_get_value_uint32(env, argv[1], &bus);
        if (status != napi_ok) {
            napi_throw_error(env, ARGUMENT_ERROR, "Couldn't get bus nr");
            return NULL;
        }
    }

    // Open correct device file
    char i2c_device_file[20];
    snprintf(i2c_device_file, 20, "/dev/i2c-%d", bus);
    int fd = open(i2c_device_file, O_RDWR);
    if (fd < 0) {
        perror("cb_vl53l5cx_comms_init");
        napi_throw_error(env, "Error opening file",
                         "Can't open file at cb_vl53l5cx_comms_init");
        return NULL;
    }

    VL53L5CX_Configuration* config =
        ((VL53L5CX_Configuration*)data) + device_ndx;

    config->platform.fd = fd;

    uint8_t comms_awry = vl53l5cx_comms_init(&config->platform);
    if (comms_awry) {
        char err[MAX_LEN_ERROR] = {0};
        snprintf(err, MAX_LEN_ERROR,
                 "couldn't establish comms with vl53l5cx at %hx (ndx %u)",
                 config->platform.address, device_ndx);
        napi_throw_error(env, INIT_COMMS_ERROR, err);
    }
    return NULL;
}


/***********
 * Is alive?
 */

/// @brief Is the sensor alive? In case of a problem throws an error.
///        Sensor may have multiple errors, but _only one_ error will be thrown.
///        For such cases see `drv_status` and compare to the error values in
///        `vl53lcx_api.h` in the Linux driver.
/// @param env Node environment
/// @param info 
/// @return Nothing
napi_value cb_vl53l5cx_is_alive(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    uint8_t is_alive = 0;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t device_ndx = 0;
    napi_get_value_uint32(env, argv[0], &device_ndx);

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + device_ndx;
    

    // Call the driver's relevant function.
    uint8_t drv_status = vl53l5cx_is_alive(conf, &is_alive);

	if(!is_alive || drv_status) {
        char err[MAX_LEN_ERROR] = {0};
        snprintf(
            err,
            MAX_LEN_ERROR-1, 
            "VL53L5CX not detected at address 0x%hhx",
            conf->platform.address
        );
        napi_throw_error(
            env,
            NO_SENSOR,
            err
        );
    }
    return NULL;
}


/***************
 * Start ranging
 */

 napi_value cb_vl53l5cx_start_ranging(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t device_ndx = 0;
    status = napi_get_value_uint32(env, argv[0], &device_ndx);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Must give device index (0-9) as argument. fn: cb_vl53l5cx_start_ranging"
        );
        return NULL;
    }
    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + device_ndx;
    
    vl53l5cx_start_ranging(conf);
    return NULL;
 }


/**
 * vl53l5cx_check_data_ready
 */

 napi_value cb_vl53l5cx_check_data_ready(
    napi_env env,
    napi_callback_info info
) {
    size_t argc = MAX_ARGUMENTS;
    napi_value this;
    void* data;
    napi_value ret_val;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t device_ndx = 0;
    napi_get_value_uint32(env, argv[0], &device_ndx);
    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + device_ndx;

    uint8_t is_ready = 0;
    vl53l5cx_check_data_ready(conf, &is_ready);
    is_ready
        ? napi_get_boolean(env, true, &ret_val)
        : napi_get_boolean(env, false, &ret_val);

    return ret_val;
}

/**
 *  Stop ranging
 */

napi_value cb_vl53l5cx_stop_ranging(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t device_ndx = 0;
    napi_get_value_uint32(env, argv[0], &device_ndx);
    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + device_ndx;
    

    vl53l5cx_stop_ranging(conf);
    return NULL;
}

/**
 * Get ranging data
 */
napi_value cb_vl53l5cx_get_ranging_data(
    napi_env env,
    napi_callback_info info
) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t device_ndx = 0;
    napi_get_value_uint32(env, argv[0], &device_ndx);
    VL53L5CX_Configuration* config = 
        ((VL53L5CX_Configuration*) data) + device_ndx;
    

    /* Returned object */
    VL53L5CX_ResultsData results = {0};
    vl53l5cx_get_ranging_data(config, &results);
    napi_value ret_results = NULL;
    napi_create_object(env, &ret_results);

    /* siliconTempC prop */
    napi_value silicon_temp_c = NULL;
    status = napi_create_int32(
        env, 
        (int32_t)results.silicon_temp_degc, 
        &silicon_temp_c
    );
    if (status != napi_ok) {
        napi_throw_error(
            env,
            VALUE_NAPI_ERROR,
            "Couldn't create an int32. fn: cb_vl53l5cx_get_ranging_data"
        );
        return NULL;
    }
    napi_set_named_property(env, ret_results, "chipTempC", silicon_temp_c);

    /* Scan zones */
    napi_value scan_zones = NULL;
    uint8_t resolution;
    uint8_t rm_status = vl53l5cx_get_resolution(config, &resolution);
    // If resolution is not set to the device, it defaults to 16 zones.
    if (!resolution) resolution = VL53L5CX_RESOLUTION_4X4;
    if (rm_status) {
        napi_throw_error(
            env, ERROR, 
        "Couldn't get ranging resolution in fn: cb_vl53l5cx_get_ranging_data"
        );
        return NULL;
    }
    status = napi_create_array_with_length(
        env,
        resolution, /* 4*4 or 8*8 */
        &scan_zones
    );
    if (status != napi_ok) {
        napi_throw_error(
            env,
            VALUE_NAPI_ERROR,
            "Couldn't create an array. fn: cb_vl53l5cx_get_ranging_data"
        );
        return NULL;
    }

    /* Populate scan zones array */
    for (uint32_t i = 0; i < resolution; i++) {
        /* Create the zone */
        napi_value zone = NULL;
        status = napi_create_object(env, &zone);
        if (status != napi_ok) {
            napi_throw_error(
                env,
                VALUE_NAPI_ERROR,
                "Couldn't create napi object. fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }

        /* Zone index */
        napi_value zone_index = NULL;
        status = napi_create_uint32(env, i, &zone_index);
        if (status != napi_ok) {
            napi_throw_error(
                env,
                VALUE_NAPI_ERROR,
                "Couldn't create napi object. fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
        napi_set_named_property(env, zone, "zoneIndex", zone_index);
                
        
        /* Measured distance */
        #ifndef VL53L5CX_DISABLE_DISTANCE_MM
        napi_value distance_mm = NULL;
        status = napi_create_int32(
            env, 
            results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i],
            &distance_mm);
        if (status != napi_ok) {
            napi_throw_error(
                env,
                VALUE_NAPI_ERROR,
                "Couldn't create napi int32. fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
        status = napi_set_named_property(
            env, 
            zone, 
            "distanceMillimeters", 
            distance_mm
        );
        if (status != napi_ok) {
            napi_throw_error(
                env,
                NAMED_PROPERTY_NOT_SET,
                "fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
        #endif

        /* Target status */
        #ifndef VL53L5CX_DISABLE_TARGET_STATUS
        napi_value target_status = NULL;
        status = napi_create_uint32(
            env,
            results.target_status[VL53L5CX_NB_TARGET_PER_ZONE*i],
            &target_status
        );
        if (status != napi_ok) {
            napi_throw_error(
                env,
                VALUE_NAPI_ERROR,
                "Couldn't create napi uint32. fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
        status = napi_set_named_property(env, zone, "status", target_status);
        if (status != napi_ok) {
            napi_throw_error(
                env,
                NAMED_PROPERTY_NOT_SET,
                "fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
        #endif

        // Number of targets in zone
        uint8_t targets_for_this_zone = results.nb_target_detected[i];
        napi_value targets;
        status = napi_create_uint32(
            env, (uint32_t) targets_for_this_zone, &targets);
        if (status != napi_ok) {
            napi_throw_error(
                env, VALUE_NAPI_ERROR, "fn: get_ranging_data"
            );
            return NULL;
        }
        status = napi_set_named_property(
            env, zone, "nbTargetDetected", targets);
        if (status != napi_ok) {
            napi_throw_error(
                env, NAMED_PROPERTY_NOT_SET, "fn: get_ranging_data");
            return NULL;
        }

        // Ambient per spad
        uint32_t ambient = results.ambient_per_spad[i];
        napi_value n_ambient;
        status = napi_create_uint32(env, ambient, &n_ambient);
        if (status != napi_ok) {
            napi_throw_error(env, VALUE_NAPI_ERROR, "fn: get_ranging_data");
            return NULL;
        }
        status = napi_set_named_property(
            env, zone, "ambientPerSpad", n_ambient);
        if (status != napi_ok) {
            napi_throw_error(
                env, NAMED_PROPERTY_NOT_SET, "fn: get_ranging_data");
            return NULL;
        }

        // Number of spads enabled
        uint32_t num_spads = results.nb_spads_enabled[i];
        napi_value n_num_spads;
        status = napi_create_uint32(env, num_spads, &n_num_spads);
        if (status != napi_ok) {
            napi_throw_error(env, VALUE_NAPI_ERROR, "fn: get_ranging_data");
            return NULL;
        }
        status = napi_set_named_property(
            env, zone, "nbSpadsEnabled", n_num_spads);
        if (status != napi_ok) {
            napi_throw_error(
                env, NAMED_PROPERTY_NOT_SET, "fn: get_ranging_data");
            return NULL;
        }

        // Add per target data
        napi_value targets_array;
        status = napi_create_array_with_length(
            env, VL53L5CX_NB_TARGET_PER_ZONE, &targets_array);
        if (status != napi_ok) {
            napi_throw_error(env, VALUE_NAPI_ERROR, "fn: get_ranging_data");
            return NULL;
        }
        for (size_t j=0; j < VL53L5CX_NB_TARGET_PER_ZONE; j++) {
            napi_value target;
            status = napi_create_object(env, &target);
            if (status != napi_ok) {
                napi_throw_error(env, VALUE_NAPI_ERROR, "fn: get_ranging_data");
                return NULL;
            }
            const uint16_t idx = VL53L5CX_NB_TARGET_PER_ZONE*i+j;
            uint8_t target_status_c = results.target_status[idx];
            int16_t distance_millimeters_c = results.distance_mm[idx];
            uint32_t signal_per_spad_c = results.signal_per_spad[idx];
            uint16_t range_sigma_mm_c = results.range_sigma_mm[idx];
            napi_value target_status;
            napi_value distance_millimeters;
            napi_value signal_per_spad;
            napi_value range_sigma_millimeters;
            status = napi_ok; // 0
            status |= napi_create_uint32(
                env, (uint32_t) target_status_c, &target_status);
            status |=  napi_create_int32(
                env, (int32_t) distance_millimeters_c, &distance_millimeters);
            status |= napi_create_uint32(
                env, signal_per_spad_c, &signal_per_spad);
            status |= napi_create_uint32(
                env, (uint32_t) range_sigma_mm_c, &range_sigma_millimeters);
            if (status != napi_ok) {
                napi_throw_error(env, VALUE_NAPI_ERROR, "fn: get_ranging_data");
                return NULL;
            }
            status = napi_set_named_property(
                env, target, "targetStatus", target_status);
            status |= napi_set_named_property(
                env, target, "distanceMillimeters", distance_millimeters);
            status |= napi_set_named_property(
                env, target, "signalPerSpad", signal_per_spad);
            status |= napi_set_named_property(
                env, target, "rangeSigmaMillimeters", range_sigma_millimeters);
            if (status != napi_ok) {
                napi_throw_error(
                    env, NAMED_PROPERTY_NOT_SET, "fn: get_ranging_data");
                return NULL;
            }

            napi_set_element(env, targets_array, j, target);
        }
        status = napi_set_named_property(env, zone, "targetData", targets_array);
        if (status != napi_ok) {
            napi_throw_error(
                env, NAMED_PROPERTY_NOT_SET, "fn get_ranging_data");
        }

        /* Finally add the zone element to the zones array */
        status = napi_set_element(env, scan_zones, i, zone);
        if (status != napi_ok) {
            napi_throw_error(
                env,
                ARRAY_VALUE_NOT_SET,
                "fn: cb_vl53l5cx_get_ranging_data"
            );
            return NULL;
        }
    }

    /* Assign zones array to the results */
    napi_set_named_property(env, ret_results, "scanZones", scan_zones);
    return ret_results;
}

/**
 * Set scanning resolution
 */
napi_value cb_vl53l5cx_set_resolution(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t resolution;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid first argument for ..set_resolution(cfg, resolution)."
        );
        return NULL;
    }
    status = napi_get_value_uint32(env, argv[1], &resolution);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid second argument for ..set_resolution(cfg, resolution)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    if (resolution != VL53L5CX_RESOLUTION_8X8
        && resolution != VL53L5CX_RESOLUTION_4X4) {
        napi_throw_error(
            env,
            ARGUMENT_ERROR,
            "Resolution must be one of VL53L5CX_RESOLUTION_*."
        );
        return NULL;
    }

    uint8_t res_status = vl53l5cx_set_resolution(conf, (uint8_t) resolution);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensor resolution."
        );
    }
    return NULL;
}

/**
 * Set scanning frequency
 */
napi_value cb_vl53l5cx_set_ranging_frequency_hz(
    napi_env env, 
    napi_callback_info info
) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t frequency;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid first argument for ..set_frequency_hz(cfg, resolution)."
        );
        return NULL;
    }
    status = napi_get_value_uint32(env, argv[1], &frequency);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid second argument for ..set_frequency_hz(cfg, resolution)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    // Need resolution to check the input frequency is sane.
    uint8_t resolution = 0;
    vl53l5cx_get_resolution(conf, &resolution);
    if (((VL53L5CX_RESOLUTION_8X8 == (uint8_t) resolution)
        && (frequency < 1 || frequency > 15))
        ||
        ((VL53L5CX_RESOLUTION_4X4 == (uint8_t) resolution)
        && (frequency < 1 || frequency > 60))) {
        napi_throw_error(
            env,
            ARGUMENT_ERROR,
            "Impossible frequency for the given resolution."
        );
        return NULL;
    }

    uint8_t res_status = vl53l5cx_set_ranging_frequency_hz(
        conf, (uint8_t) frequency);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensing frequency."
        );
    }
    return NULL;
}

/** 
 * Target order selection. 
 */
napi_value cb_vl53l5cx_set_target_order(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t target_order;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid first argument for ..set_target_order(cfg, resolution)."
        );
        return NULL;
    }
    status = napi_get_value_uint32(env, argv[1], &target_order);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid second argument for ..set_target_order(cfg, resolution)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    if (target_order != VL53L5CX_TARGET_ORDER_CLOSEST
            && target_order != VL53L5CX_TARGET_ORDER_STRONGEST) {
        napi_throw_error(
            env,
            ARGUMENT_ERROR,
            "Target order must be one of VL53L5CX_TARGET_ORDER_* constants."
        );
        return NULL;
    }

    uint8_t res_status = vl53l5cx_set_target_order(conf, (uint8_t)target_order);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensor target ordering."
        );
    }
    return NULL;
}

napi_value cb_vl53l5cx_set_ranging_mode(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t mode;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid first argument for ..set_ranging_mode(cfg, mode)."
        );
        return NULL;
    }
    status = napi_get_value_uint32(env, argv[1], &mode);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR, 
            "Invalid second argument for ..set_ranging_mode(cfg, mode)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    if (mode != VL53L5CX_RANGING_MODE_CONTINUOUS
            && mode != VL53L5CX_RANGING_MODE_AUTONOMOUS) {
        napi_throw_error(
            env,
            ARGUMENT_ERROR,
            "Target order must be one of VL53L5CX_RANGING_MODE_* constants."
        );
        return NULL;
    }

    uint8_t res_status = vl53l5cx_set_ranging_mode(conf, (uint8_t)mode);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensor mode."
        );
    }
    return NULL;
}

napi_value cb_vl53l5cx_init(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid first argument for init(cfg)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    uint8_t res_status = vl53l5cx_init(conf);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensor mode."
        );
    }
    return NULL;
}

napi_value cb_vl53l5cx_get_integration_time_ms(
    napi_env env, napi_callback_info info
) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid first argument for init(cfg)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    uint32_t it_ms;
    uint8_t res_status = vl53l5cx_get_integration_time_ms(conf, &it_ms);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_GETTING_VALUE,
            "Couldn't read integration time."
        );
        return NULL;
    }

    napi_value ret_val;
    status = napi_create_uint32(env, it_ms, &ret_val);
    if (status != napi_ok) {
        napi_throw_error(
            env,
            VALUE_NAPI_ERROR,
            "Couldn't create uint32 in cb_vl53l5cx_get_integration_time_ms"
        );
        return NULL;
    }

    return ret_val;
}

napi_value cb_vl53l5cx_set_power_mode(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t power_mode;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid first argument for set_power_mode(cfg, mode)."
        );
        return NULL;
    }
    status = napi_get_value_uint32(env, argv[1], &power_mode);
    if (status != napi_ok 
        || (power_mode != VL53L5CX_POWER_MODE_SLEEP 
            && power_mode != VL53L5CX_POWER_MODE_WAKEUP)) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid second argument for set_power_mode(cfg, mode)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    uint8_t res_status = vl53l5cx_set_power_mode(conf, (uint8_t) power_mode);
    if (res_status) {
        napi_throw_error(
            env,
            ERROR_CHANGING_SETTING,
            "Couldn't change sensor power mode."
        );
    }

    return NULL;
}
napi_value cb_vl53l5cx_calibrate_xtalk(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 4, 4
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;
    uint32_t reflectance_percent; // uint16_t
    uint32_t nb_samples; // uint8_t
    uint32_t distance_mm; // uint16_t

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    status |= napi_get_value_uint32(env, argv[1], &reflectance_percent);
    status |= napi_get_value_uint32(env, argv[2], &nb_samples);
    status |= napi_get_value_uint32(env, argv[3], &distance_mm);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
"Invalid argument for calibrate_xtalk(cfg, reflectance, n_samples, distance)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    status = vl53l5cx_calibrate_xtalk(
        conf, (uint16_t) reflectance_percent,
        (uint8_t) nb_samples, (uint16_t) distance_mm
    );
    if (status) {
        char err[MAX_LEN_ERROR] = {0};
        snprintf(err, MAX_LEN_ERROR, "Status: 0x%X", status);
        napi_throw_error(
            env, XTALK_CALIBRATION_FAILED, err
        );
    }
    return NULL;
}

napi_value cb_vl53l5cx_get_caldata_xtalk(
    napi_env env, napi_callback_info info
) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 1
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, 
            ARGUMENT_ERROR,
            "Invalid argument for get_caldata_xtalk(cfg)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf = 
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    void* xtalk_data;
    napi_value xtalk_data_arraybuf;
    napi_create_arraybuffer(
        env, VL53L5CX_XTALK_BUFFER_SIZE, &xtalk_data, &xtalk_data_arraybuf
    );
    status = vl53l5cx_get_caldata_xtalk(conf, (uint8_t*) xtalk_data);
    if (status) {
        char err[MAX_LEN_ERROR] = {0};
        snprintf(err, MAX_LEN_ERROR, "Status: 0x%X", status);
        napi_throw_error(
            env, XTALK_CANNOT_FETCH_DATA, err
        );
        return NULL;
    }
    return xtalk_data_arraybuf;
}

napi_value cb_vl53l5cx_set_caldata_xtalk(
    napi_env env, napi_callback_info info
) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 2, 2
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, ARGUMENT_ERROR, "Invalid argument for get_caldata_xtalk(cfg)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf =
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    uint8_t* xtalk_calib;
    size_t xcalib_len;

    status = napi_get_arraybuffer_info(
        env, argv[1], (void**) &xtalk_calib, &xcalib_len);
    if (status) {
        napi_throw_error(
            env, ARGUMENT_ERROR, "fn: set_caldata_xtalk"
        );
        return NULL;
    } else if (xcalib_len != VL53L5CX_XTALK_BUFFER_SIZE) {
        napi_throw_error(
            env, ARGUMENT_ERROR,
            "ArrayBuffer size is not right. fn: set_caldata_xtalk"
        );
        return NULL;
    }
    status = vl53l5cx_set_caldata_xtalk(conf, xtalk_calib);
    if (status) {
        napi_throw_error(
            env, ERROR_CHANGING_SETTING,
            "Couldn't set xtalk calibration. fn: set_caldata_xtalk"
        );
    }
    return NULL;
}

napi_value cb_get_shape_and_kcps_xtalk(napi_env env, napi_callback_info info) {
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void* data;
    napi_status status;
    napi_value argv[MAX_ARGUMENTS] = {NULL};

    bool success = parse_args(
        env, info, &argc, argv, &this, &data, 1, 3
    );
    if (!success) {
        return NULL;
    }

    uint32_t cfg_slot;

    status = napi_get_value_uint32(env, argv[0], &cfg_slot);
    if (status != napi_ok) {
        napi_throw_error(
            env, ARGUMENT_ERROR, "Invalid argument for get_caldata_xtalk(cfg)."
        );
        return NULL;
    }

    VL53L5CX_Configuration* conf =
        ((VL53L5CX_Configuration*) data) + cfg_slot;

    uint8_t xtalk_data[VL53L5CX_XTALK_BUFFER_SIZE];
    uint16_t xtalk_shape_bins[144]; // 12*12


}
