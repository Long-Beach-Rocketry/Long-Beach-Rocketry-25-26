#include "board.h"
#include "delay.h"

using namespace LBR;

uint8_t rxb;

namespace
{
size_t append_text(char* dst, size_t cap, size_t idx, const char* text)
{
    while ((*text != '\0') && (idx < cap))
    {
        dst[idx++] = *text++;
    }
    return idx;
}

size_t append_u32(char* dst, size_t cap, size_t idx, uint32_t value)
{
    char tmp[10];
    size_t n = 0;

    do
    {
        tmp[n++] = static_cast<char>('0' + (value % 10U));
        value /= 10U;
    } while ((value != 0U) && (n < sizeof(tmp)));

    while ((n > 0) && (idx < cap))
    {
        dst[idx++] = tmp[--n];
    }

    return idx;
}

size_t build_clock_line(char* dst, size_t cap, const ClockFrequencies& freqs)
{
    size_t idx = 0;
    idx = append_text(dst, cap, idx, "SYSCLK: ");
    idx = append_u32(dst, cap, idx, freqs.sysclk);
    idx = append_text(dst, cap, idx, " Hz, HCLK: ");
    idx = append_u32(dst, cap, idx, freqs.ahb);
    idx = append_text(dst, cap, idx, " Hz, PCLK1: ");
    idx = append_u32(dst, cap, idx, freqs.apb1);
    idx = append_text(dst, cap, idx, " Hz, PCLK2: ");
    idx = append_u32(dst, cap, idx, freqs.apb2);
    idx = append_text(dst, cap, idx, " Hz, PCLK3: ");
    idx = append_u32(dst, cap, idx, freqs.apb3);
    idx = append_text(dst, cap, idx, " Hz, PCLK4: ");
    idx = append_u32(dst, cap, idx, freqs.apb4);
    idx = append_text(dst, cap, idx, " Hz\r\n");
    return idx;
}
}  // namespace

int main(int argc, char** argv)
{
    bsp_init();

    Board& board = get_board();

    while (1)
    {
        const ClockFrequencies& freqs = board.clock.get_clock_frequencies();

        char buf[160];
        size_t tx_len = build_clock_line(buf, sizeof(buf), freqs);
        std::span<const uint8_t> tx_span(reinterpret_cast<const uint8_t*>(buf),
                                         tx_len);
        board.usart.send(tx_span);

        // Busy wait
        Utils::DelayMs(1000);
    }

    return 0;
}