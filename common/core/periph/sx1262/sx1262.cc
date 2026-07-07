#include "sx1262.h"

namespace LBR
{
Sx1262::Sx1262(Sx1262Params& params_)
    : settings{params_.settings}, spi{params_.spi}
{
}
}  // namespace LBR
