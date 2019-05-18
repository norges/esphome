#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace tsl2561 {

/** Enum listing all conversion/integration time settings for the VEML6075
 *
 * Higher values mean more accurate results, but will take more energy/more time.
 */
enum VEML6075IntegrationTime {
  VEML6075_INTEGRATION_14MS = 0b00,
  VEML6075_INTEGRATION_101MS = 0b01,
  VEML6075_INTEGRATION_402MS = 0b10,
};

/** Enum listing all gain settings for the VEML6075.
 *
 * Higher values are better for low light situations, but can increase noise.
 */
enum VEML6075Gain {
  VEML6075_GAIN_1X = 0,
  VEML6075_GAIN_16X = 1,
};

/// This class includes support for the VEML6075 i2c ambient light sensor.
class VEML6075Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  /** Set the time that sensor values should be accumulated for.
   *
   * Longer means more accurate, but also mean more power consumption.
   *
   * Possible values are:
   *
   *  - `sensor::VEML6075_INTEGRATION_14MS`
   *  - `sensor::VEML6075_INTEGRATION_101MS`
   *  - `sensor::VEML6075_INTEGRATION_402MS` (default)
   *
   * @param integration_time The new integration time.
   */
  void set_integration_time(VEML6075IntegrationTime integration_time);

  /** Set the internal gain of the sensor. Can be useful for low-light conditions
   *
   * Possible values are:
   *
   *  - `sensor::VEML6075_GAIN_1X` (default)
   *  - `sensor::VEML6075_GAIN_16X`
   *
   * @param gain The new gain.
   */
  void set_gain(VEML6075Gain gain);

  /** The "CS" package of this sensor has a slightly different formula for
   * converting the raw values. Use this setting to indicate that this is a CS
   * package. Defaults to false (not a CS package)
   *
   * @param package_cs Is this a CS package.
   */
  void set_is_cs_package(bool package_cs);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  bool tsl2561_read_byte(uint8_t a_register, uint8_t *value);
  bool tsl2561_read_uint(uint8_t a_register, uint16_t *value);
  bool tsl2561_write_byte(uint8_t a_register, uint8_t value);

 protected:
  float get_integration_time_ms_();
  void read_data_();
  float calculate_lx_(uint16_t ch0, uint16_t ch1);

  VEML6075IntegrationTime integration_time_{VEML6075_INTEGRATION_402MS};
  VEML6075Gain gain_{VEML6075_GAIN_1X};
  bool package_cs_{false};
};

}  // namespace tsl2561
}  // namespace esphome
