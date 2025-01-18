#include "esphome.h"

using namespace esphome;

static const std::map<std::string, std::string> IR_CODES = {
    {"ON", "0000 006D 0000 0022 0153 00AB 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0474"},
    {"OFF", "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474"},
    {"bri+", "0000 006D 0022 0000 0153 00AD 0016 0015 0016 0015 0016 0015 0016 0015 0016 0016 0016 0041 0016 0015 0016 0041 0016 0040 0016 003F 0017 003F 0016 0042 0015 0040 0016 0017 0015 0041 0015 0016 0015 0042 0015 0016 0015 0016 0015 0040 0016 0041 0016 0015 0016 0041 0015 0016 0016 0015 0016 0041 0014 0042 0014 0016 0015 0016 0015 0042 0015 0016 0015 0041 0015 0181 06C3"},
    {"bri-", "0000 006D 0000 0022 0154 00A9 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0016 0016 0040 0016 0016 0016 0040 0016 0016 0016 0016 0016 0040 0016 0040 0016 0040 0016 0016 0016 0040 0016 0016 0016 0040 0016 0474"},
    {"cold", "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474"},
    {"warm", "0000 006D 0022 0000 0155 00AC 0015 0015 0016 0015 0016 0015 0017 0015 0016 0015 0016 003F 0017 0015 0017 003F 0016 0041 0016 0041 0016 0041 0016 0040 0016 0040 0016 0015 0016 0040 0017 0015 0016 0015 0016 003F 0017 003F 0016 0016 0015 0041 0016 0016 0015 0041 0016 0016 0016 0040 0016 0016 0015 0017 0015 0042 0015 0016 0015 0041 0015 0016 0015 0042 0014 0181 06C3"},
    {"max", "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474"},
    {"M1", "0000 006D 0000 0022 0153 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 0474"},
    {"M2", "0000 006D 0000 0022 0153 00A8 0018 0016 0018 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0018 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474"},
    {"M3", "0000 006D 0000 0022 0154 00A8 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 0474"},
    {"M4", "0000 006D 0000 0022 0152 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0018 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0474"},
    {"RGB", "0000 006D 0000 0022 0152 00AA 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 003F 0016 0016 0016 003F 0016 0016 0016 003F 0016 003F 0016 0016 0016 0016 0016 003F 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 0016 003F 0016 003F 0016 0016 0016 003F 0016 003F 0016 003F 0016 0474"}};

class CitiluxIROutput : public Component, public LightOutput
{
public:
    LightTraits get_traits() override
    {
        auto traits = LightTraits();
        traits.set_supported_color_modes({ColorMode::ON_OFF, ColorMode::BRIGHTNESS, ColorMode::RGB_COLOR_TEMPERATURE});
        return traits;
    }

    void setup() override
    {
        // Initialization
        this->on_ = false;
        this->brightness_ = 0;
        this->colorTemp_ = 0;
        this->hue_ = 0;
        this->sat_ = 0;
    }

    void write_state(LightState *state) override
    {
        float brightnessBuf;
        float red, green, blue, hue, sat;
        state->set_default_transition_length(0);
        state->current_values_as_brightness(&brightnessBuf);
        auto brightness = f2u(brightnessBuf);
        auto colorTemp = f2u(state->remote_values.get_color_temperature());
        state->current_values_as_rgb(&red, &green, &blue);
        RGBtoHSV(red, green, blue, hue, sat);
        state->make_call().set_red(1.0);
        state->make_call().set_green(1.0);
        state->make_call().set_blue(1.0);
        state->make_call().set_brightness(1.0);
        state->turn_on();
        if (brightness == 0)
        {
            this->on_ = false;
            state->make_call().set_brightness(0.0);
            state->turn_off();
            send_ir("OFF");
        }
        else
        {
            this->on_ = true;
            send_ir("ON");
            if (brightness != this->brightness_)
            {
                this->brightness_ = brightness;
                state->make_call().set_brightness(brightnessBuf);
                set_brightness(brightness);
            }
            else if (colorTemp != this->colorTemp_)
            {
                this->colorTemp_ = colorTemp;
                state->make_call().set_color_temperature(colorTemp);
                set_color_temp(colorTemp);
            }
            else if (hue != this->hue_ || sat != this->sat_)
            {
                this->hue_ = hue;
                this->sat_ = sat;
                set_hs_color(hue, sat, state);
            }
        }
        ESP_LOGI("citilux", "brightness: %d, colorTemp: %d, hue: %f, sat: %f", brightness, colorTemp, hue, sat);
        state->publish_state();
    }

    void set_brightness(int brightness)
    {
        if (brightness == 255)
        {
            send_ir("max");
        }
        else if (brightness == 0)
        {
            send_ir("OFF");
        }
        else if (brightness > 128)
        {
            send_ir("M3");
            send_ir("bri-", round((255 - brightness) / 9.444444));
        }
        else if (brightness < 20)
        {
            send_ir("M1");
        }
        else
        {
            send_ir("M2");
            send_ir("bri-", round((128 - brightness) / 9.444444));
        }
    }

    void set_color_temp(int color_temp)
    {
        send_ir("max");
        send_ir("cold", round((color_temp - 155) / 10.1470588235));
    }

    void set_hs_color(unsigned char hue, unsigned char sat, LightState *state)
    {
        send_ir("ON");
        send_ir("RGB", convert_to_impulses(hue, sat, state));
    }

    unsigned char f2u(float f)
    {
        return (unsigned char)(f * 255.0f);
    }

    void RGBtoHSV(float &fR, float &fG, float &fB, float &fH, float &fS)
    {
        float fCMax = max(max(fR, fG), fB);
        float fCMin = min(min(fR, fG), fB);
        float fDelta = fCMax - fCMin;

        if (fDelta > 0)
        {
            if (fCMax == fR)
            {
                fH = 60 * (fmod(((fG - fB) / fDelta), 6));
            }
            else if (fCMax == fG)
            {
                fH = 60 * (((fB - fR) / fDelta) + 2);
            }
            else if (fCMax == fB)
            {
                fH = 60 * (((fR - fG) / fDelta) + 4);
            }

            if (fCMax > 0)
            {
                fS = fDelta / fCMax;
            }
            else
            {
                fS = 0;
            }
        }
        else
        {
            fH = 0;
            fS = 0;
        }

        if (fH < 0)
        {
            fH = 360 + fH;
        }
    }

    int convert_to_impulses(unsigned char hue, unsigned char sat, LightState *state)
    {
        state->turn_on();
        state->make_call().set_brightness(0.5);
        if (hue == 0 && sat == 0)
        {
            state->make_call().set_red(1.0);
            state->make_call().set_green(1.0);
            state->make_call().set_blue(1.0);
            return -1; // белый
        }
        else if (hue < 10 || hue > 355)
        {
            state->make_call().set_red(1.0);
            state->make_call().set_green(0.0);
            state->make_call().set_blue(0.0);
            return 3; // красный
        }
        else if (hue < 71)
        {
            state->make_call().set_red(1.0);
            state->make_call().set_green(0.5);
            state->make_call().set_blue(0.0);
            return 4; // оранжевый
        }
        else if (hue < 140)
        {
            state->make_call().set_red(0.0);
            state->make_call().set_green(1.0);
            state->make_call().set_blue(0.0);
            return 6; // зелёный
        }
        else if (hue < 160)
        {
            state->make_call().set_red(1.0);
            state->make_call().set_green(1.0);
            state->make_call().set_blue(0.0);
            return 5; // салатовый
        }
        else if (hue < 212)
        {
            state->make_call().set_red(0.0);
            state->make_call().set_green(1.0);
            state->make_call().set_blue(1.0);
            return 7; // голубой
        }
        else if (hue < 263)
        {
            state->make_call().set_red(0.0);
            state->make_call().set_green(0.0);
            state->make_call().set_blue(1.0);
            return 1; // синий
        }
        else if (hue < 325)
        {
            state->make_call().set_red(1.0);
            state->make_call().set_green(0.0);
            state->make_call().set_blue(1.0);
            return 2; // фиолетовый
        }
        state->make_call().set_red(1.0);
        state->make_call().set_green(0.0);
        state->make_call().set_blue(0.0);
        return 3;
    }

private:
    bool on_;
    unsigned char colorTemp_, brightness_;
    float hue_, sat_;
    void send_ir(std::string code, int repeat_times = 1)
    {
        if (repeat_times == -1)
        {
            send_ir("max");
            return;
        }
        auto ir_code = IR_CODES.at(code);
        auto transmit = id(transmitter).transmit();
        esphome::remote_base::ProntoData data{};
        data.data = ir_code;
        esphome::remote_base::ProntoProtocol().encode(transmit.get_data(), data);

        for (int i = 0; i < repeat_times; i++)
        {
            transmit.perform();
            delay_microseconds_safe(2000);
        }
    }
};
