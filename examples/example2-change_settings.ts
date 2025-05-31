import { bindings as vl53l5cx } from "..";

function example2() {
    const cfg = 0;
    vl53l5cx.comms_init(cfg);
    vl53l5cx.init(cfg); // init device

    // Change resolution to 8x8
    vl53l5cx.set_resolution(cfg, vl53l5cx.VL53L5CX_RESOLUTION_8X8);

    /* Set ranging frequency to 6Hz.
	 * Using 4x4, min frequency is 1Hz and max is 60Hz
	 * Using 8x8, min frequency is 1Hz and max is 15Hz
	 */
    vl53l5cx.set_ranging_frequency_hz(cfg, 6);

    /* Set target to closest */
    vl53l5cx.set_target_order(
        cfg, vl53l5cx.VL53L5CX_TARGET_ORDER_CLOSEST
    );
    console.log(`Integration time: ${vl53l5cx.get_integration_time_ms(cfg)}ms`)

    vl53l5cx.start_ranging(cfg);
    for (let i = 0; i < 10; i++) {
        while (!vl53l5cx.check_data_ready(cfg)) {
            const sab = new SharedArrayBuffer(4);
            const int32 = new Int32Array(sab);
            Atomics.wait(int32, 0, 0, 5); // Sleep for 5ms
        }
        const data = vl53l5cx.get_ranging_data(cfg);
        console.log(`Chip temperature: ${data.chipTempC}C degrees`);
        for (let zone of data.scanZones) {
            console.log(zone);
        }
    }
    vl53l5cx.stop_ranging(cfg);
}

example2()
