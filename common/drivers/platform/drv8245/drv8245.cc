#include "drv8245.h"

namespace LBR {

Drv8245::Drv8245(Gpio& dir, Pwm& pwm, Gpio& drvoff, Gpio& sleep, Gpio& fault)
    : dir_(dir), pwm_(pwm), drvoff_(drvoff), sleep_(sleep), fault_(fault)
{
}

bool Drv8245::init() {




}

} // namespace LBR