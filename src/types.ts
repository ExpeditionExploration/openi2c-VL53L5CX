export type ScanTarget = {
  targetStatus: number,
  distanceMillimeters: number,
  signalPerSpad: number,
  rangeSigmaMillimeters: number,
}
export type ScanZone = {
    zoneIndex: number;
    distanceMillimeters: number;
    status: number;
    /**
     * Targets in zone
     */ 
    nbTargetDetected: number;
    /**
     * kcps
     */
    ambientPerSpad: number;
    /**
     * Number of spads enabled
     */
    nbSpadsEnabled: number;
    targetData: ScanTarget[];
}
export type ScanData = {
    chipTempC: number;
    scanZones: ScanZone[];
}

type XtalkShapeAndStrength = {
  kiloCountsPerSecond: number[],
  xTalkShapeBin: number[]
}

export type VL53L5CX = {
  /**
   * Initialize communications with a sensor.
   * 
   * Initializes a device to config slot 0-9, as chosen by the caller. All
   * subsequent api calls for a sensor **must** use the same `cfg`.
   * 
   * @param cfg number 0-9
   * @returns nothing
   * @throws errors if something went awry
   */
  comms_init: (cfg: number) => undefined;

  /**
   * Check if sensor is alive.
   * 
   * @returns nothing
   * @throws errors if something went awry
   */
  is_alive: (cfg: number) => undefined;

  /**
   * Check if data is ready to be fetched.
   * 
   * @return true
   * @throws errors if something went awry
   */
  check_data_ready: (cfg: number) => boolean;

  /**
   * Start ranging
   * 
   * @return nothing
   * @throws errors if something went awry
   */
  start_ranging: (cfg: number) => undefined;

  /**
   * Stop ranging
   * 
   * @return nothing
   * @throws errors if something went awry
   */
  stop_ranging: (cfg: number) => undefined;

  /**
   * Fetch scan data.
   * 
   * @param cfg 
   * @returns {ScanData}
   * 
   * @throws `couldn't create napi value`, `unknown generic error`,
   *  `couldn't set named property`, `couldn't set array napi value`
   */
  get_ranging_data: (cfg: number) => ScanData;

  /**
   * Set scan resolution.
   * 
   * Other sensor parameters depend on this, so this **must** be the first one
   * to be set.
   * 
   * @param cfg config slot
   * @param resolution One of the `VL53L5CX_RESOLUTION_*` constants
   * @returns Nothing
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_resolution: (cfg: number, resolution: number) => undefined;

  /**
   *  Used as parameter for vl53l5cx_set_resolution(..)
   */
  VL53L5CX_RESOLUTION_4X4: number;
  /**
   *  Used as parameter for vl53l5cx_set_resolution(..)
   */
  VL53L5CX_RESOLUTION_8X8: number;

  /**
   * Set ranging frequency
   * 
   * **Must** be set after setting the resolution.
   * 
   * @param cfg cfg slot
   * @param freq Frequency in Hz
   * @returns undefined
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_ranging_frequency_hz: (cfg: number, freq: number) => undefined;

  /**
   * Set target ordering
   * 
   * **Must** be set after setting the resolution.
   * 
   * @param cfg cfg slot
   * @param ordering One of `VL53L5CX_TARGET_ORDER_*` constants.
   * @returns undefined
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_target_order: (cfg: number, ordering: number) => undefined;
  VL53L5CX_TARGET_ORDER_CLOSEST: number;
  VL53L5CX_TARGET_ORDER_STRONGEST: number;

  /**
   * Set ranging mode
   * 
   * @param cfg
   * @mode One of `VL53L5CX_RANGING_MODE_*` constants.
   * @returns undefined
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_ranging_mode: (cfg: number, mode: number) => undefined;
  VL53L5CX_RANGING_MODE_CONTINUOUS: number;
  VL53L5CX_RANGING_MODE_AUTONOMOUS: number;

  /**
   * Initialize device
   * 
   * @param cfg conf slot
   * @returns undefined
   */
  init: (cfg: number) => undefined;

  /**
   * 
   * @param cfg Get integration time of a scan frame.
   * @returns Time in ms
   * 
   * @throws `invalid arguments`,
   *         `can't read value from sensor`, 
   *         `couldn't create napi value`
   */
  get_integration_time_ms: (cfg: number) => number;
  /**
   * Change power to being awake or sleeping
   * 
   * @param cfg conf slot
   * @param mode One of `VL53L5CX_POWER_MODE_*` constants.
   * @returns undefined
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_power_mode: (cfg: number, mode: number) => undefined;
  VL53L5CX_POWER_MODE_SLEEP: number;
  VL53L5CX_POWER_MODE_WAKEUP: number;

  /**
   * Calibrate xtalk.
   * 
   * @param cfg config slot
   * @param reflectance_percent integer
   * @param num_samples 
   * @param distance_mm 
   * @returns undefined
   * 
   * @throws `invalid arguments`, `xtalk calibration failed`
   */
  calibrate_xtalk: (
    cfg: number,
    reflectance_percent: number,
    num_samples: number,
    distance_mm: number
  ) => undefined;

  /**
   * Fetch the stored xtalk calibration data
   * 
   * @param cfg cfg slot
   * @returns Arraybuffer
   * 
   * @throws `invalid arguments`, `cannot fetch xtalk calibration`
   */
  get_caldata_xtalk: (cfg: number) => ArrayBuffer;

  /**
   * Store the saved xtalk calibration data onto a sensor.
   * 
   * @param cfg cfg slot
   * @param calibration_data is `VL53L5CX_XTALK_BUFFER_SIZE` bytes long.
   * @returns undefined
   * 
   * @throws `invalid arguments`, `change setting error`
   */
  set_caldata_xtalk: (cfg: number, calibration_data: ArrayBuffer) => undefined;

  /**
   * Length of xtalk calibration data in bytes.
   */
  VL53L5CX_XTALK_BUFFER_SIZE: number;

  /**
   * 
   * @param cfg config slot
   * @param calibration_data ArrayBuffer as read from the sensor.
   * @param resolution One of `VL53L5CX_RESOLUTION_*` constants.
   * @returns 
   */
  get_shape_and_kcps_xtalk: (
    cfg: number,
    calibration_data: ArrayBuffer,
    resolution: number
  ) => XtalkShapeAndStrength
};
