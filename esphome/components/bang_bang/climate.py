import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import climate, sensor
from esphome.const import CONF_AWAY_CONFIG, CONF_COOL_ACTION, \
    CONF_DEFAULT_TARGET_TEMPERATURE_HIGH, CONF_DEFAULT_TARGET_TEMPERATURE_LOW, CONF_HEAT_ACTION, \
    CONF_ID, CONF_IDLE_ACTION, CONF_SENSOR

bang_bang_ns = cg.esphome_ns.namespace('bang_bang')
BangBangClimate = bang_bang_ns.class_('BangBangClimate', climate.Climate)
BangBangClimateTargetTempConfig = bang_bang_ns.struct('BangBangClimateTargetTempConfig')

CONFIG_SCHEMA = cv.All(climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(BangBangClimate),
    cv.Required(CONF_SENSOR): cv.use_id(sensor.Sensor),
    cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_LOW): cv.temperature,
    cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_HIGH): cv.temperature,
    cv.Required(CONF_IDLE_ACTION): automation.validate_automation(single=True),
    cv.Optional(CONF_COOL_ACTION): automation.validate_automation(single=True),
    cv.Optional(CONF_HEAT_ACTION): automation.validate_automation(single=True),
    cv.Optional(CONF_AWAY_CONFIG): cv.Schema({
        cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_LOW): cv.temperature,
        cv.Required(CONF_DEFAULT_TARGET_TEMPERATURE_HIGH): cv.temperature,
    }),
}).extend(cv.COMPONENT_SCHEMA), cv.has_at_least_one_key(CONF_COOL_ACTION, CONF_HEAT_ACTION))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield climate.register_climate(var, config)

    sens = yield cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_sensor(sens))

    normal_config = BangBangClimateTargetTempConfig(
        config[CONF_DEFAULT_TARGET_TEMPERATURE_LOW],
        config[CONF_DEFAULT_TARGET_TEMPERATURE_HIGH]
    )
    cg.add(var.set_normal_config(normal_config))

    yield automation.build_automation(var.get_idle_trigger(), [], config[CONF_IDLE_ACTION])

    if CONF_COOL_ACTION in config:
        yield automation.build_automation(var.get_cool_trigger(), [], config[CONF_COOL_ACTION])
        cg.add(var.set_supports_cool(True))
    if CONF_HEAT_ACTION in config:
        yield automation.build_automation(var.get_heat_trigger(), [], config[CONF_HEAT_ACTION])
        cg.add(var.set_supports_heat(True))

    if CONF_AWAY_CONFIG in config:
        away = config[CONF_AWAY_CONFIG]
        away_config = BangBangClimateTargetTempConfig(
            away[CONF_DEFAULT_TARGET_TEMPERATURE_LOW],
            away[CONF_DEFAULT_TARGET_TEMPERATURE_HIGH]
        )
        cg.add(var.set_away_config(away_config))
