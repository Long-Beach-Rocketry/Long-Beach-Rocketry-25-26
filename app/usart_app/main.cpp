#include "usart_app_bsp.h"

int main(int argc, char** argv)
{
    std::string str_arr = "Received Message Successfully";
    std::string recieve_data = "";
    std::span<char> data(recieve_data.data(), recieve_data.size());

    if (BSP_Init(USART2).receive_rx(data))
    {
        BSP_Init(USART2).send_tx(str_arr);
    }

    while (1);

    return 0;
}