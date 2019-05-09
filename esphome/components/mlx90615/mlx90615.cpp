#include "mlx90615.h"
#include "esphome/core/log.h"

namespace esphome {
namespace custom {

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


void MLX90615Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up MLX90615...");

  ESP_LOGV(TAG, "  Setting up Config...");
  uint16_t config;

  config &= 

void MLX90615Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up MLX90615...");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with MLX90615 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

}  // namespace mlx90615
}  // namespace esphome
