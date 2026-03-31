#include "nick_uart.h"

#define BAUD_RATE 115200

void uart_setup(void); //set uart clock and baud rate; parity bits, stop bits, etc.
{
    rcc_periph_clock_enable(RCC_USART2);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
    usart_set_data_bits(USART2, 8); //1 byte at a time
    usart_set_baudrate(USART2, BAUD_RATE);
    usart_set_parity(USART2, 0); //no parity bit
    usart_set_stopbits(USART2, 1); //1 stop bit
    usart_set_mode(USART2, USART_MODE_TX_RX); //enable both transmit and receive
    usart_enable_rx_interrupt(USART2); //enable receive interrupt
    
}

void uart_write(const uint8_t* data, size_t len); 
{

}

void uart_write_byte(uint8_t b);
{

}

uint32_t uart_read(uint8_t* buf, size_t max_len);
{

}

unint8_t uart_read_byte(void); 
{

}

bool uart_byte_available(void); 
{

}
