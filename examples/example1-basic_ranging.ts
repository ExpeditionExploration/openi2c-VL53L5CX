import { bindings as vl53l5cx } from "..";

async function example1() {
    const cfg = 0;
    vl53l5cx.comms_init(cfg, 3); // init comms
    vl53l5cx.init(cfg); // init device
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
            process.stdout.write(`zone: ${zone.zoneIndex}, dist: ${zone.distanceMillimeters / 10}cm | `);
            if ((zone.zoneIndex + 1) % 4 == 0) {
                console.log()
            }
        }
    }
    vl53l5cx.stop_ranging(cfg);
}

example1()
