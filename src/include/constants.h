#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <vl53l5cx_api.h>
#include <vl53l5cx_plugin_detection_thresholds.h>
#include <vl53l5cx_plugin_motion_indicator.h>
#include <vl53l5cx_plugin_xtalk.h>

#include <stdlib.h>


/**
 * Constant to be registered using `register_constant(..)` must be one of
 * the types enumerated here.
 * 
 * - `CT_UTF_8_STRING` is actually a `const char*` that must be valid UTF-8.
 * - `CT_UINT8_INTEGER` is an unsigned 8-bit integer.
 */
typedef enum {
    CT_UTF_8_STRING,
    CT_UINT8_INTEGER,
    CT_UINT16_INTEGER,
    CT_UINT32_INTEGER,
} binding_constant_type_t;

/**
 * Register a constant to the exposed Nodejs module API.
 */
void register_constant(
    napi_env env,
    napi_value exports,
    const char* binding_name,
    binding_constant_type_t constant_type,
    const void* constant_data
) {
    napi_value val;
    napi_ref ref;

    switch (constant_type)
    {
    case CT_UTF_8_STRING:
        napi_create_string_utf8(
            env,
            constant_data,
            strlen(constant_data),
            &val
        );
        napi_set_named_property(
            env,
            exports,
            binding_name,
            val
        );
        break;

    case CT_UINT8_INTEGER:
        // TODO: Check for OOM and what not.
        napi_create_uint32(
            env, 
            *(uint8_t*) constant_data, 
            &val
        );
        napi_set_named_property(env, exports, binding_name, val);
        napi_create_reference(env, val, 1, &ref);
        break;

    case CT_UINT16_INTEGER:
        // TODO: Check for OOM and what not.
        napi_create_uint32(
            env, 
            *(uint16_t*) constant_data, 
            &val
        );
        napi_set_named_property(env, exports, binding_name, val);
        napi_create_reference(env, val, 1, &ref);
        break;
    
    case CT_UINT32_INTEGER:
        // TODO: Check for OOM and what not.
        napi_create_uint32(
            env, 
            *(uint32_t*) constant_data, 
            &val
        );
        napi_set_named_property(env, exports, binding_name, val);
        napi_create_reference(env, val, 1, &ref);
        break;

    default:
        break;
    }

}

#endif