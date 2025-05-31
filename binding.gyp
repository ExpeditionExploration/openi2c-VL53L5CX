{
    "targets": [{
        "target_name": "libvl53l5cx_driver",
        "type": "static_library",
        "sources": [
            "src/vl53l5cx-linux/user/uld-driver/src/vl53l5cx_api.c",
            "src/vl53l5cx-linux/user/uld-driver/src/vl53l5cx_plugin_detection_thresholds.c",
            "src/vl53l5cx-linux/user/uld-driver/src/vl53l5cx_plugin_motion_indicator.c",
            "src/vl53l5cx-linux/user/uld-driver/src/vl53l5cx_plugin_xtalk.c",
            "src/vl53l5cx-linux/user/platform/platform.c"
        ],
        "include_dirs": [
            "src/vl53l5cx-linux/user/uld-driver/inc/",
            "src/vl53l5cx-linux/user/platform/"
        ],
        "cflags!": [ 
            "-Wall",               # basic warnings
            "-Wextra",             # extra warnings
            "-Wpedantic",          # strict ISO compliance
            "-Wconversion",        # implicit conversions
            "-Wshadow",            # variable shadowing
            "-Wcast-align",        # alignment casts
            "-Wformat-security",   # printf/scanf security
            "-Wnull-dereference",  # null pointer checks
            "-Werror"              # turn *all* warnings into errors
        ],
    }, {
    "target_name": "vl53l5cx_native",
    "sources": [
        "src/binding.c",
        "src/error.c",
        "src/funcs.c"
    ],
    "include_dirs": [
        "src/include",
        "src/vl53l5cx-linux/user/platform/",
        "src/vl53l5cx-linux/user/uld-driver/inc/"
    ],
    "cflags!": [ 
        "-Wall",               # basic warnings
        "-Wextra",             # extra warnings
        "-Wpedantic",          # strict ISO compliance
        "-Wconversion",        # implicit conversions
        "-Wshadow",            # variable shadowing
        "-Wcast-align",        # alignment casts
        "-Wformat-security",   # printf/scanf security
        "-Wnull-dereference",  # null pointer checks
        "-Werror"              # turn *all* warnings into errors
    ],
    "dependencies": [ "libvl53l5cx_driver" ]
    }]
}
