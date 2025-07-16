
const binding = require('bindings');
import { type VL53L5CX } from "./types"
import { ScanData, ScanZone, ScanTarget } from "./types"
export const bindings: VL53L5CX = binding('vl53l5cx_native');
export { ScanData, ScanZone, ScanTarget }
