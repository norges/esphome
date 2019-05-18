import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_GAIN, CONF_ID, CONF_INTEGRATION_TIME, UNIT_LUX, ICON_BRIGHTNESS_5

DEPENDENCIES = ['i2c']

veml6075_ns = cg.esphome_ns.namespace('veml6075')
VEML6075IntegrationTime = veml6075_ns.enum('VEML6075IntegrationTime')
INTEGRATION_TIMES = {
    14: VEML6075IntegrationTime.VEML6075_INTEGRATION_14MS,
    101: VEML6075IntegrationTime.VEML6075_INTEGRATION_101MS,
    402: VEML6075IntegrationTime.VEML6075_INTEGRATION_402MS,
}

VEML6075Gain = veml6075_ns.enum('VEML6075Gain')
GAINS = {
    '1X': VEML6075Gain.VEML6075_GAIN_1X,
    '16X': VEML6075Gain.VEML6075_GAIN_16X,
}

CONF_IS_CS_PACKAGE = 'is_cs_package'


def validate_integration_time(value):
    value = cv.positive_time_period_milliseconds(value).total_milliseconds
    return cv.enum(INTEGRATION_TIMES, int=True)(value)


VEML6075Sensor = veml6075_ns.class_('VEML6075Sensor', sensor.Sensor, cg.PollingComponent,
                                  i2c.I2CDevice)

CONFIG_SCHEMA = sensor.sensor_schema(UNIT_LUX, ICON_BRIGHTNESS_5, 1).extend({
    cv.GenerateID(): cv.declare_id(VEML6075Sensor),
    cv.Optional(CONF_INTEGRATION_TIME, default=402): validate_integration_time,
    cv.Optional(CONF_GAIN, default='1X'): cv.enum(GAINS, upper=True),
    cv.Optional(CONF_IS_CS_PACKAGE, default=False): cv.boolean,
}).extend(cv.polling_component_schema('60s')).extend(i2c.i2c_device_schema(0x39))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
    yield sensor.register_sensor(var, config)

    cg.add(var.set_integration_time(config[CONF_INTEGRATION_TIME]))
    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_is_cs_package(config[CONF_IS_CS_PACKAGE]))
