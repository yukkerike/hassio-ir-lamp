import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, remote_transmitter
from esphome.const import CONF_OUTPUT_ID

CONF_TRANSMITTER_ID = "transmitter_id"
citilux_ir_ns = cg.esphome_ns.namespace("citilux_ir")
CitiluxIROutput = citilux_ir_ns.class_("CitiluxIROutput", cg.Component)

DEPENDENCIES = ["remote_transmitter"]
AUTO_LOAD = ["citilux_ir"]

CONFIG_SCHEMA = light.LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(CitiluxIROutput),
        cv.Required(CONF_TRANSMITTER_ID): cv.use_id(
            remote_transmitter.RemoteTransmitterComponent
        ),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)

    transmitter = await cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_transmitter(transmitter))
