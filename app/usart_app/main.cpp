#include "usart_app_bsp.h"

int main(int argc, char** argv)
{

    uint8_t data[] = {"testin8\n\r"};
    Usart& usart = BSP_Init(USART2);

    usart.send_tx(data, sizeof(data));

    return 0;
}