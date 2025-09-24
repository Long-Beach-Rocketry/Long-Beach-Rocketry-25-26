#include "st_usart.h"

void StUsart::receive_rx(volatile uint8_t* data, size_t size)
{
    size_t count = 0;
    while (count < size)
    {
        instance->RDR = data[count++];
    }
}

void StUsart::send_tx(uint8_t* data, size_t size)
{
    size_t count = 0;
    while (1)
    {
        instance->TDR = data[count++];
    }
}

void StUsart::fake_func()
{
}