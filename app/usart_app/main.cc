#include "usart_app_bsp.h"

std::string rxb = "Message";

char data[16];
std::span<char> rx_span(data, sizeof(data));

Usart& usart = BSP_Init(USART2);

bool rx_finished = false;

int main(int argc, char** argv)
{

    usart.send_tx(rxb);

    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    while (1);

    return 0;
}

extern "C" void USART2_IRQHandler(void)
{

    usart.receive_rx(rx_span);

    usart.send_tx(rx_span);
}
