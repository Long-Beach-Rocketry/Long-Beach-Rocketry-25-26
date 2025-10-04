#pragma once
#include <span>
#include <string>

class Usart
{
public:
    virtual void send_tx(const std::span<char> data) = 0;
    virtual bool receive_rx(std::span<char>& data) = 0;
};