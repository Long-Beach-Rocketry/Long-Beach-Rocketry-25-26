#include "pps_hw.h"


namespace LBR {
    bool lms() {
        // PA3: limit switch input
        return (GPIOA->IDR & (1 << 3)) != 0;
    }
}

