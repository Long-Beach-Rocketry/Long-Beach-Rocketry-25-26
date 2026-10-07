#include "board.h"
#include "ft_app.h"

using namespace LBR;

// TODO: TEMP FOR TESTING, SWITCH WHEN DRIVER IS DONE AND ADD LoRa to board
class MockLoRa : public LoRa
{
public:
    bool transmit(std::span<const uint8_t> data) override
    {
        (void)data;
        return true;
    }
};

static MockLoRa mock_lora;

int main()
{
    Board& board = get_board();
    board.init();

    FlightTelemApp flight_telem{board.pipeline, mock_lora, &board.flash};
    flight_telem.init();

    while (true)
    {
        flight_telem.update();
    }

    return 0;
}