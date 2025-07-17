import { bindings as vl53l5cx } from "..";

export async function sleep(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function example5() {
    const cfg = 0;
    vl53l5cx.comms_init(cfg); // init comms
    vl53l5cx.init(cfg); // init device

    console.log("Sensor cross talk calibration starting..");
    try {
        vl53l5cx.calibrate_xtalk(
            cfg,
            3,  // Reflectance percent
            4,  // Samples
            600 // Distance to cover window
        );

        // Saving the calibration data.
        const xtalk_calibration = vl53l5cx.get_caldata_xtalk(cfg);

        // Storing the calibration data onto the sensor.
        vl53l5cx.set_caldata_xtalk(cfg, xtalk_calibration);
    } catch (e) {
        console.log(e);
        throw e;
    }

    vl53l5cx.start_ranging(cfg);

    for (let i = 0; i < 10; i++) {
        while (!vl53l5cx.check_data_ready(cfg)) {
            await sleep(5);
        }
        const data = vl53l5cx.get_ranging_data(cfg);
        console.log(`Chip temperature: ${data.chipTempC}C degrees`);
        for (let zone of data.scanZones) {
            console.log(zone);
        }
        await sleep(5);
    }
    vl53l5cx.stop_ranging(cfg);
}

example5()
