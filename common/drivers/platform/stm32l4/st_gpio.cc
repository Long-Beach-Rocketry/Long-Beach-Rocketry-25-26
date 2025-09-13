#include "st_gpio.h"
namespace LBR
{
    namespace Stml4
    {
        HwGpio::HwGpio(uint8_t pin_num, GPIO_TypeDef base_addr)
            : pin_num{pin_num}, base_addr{base_addr}
        {
        }

        bool HwGpio::config()
        {
        }
    }
}