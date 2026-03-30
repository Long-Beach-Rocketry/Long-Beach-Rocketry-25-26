#include "st_pwm.h"

namespace LBR {
namespace Stmh7 
{



HwPwm::HwPwm(const StPwmParams& params_) : 
    base_addr{params_->base_addr},
    channel{params_->channel},
    settings{params_->settings}
    {}

bool HwPwm::init() {
  if (base_addr == nullptr) { return false; }

  if (base_addr == TIM6) || (base_addr == TIM7) { return false; }

  return true;
}

bool HwPwm::set_freq(uint32_t freq) override {

}

bool HwPwm::set_duty_cycle(uint8_t duty_cycle) {

}

}  // namespace Stmh7
}  // namespace LBR