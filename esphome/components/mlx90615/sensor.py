import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, CONF_TEMPERATURE, \
    ICON_BRIEFCASE_DOWNLOAD, UNIT_METER_PER_SECOND_SQUARED, \
    ICON_SCREEN_ROTATION, UNIT_DEGREE_PER_SECOND, ICON_THERMOMETER, UNIT_CELSIUS

DEPENDENCIES = ['i2c']

CONF_TEMPERATURE = 'temperature'


mlx90615_ns = cg.esphome_ns.namespace('mlx90615')
MLX90615Component = mlx90615_ns.class_('MLX90615Component', cg.PollingComponent, i2c.I2CDevice)

temperature_schema = sensor.sensor_schema(UNIT_CELSIUS, ICON_THERMOMETER, 1)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MLX90615Component),
    cv.Optional(CONF_TEMPERATURE): temperature_schema,
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(0x5))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)

    if CONF_TEMPERATURE in config:
        sens = yield sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))
