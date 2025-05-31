import { bindings as vl53l5cx } from "..";

export async function sleep(ms:number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function example3() {
    const cfg = 0;
    vl53l5cx.comms_init(cfg); // init comms
    vl53l5cx.init(cfg); // init device

    vl53l5cx.set_ranging_mode(cfg, vl53l5cx.VL53L5CX_RANGING_MODE_AUTONOMOUS);
    console.log(`Integration time: ${vl53l5cx.get_integration_time_ms(cfg)}ms`)

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

example3()
