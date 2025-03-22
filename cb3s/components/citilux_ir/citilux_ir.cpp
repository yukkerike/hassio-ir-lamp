#include "citilux_ir.h"

namespace esphome
{
    namespace citilux_ir
    {
        static const char *TAG = "citilux_ir";

        light::LightTraits CitiluxIROutput::get_traits()
        {
            auto traits = light::LightTraits();
            // traits.set_supported_color_modes({light::ColorMode::ON_OFF, light::ColorMode::BRIGHTNESS, light::ColorMode::RGB, light::ColorMode::COLOR_TEMPERATURE});
            traits.set_supported_color_modes({light::ColorMode::RGB});
            return traits;
        }

        void CitiluxIROutput::setup()
        {
            this->on_ = false;
            this->brightness_ = 0;
            this->colorTemp_ = 0;
            this->hue_ = 0;
            this->sat_ = 0;
        }

        void CitiluxIROutput::write_state(light::LightState *state)
        {
            state->set_default_transition_length(0);

            float brightnessBuf;
            state->current_values_as_brightness(&brightnessBuf);
            auto brightness = this->f2u(brightnessBuf);

            auto colorTemp = this->f2u(state->remote_values.get_color_temperature());

            float red, green, blue, hue, sat;
            state->current_values_as_rgb(&red, &green, &blue);
            this->RGBtoHSV(red, green, blue, hue, sat);

            ESP_LOGI(TAG, "Pre-state update: on_=%d, brightness_=%d, requested brightness=%d, hue=%f, sat=%f",
                     this->on_, this->brightness_, brightness, hue, sat);
            if (brightness == 0 &&
                    this->on_ == false ||
                (this->brightness_ == brightness &&
                 this->colorTemp_ == colorTemp &&
                 this->hue_ == hue &&
                 this->sat_ == sat))
                return;

            auto call = state->make_call();

            if (brightness == 0)
            {
                this->on_ = false;
                this->brightness_ = 0;
                call.set_state(false);
                call.set_brightness(0);
                send_ir("OFF");
            }
            else if (this->on_ == false && brightness > 0)
            {
                this->on_ = true;
                this->brightness_ = brightness;
                call.set_state(true);
                call.set_brightness(brightnessBuf);
                send_ir("ON");
            }
            else
            {
                this->on_ = true;
                call.set_state(true);

                if (colorTemp != this->colorTemp_)
                {
                    this->colorTemp_ = colorTemp;
                    this->set_color_temp(colorTemp);
                    call.set_color_temperature(colorTemp);
                }
                else if (hue != this->hue_ || sat != this->sat_)
                {
                    this->hue_ = hue;
                    this->sat_ = sat;
                    send_ir("ON");
                    this->set_hs_color(hue, sat);
                    call.set_rgb(red, green, blue);
                    ESP_LOGI(TAG, "RGB mode activated");
                }
                else if (brightness != this->brightness_)
                {
                    this->brightness_ = brightness;
                    this->set_brightness(brightness);
                    call.set_brightness(brightnessBuf);
                }
            }

            call.set_publish(false);
            call.set_save(false);
            call.perform();
            ESP_LOGI(TAG, "Final state: on=%d, brightness=%d (internal brightness_=%d), colorTemp=%d, hue=%f, sat=%f",
                     this->on_, brightness, this->brightness_, colorTemp, hue, sat);

            // state->publish_state();
        }

        void CitiluxIROutput::dump_config()
        {
            ESP_LOGCONFIG(TAG, "CitiluxIR Light:");
            ESP_LOGCONFIG(TAG, "  Supported modes: ON/OFF, Brightness, RGB, Color Temperature");
            if (this->transmitter_ == nullptr)
            {
                ESP_LOGCONFIG(TAG, "  WARNING: No transmitter set!");
            }
        }

        void CitiluxIROutput::set_brightness(int brightness)
        {
            if (brightness == 255)
            {
                send_ir("M3");
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

        void CitiluxIROutput::set_color_temp(int color_temp)
        {
            send_ir("M3");
            send_ir("cold", round((color_temp - 155) / 10.1470588235));
        }

        void CitiluxIROutput::set_hs_color(unsigned char hue, unsigned char sat)
        {
            send_ir("RGB", convert_to_impulses(hue, sat));
        }

        void CitiluxIROutput::RGBtoHSV(float &fR, float &fG, float &fB, float &fH, float &fS)
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

        int CitiluxIROutput::convert_to_impulses(unsigned char hue, unsigned char sat)
        {
            if (hue == 0 && sat == 0)
            {
                return -1; // белый
            }
            else if (hue < 10 || hue > 355)
            {
                return 3; // красный
            }
            else if (hue < 71)
            {
                return 4; // оранжевый
            }
            else if (hue < 140)
            {
                return 6; // зелёный
            }
            else if (hue < 160)
            {
                return 5; // салатовый
            }
            else if (hue < 212)
            {
                return 7; // голубой
            }
            else if (hue < 263)
            {
                return 1; // синий
            }
            else if (hue < 325)
            {
                return 2; // фиолетовый
            }
            return 3;
        }

        void CitiluxIROutput::send_ir(std::string code, int repeat_times)
        {
            if (repeat_times == -1)
            {
                send_ir("M3");
                return;
            }

            if (!this->transmitter_)
            {
                ESP_LOGE(TAG, "Transmitter not set");
                return;
            }

            auto ir_code_it = IR_CODES.find(code);
            if (ir_code_it == IR_CODES.end())
            {
                ESP_LOGE(TAG, "Unknown IR code: %s", code.c_str());
                return;
            }

            auto transmit = this->transmitter_->transmit();
            remote_base::ProntoData data{};
            data.data = ir_code_it->second;
            remote_base::ProntoProtocol().encode(transmit.get_data(), data);
            transmit.perform();

            if (repeat_times > 1)
            {
                if (code != "RGB")
                {
                    data.data = STANDARD_REPEAT_SEQUENCE;
                    transmit = this->transmitter_->transmit();
                    remote_base::ProntoProtocol().encode(transmit.get_data(), data);

                    for (int i = 1; i < repeat_times; i++)
                    {
                        delay_microseconds_safe(50000);
                        transmit.perform();
                    }
                }
                else
                {
                    for (int i = 1; i < repeat_times; i++)
                    {
                        delay_microseconds_safe(150000);
                        transmit.perform();
                    }
                }
            }
        }
    }
}