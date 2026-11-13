#include "baro_math.h"

namespace LBR
{

float cal_altitude(float pressure_pa, float atmosph_press_inHg)
{
    return 44330 *
           (1 - pow(pressure_pa / (atmosph_press_inHg * 3386.389), 1 / 5.255));
}

}  // namespace LBR