import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.sensor as esphome_sensor  # Alias to avoid circular import conflict
from esphome.const import CONF_ID, CONF_PIN, CONF_UPDATE_INTERVAL
from esphome import pins

custom_adc_ns = cg.esphome_ns.namespace("custom_adc")
CustomADCSensor = custom_adc_ns.class_("CustomADCSensor", esphome_sensor.Sensor, cg.PollingComponent)

# Enum for attenuation
ATTEN_OPTIONS = {
    "DB_0": "ADC_ATTEN_DB_0",
    "DB_2_5": "ADC_ATTEN_DB_2_5",
    "DB_6": "ADC_ATTEN_DB_6",
    "DB_11": "ADC_ATTEN_DB_11",
}

CONFIG_SCHEMA = esphome_sensor.sensor_schema(CustomADCSensor).extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(CustomADCSensor),
        cv.Required(CONF_PIN): pins.internal_gpio_input_pin_number,
        cv.Required("atten"): cv.enum(ATTEN_OPTIONS, upper=True),
        cv.Optional(CONF_UPDATE_INTERVAL, default="100ms"): cv.positive_time_period_milliseconds,
    }
).extend(cv.polling_component_schema("60s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await esphome_sensor.register_sensor(var, config)
    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_atten(cg.RawExpression(config["atten"])))
