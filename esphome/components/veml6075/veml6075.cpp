#include "veml6075.h"
#include "esphome/core/log.h"

namespace esphome {
namespace veml6075 {

static const char *TAG = "veml6075";

static const uint8_t VEML6075_COMMAND_BIT = 0x80;
static const uint8_t VEML6075_WORD_BIT = 0x20;
static const uint8_t VEML6075_REGISTER_CONTROL = 0x00;
static const uint8_t VEML6075_REGISTER_TIMING = 0x01;
static const uint8_t VEML6075_REGISTER_ID = 0x0A;
static const uint8_t VEML6075_REGISTER_DATA_0 = 0x0C;
static const uint8_t VEML6075_REGISTER_DATA_1 = 0x0E;

void VEML6075Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up VEML6075...");
  uint8_t id;
  if (!this->veml6075_read_byte(VEML6075_REGISTER_ID, &id)) {
    this->mark_failed();
    return;
  }

  uint8_t timing;
  if (!this->veml6075_read_byte(VEML6075_REGISTER_TIMING, &timing)) {
    this->mark_failed();
    return;
  }

  timing &= ~0b00010000;
  timing |= this->gain_ == VEML6075_GAIN_16X ? 0b00010000 : 0;

  timing &= ~0b00000011;
  timing |= this->integration_time_ & 0b11;

  this->veml6075_write_byte(VEML6075_REGISTER_TIMING, timing);
}
void VEML6075Sensor::dump_config() {
  LOG_SENSOR("", "VEML6075", this);
  LOG_I2C_DEVICE(this);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with VEML6075 failed!");
  }

  int gain = this->gain_ == VEML6075_GAIN_1X ? 1 : 16;
  ESP_LOGCONFIG(TAG, "  Gain: %dx", gain);
  ESP_LOGCONFIG(TAG, "  Integration Time: %.1f ms", this->get_integration_time_ms_());

  LOG_UPDATE_INTERVAL(this);
}
void VEML6075Sensor::update() {
  // Power on
  if (!this->veml6075_write_byte(VEML6075_REGISTER_CONTROL, 0b00000011)) {
    this->status_set_warning();
    return;
  }

  // Make sure the data is there when we will read it.
  auto timeout = static_cast<uint32_t>(this->get_integration_time_ms_() + 20);

  this->set_timeout("illuminance", timeout, [this]() { this->read_data_(); });
}

float VEML6075Sensor::calculate_lx_(uint16_t ch0, uint16_t ch1) {
  if ((ch0 == 0xFFFF) || (ch1 == 0xFFFF)) {
    ESP_LOGW(TAG, "VEML6075 sensor is saturated.");
    return NAN;
  }

  float d0 = ch0, d1 = ch1;
  float ratio = d1 / d0;

  float ms = this->get_integration_time_ms_();
  d0 *= (402.0f / ms);
  d1 *= (402.0f / ms);

  if (this->gain_ == VEML6075_GAIN_1X) {
    d0 *= 16;
    d1 *= 16;
  }

  if (this->package_cs_) {
    if (ratio < 0.52f) {
      return 0.0315f * d0 - 0.0593f * d0 * powf(ratio, 1.4f);
    } else if (ratio < 0.65f) {
      return 0.0229f * d0 - 0.0291f * d1;
    } else if (ratio < 0.80f) {
      return 0.0157f * d0 - 0.0153f * d1;
    } else if (ratio < 1.30f) {
      return 0.00338f * d0 - 0.00260f * d1;
    }

    return 0.0f;
  } else {
    if (ratio < 0.5f) {
      return 0.0304f * d0 - 0.062f * d0 * powf(ratio, 1.4f);
    } else if (ratio < 0.61f) {
      return 0.0224f * d0 - 0.031f * d1;
    } else if (ratio < 0.80f) {
      return 0.0128f * d0 - 0.0153f * d1;
    } else if (ratio < 1.30f) {
      return 0.00146f * d0 - 0.00112f * d1;
    }
    return 0.0f;
  }
}
void VEML6075Sensor::read_data_() {
  uint16_t data1, data0;
  if (!this->veml6075_read_uint(VEML6075_WORD_BIT | VEML6075_REGISTER_DATA_1, &data1)) {
    this->status_set_warning();
    return;
  }
  if (!this->veml6075_read_uint(VEML6075_WORD_BIT | VEML6075_REGISTER_DATA_0, &data0)) {
    this->status_set_warning();
    return;
  }
  // Power off
  if (!this->veml6075_write_byte(VEML6075_REGISTER_CONTROL, 0b00000000)) {
    this->status_set_warning();
    return;
  }

  float lx = this->calculate_lx_(data0, data1);
  ESP_LOGD(TAG, "Got illuminance=%.1flx", lx);
  this->publish_state(lx);
  this->status_clear_warning();
}
float VEML6075Sensor::get_integration_time_ms_() {
  switch (this->integration_time_) {
    case VEML6075_INTEGRATION_14MS:
      return 13.7f;
    case VEML6075_INTEGRATION_101MS:
      return 100.0f;
    case VEML6075_INTEGRATION_402MS:
      return 402.0f;
  }
  return 0.0f;
}
void VEML6075Sensor::set_integration_time(VEML6075IntegrationTime integration_time) {
  this->integration_time_ = integration_time;
}
void VEML6075Sensor::set_gain(VEML6075Gain gain) { this->gain_ = gain; }
void VEML6075Sensor::set_is_cs_package(bool package_cs) { this->package_cs_ = package_cs; }
float VEML6075Sensor::get_setup_priority() const { return setup_priority::DATA; }
bool VEML6075Sensor::veml6075_write_byte(uint8_t a_register, uint8_t value) {
  return this->write_byte(a_register | VEML6075_COMMAND_BIT, value);
}
bool VEML6075Sensor::veml6075_read_uint(uint8_t a_register, uint16_t *value) {
  uint8_t data[2];
  if (!this->read_bytes(a_register | VEML6075_COMMAND_BIT, data, 2))
    return false;
  const uint16_t hi = data[1];
  const uint16_t lo = data[0];
  *value = (hi << 8) | lo;
  return true;
}
bool VEML6075Sensor::veml6075_read_byte(uint8_t a_register, uint8_t *value) {
  return this->read_byte(a_register | VEML6075_COMMAND_BIT, value);
}

}  // namespace veml6075
}  // namespace esphome
