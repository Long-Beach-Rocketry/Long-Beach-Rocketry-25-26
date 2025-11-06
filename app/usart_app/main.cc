#include "usart_app_bsp.h"

char rxb[16] = "Message";
std::span<char> tx_span(rxb, sizeof(rxb));

char data[64];
std::span<char> rx_span(data, sizeof(data));

Usart& usart = BSP_Init(USART2);

bool rx_finished;

int main(int argc, char** argv)
{

    usart.send_tx(tx_span);

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    while (1);

    return 0;
}

extern "C" void USART2_IRQHandler(void)
{

    if (usart.receive_rx(rx_span))
    {
        usart.send_tx(rx_span);
    }
}
