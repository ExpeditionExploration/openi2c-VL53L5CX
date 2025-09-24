import { bindings as vl53l5cx } from "..";

export async function sleep(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function example4() {
    const cfg = 0;
    vl53l5cx.comms_init(cfg, 3); // init comms
    vl53l5cx.init(cfg); // init device

    // Going to sleep
    vl53l5cx.set_power_mode(cfg, vl53l5cx.VL53L5CX_POWER_MODE_SLEEP);
    await sleep(5000);
    vl53l5cx.set_power_mode(cfg, vl53l5cx.VL53L5CX_POWER_MODE_WAKEUP);
    // Woke up

    vl53l5cx.start_ranging(cfg);

    for (let i = 0; i < 3; i++) {
        while (!vl53l5cx.check_data_ready(cfg)) {
            await sleep(5);
        }
        const data = vl53l5cx.get_ranging_data(cfg);
        console.log(`Chip temperature: ${data.chipTempC}C degrees`);
        for (let zone of data.scanZones) {
            console.log(zone);
        }
        sleep(5);
    }
    vl53l5cx.stop_ranging(cfg);
}

example4()
