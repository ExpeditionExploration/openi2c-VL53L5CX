#ifndef __FUNCS_H
#define __FUNCS_H

#include <string.h>
#include <node_api.h>
#include <vl53l5cx_api.h>

#include "error.h"


/******************
 * Initialize comms
 */

/// @brief Initialize comms to the sensor.
/// @param conf is always the start of the static CONFS-array.
/// @param env 
/// @param info Accepts I2C device address as optional parameter.
/// @return nothing
napi_value cb_vl53l5cx_comms_init(napi_env env, napi_callback_info info);

/*******************************
 * Check if the sensor is alive.
 */

/// @brief Check whether the sensor is alive.
/// @param env Node environment
/// @param info Call paramas and what-not
/// @return boolean on the status
napi_value cb_vl53l5cx_is_alive(napi_env env, napi_callback_info info);

/***************
 * Start ranging
 */

napi_value cb_vl53l5cx_start_ranging(napi_env env, napi_callback_info info);

 /**
  * Check if sensor has new data
  */
napi_value cb_vl53l5cx_check_data_ready(napi_env, napi_callback_info);

/**
 * @brief Stop ranging
 * @param napi_env environment
 * @param napi_callback_info
 * @return Nothing
 * @throw on error
 */
napi_value cb_vl53l5cx_stop_ranging(napi_env, napi_callback_info);

/**
 * Get ranging data
 */
napi_value cb_vl53l5cx_get_ranging_data(napi_env, napi_callback_info);

/**
 * Set resolution
 * 
 * Takes config slot and resolution as arguments.
 */
napi_value cb_vl53l5cx_set_resolution(napi_env, napi_callback_info);

/**
 * Set scanning frequency
 * 
 * Takes config slot and frequency as arguments.
 */
napi_value cb_vl53l5cx_set_ranging_frequency_hz(napi_env, napi_callback_info);

/**
 * Strongest or closest target selection
 * 
 * Takes config slot and order as arguments.
 */
napi_value cb_vl53l5cx_set_target_order(napi_env, napi_callback_info);

/**
 * Switch between autonomous or continuous ranging
 */
napi_value cb_vl53l5cx_set_ranging_mode(napi_env, napi_callback_info);

napi_value cb_vl53l5cx_init(napi_env, napi_callback_info);

napi_value cb_vl53l5cx_set_power_mode(napi_env, napi_callback_info);
napi_value cb_vl53l5cx_get_integration_time_ms(napi_env, napi_callback_info);
napi_value cb_vl53l5cx_calibrate_xtalk(napi_env, napi_callback_info);
napi_value cb_vl53l5cx_get_caldata_xtalk(napi_env, napi_callback_info);
napi_value cb_vl53l5cx_set_caldata_xtalk(napi_env, napi_callback_info);
napi_value cb_get_shape_and_kcps_xtalk(napi_env, napi_callback_info);

bool register_fn(
    VL53L5CX_Configuration* conf, napi_env env, napi_value exports,
    const char* fn_name, napi_callback cb
);
#endif
