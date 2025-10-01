#pragma once

class Usart
{
public:
    virtual void send_tx(const uint8_t* data, size_t size) = 0;
    virtual bool receive_rx(volatile uint8_t* data, size_t size) = 0;
};