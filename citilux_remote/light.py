from homeassistant.components.light import LightEntity, ATTR_BRIGHTNESS, ATTR_COLOR_TEMP, ATTR_HS_COLOR, SUPPORT_BRIGHTNESS, SUPPORT_COLOR_TEMP, SUPPORT_COLOR
from asyncio import sleep
from datetime import datetime
import logging
import voluptuous as vol

log = logging.getLogger(__name__)
BRIDGE = "hiper_iot_ir_v2_ir_send"
DOMAIN = "citilux"
SERVICE_NAME = "ir_lamp"
FIRE_DELAY = 0.2

SERVICE_TURN_ON_SCHEMA = vol.Schema({
    vol.Optional(ATTR_BRIGHTNESS): vol.All(vol.Coerce(int), vol.Range(min=0, max=255)),
})

def setup_platform(hass, config, add_entities, discovery_info):
    add_entities([IRLight(hass)])

IR_CODES = {
    "ON": "0000 006D 0000 0022 0153 00AB 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0474",
    "OFF": "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474",
    "bri+": "0000 006D 0022 0000 0153 00AD 0016 0015 0016 0015 0016 0015 0016 0015 0016 0016 0016 0041 0016 0015 0016 0041 0016 0040 0016 003F 0017 003F 0016 0042 0015 0040 0016 0017 0015 0041 0015 0016 0015 0042 0015 0016 0015 0016 0015 0040 0016 0041 0016 0015 0016 0041 0015 0016 0016 0015 0016 0041 0014 0042 0014 0016 0015 0016 0015 0042 0015 0016 0015 0041 0015 0181 06C3",
    "bri-": "0000 006D 0000 0022 0154 00A9 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0474",
    "cold": "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474",
    "warm": "0000 006D 0022 0000 0155 00AC 0015 0015 0016 0015 0016 0015 0017 0015 0016 0015 0016 003F 0017 0015 0017 003F 0016 0041 0016 0041 0016 0041 0016 0040 0016 0040 0016 0015 0016 0040 0017 0015 0016 0015 0016 003F 0017 003F 0016 0016 0015 0041 0016 0016 0015 0041 0016 0016 0016 0040 0016 0016 0015 0017 0015 0042 0015 0016 0015 0041 0015 0016 0015 0042 0014 0181 06C3",
    "max": "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474",
    "M1": "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 0474",
    "M2": "0000 006D 0000 0022 0153 00A8 0018 0016 0018 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0018 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474",
    "M3": "0000 006D 0000 0022 0154 00A8 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474",
    "M4": "0000 006D 0000 0022 0152 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0018 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0474",
    "RGB": "0000 006D 0000 0022 0152 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474"
}

class IRLight(LightEntity):
    def __init__(self, hass):
        super().__init__()
        self._hass = hass
        self._state = False
        self._brightness = 100
        self._hs = (175, 100)
        self._ct = 128
        self._room = None
        self._attr_unique_id = "tuya_ir_light"

    @property
    def should_poll(self):
        return False

    @property
    def supported_features(self):
        return SUPPORT_BRIGHTNESS | SUPPORT_COLOR_TEMP | SUPPORT_COLOR

    @property
    def name(self):
        return "Люстра"

    @property
    def brightness(self):
        return self._brightness

    async def set_brightness(self, value):
        if value == 255:
            await self.send_ir("max")
            self._brightness = value
            self.async_write_ha_state()
            return
        elif 128 < value < 255:
            steps = round((255 - value) / 19.6153846154)
            base = "max"
        elif value < 40:
            steps = 0
            base = "M1"
        else:
            steps = round((128 - value) / 19.6153846154)
            base = "M2"
        log.info("brightness: %s, steps: %s, from: %s", value, steps, base)
        await self.send_ir(base)
        await self.send_ir("bri-", repeat_times=steps)
        self._brightness = value
        self.async_write_ha_state()

    @property
    def hs_color(self):
        return self._hs

    async def set_hs_color(self, value):
        log.info("hs_color: %s", value)
        steps = self.convert_to_impulses(value)
        if steps == 6:
            await self.send_ir("M4")
        else:
            await self.send_ir("ON")
            await self.send_ir("RGB", repeat_times=steps)
        self._hs = value
        self.async_write_ha_state()

    @property
    def color_temp(self):
        return self._ct

    async def set_color_temp(self, value):
        log.info("color_temp: %s", value)
        if value > 327:
            log.info("broken range")
            return
        steps = round((value - 155) / 10.1470588235)
        if steps > 14:
            await self.send_ir("OFF")
            await self.send_ir("cold")
        await self.send_ir("max")
        await self.send_ir("cold", repeat_times=steps)
        self._ct = value
        self.async_write_ha_state()

    @property
    def is_on(self):
        return self._state

    def parse_color(self, rgb):
        return tuple(map(lambda x: round(float(x)), rgb))

    def convert_to_impulses(self, hs):
        hue = hs[0]
        if hue < 10 or hue > 355:   # красный
            return 3
        if 10 < hue < 71:           # оранжевый
            return 4
        elif hue < 140:             # зелёный
            return 6
        elif hue < 160:             # салатовый
            return 5
        elif hue < 212:             # голубой
            return 7
        elif hue < 263:             # синий
            return 1
        elif hue < 325:             # фиолетовый
            return 2
        else:
            return 3

    async def async_turn_on(self, **kwargs):
        self._state = True
        log.info("turn_on: %s", kwargs)
        fire = False
        if ATTR_BRIGHTNESS in kwargs:
            await self.set_brightness(kwargs[ATTR_BRIGHTNESS])
            fire = True
        if ATTR_HS_COLOR in kwargs:
            await self.set_hs_color(kwargs[ATTR_HS_COLOR])
            fire = True
        if ATTR_COLOR_TEMP in kwargs:
            await self.set_color_temp(kwargs[ATTR_COLOR_TEMP])
            fire = True
        if not fire:
            now = datetime.now()
            if 1 <= now.hour <= 7:
                await self.send_ir("bri-")
            # elif now.hour >= 19 or now.hour < 1:
                # await self.send_ir("warm", repeat_times=4)
            elif 7 <= now.hour < 13:
                await self.send_ir("cold")
            else:
                await self.send_ir("max")
        self.async_write_ha_state()

    async def async_turn_off(self):
        self._state = False
        await self.send_ir("OFF")
        self.async_write_ha_state()

    async def send_ir(self, code, repeat_times=1, wait_time=0.2):
        log.info("send_ir: %s", code)
        if code in ("bri+", "warm"):
            log.info("broken code, sending ON")
            code = "ON"
        for _ in range(repeat_times):
            await self.hass.services.async_call("esphome", BRIDGE, {"data": IR_CODES[code]})
            await sleep(wait_time)
