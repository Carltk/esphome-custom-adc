#include "esphome.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

class CustomADCSensor : public PollingComponent, public Sensor {
 public:
  // Constructor now takes no params; setters are used from YAML
  CustomADCSensor() : PollingComponent(100) {}  // Default update every 100ms

  void set_pin(gpio_num_t pin) { pin_ = pin; }
  void set_atten(adc_atten_t atten) { atten_ = atten; }

  void setup() override {
    // Initialize ADC oneshot unit for ADC1 (C6 only has this)
    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.unit_id = ADC_UNIT_1;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
    adc_oneshot_new_unit(&init_config, &handle_);

    // Configure channel
    adc_oneshot_chan_cfg_t chan_config = {};
    chan_config.atten = atten_;
    chan_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_oneshot_config_channel(handle_, (adc_channel_t) pin_, &chan_config);

    // Calibration (line fitting scheme for C6)
    adc_cali_line_fitting_config_t cali_config = {};
    cali_config.unit_id = ADC_UNIT_1;
    cali_config.atten = atten_;
    cali_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle_);
  }

  void update() override {
    int raw = 0;
    adc_oneshot_read(handle_, (adc_channel_t) pin_, &raw);

    int mv = 0;
    adc_cali_raw_to_voltage(cali_handle_, raw, &mv);
    publish_state(mv / 1000.0f);  // Publish as volts (e.g., 1.65V)
  }

 private:
  gpio_num_t pin_;
  adc_atten_t atten_;
  adc_oneshot_unit_handle_t handle_;
  adc_cali_handle_t cali_handle_;
};
