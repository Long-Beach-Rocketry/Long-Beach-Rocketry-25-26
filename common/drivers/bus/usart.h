#pragma once

class Usart
{
public:
    virtual void send_tx(uint8_t* data, size_t size) = 0;
    virtual void receive_rx(volatile uint8_t* data, size_t size) = 0;
};