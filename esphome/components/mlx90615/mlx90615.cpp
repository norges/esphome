#include "mlx90615.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mlx90615 {

static const char *TAG = "mlx90615";

const uint8_t MLX90615_REGISTER_RAW_IR_DATA = 0x05;
const uint8_t MLX90615_REGISTER_TA_DATA = 0x06;
const uint8_t MLX90615_REGISTER_TO_IR_DATA = 0x07;
const uint8_t MLX90615_REGISTER_SLAVE_ADRESS = 0x00;
const uint8_t MLX90615_REGISTER_PMW_T_RANGE = 0x01;
const uint8_t MLX90615_REGISTER_CONFIG = 0x02;
const uint8_t MLX90615_REGISTER_EMISSIVITY = 0x03;
const uint8_t MLX90615_REGISTER_ID_NUMBER1 = 0x0e;
const uint8_t MLX90615_REGISTER_ID_NUMBER2 = 0x0f;


void MLX90615Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MLX90615...");

  ESP_LOGV(TAG, "  Setting up Config...");
  uint16_t config;

  config &= 0b000000000000000;
}

void MLX90615Component::update() {
  ESP_LOGV(TAG, "    Updating MLX90615...");
  // uint16_t raw_data[16];
  // if (!this->read_bytes_16(0x10, raw_data, 16)) {
  //   this->status_set_warning();
  //   return;
  // }
  uint8_t raw_data[2];
  Wire.beginTransmission(0x5b);
  Wire.write(0xB6);
  Wire.write(0x27);
  Wire.write(0xB7);
  raw_data[0] = Wire.read();
  raw_data[1] = Wire.read();
  ESP_LOGV(TAG, "raw0 0x%02X", raw_data[0]);
  ESP_LOGV(TAG, "raw1 0x%02X", raw_data[1]);
  Wire.endTransmission();
  
}


void MLX90615Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up MLX90615...");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with MLX90615 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

float MLX90615Component::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace mlx90615
}  // namespace esphome
