#ifndef NICK_UART_H
#define NICK_UART_H

#include "stm32l476xx.h"
#include "usart.h"
#include <cstdint> //for uint8_t, uint32_t, etc

void uart_setup(void); //set uart clock and baud rate
void uart_write(const uint8_t* data, size_t len); //generic write function; handles writing multiple bytes
void uart_write_byte(uint8_t b); //write a single byte
uint32_t uart_read(uint8_t* buf, size_t max_len); //read up to max_len bytes into buf, return number of bytes read
uint8_t uart_read_byte(void); //read a single byte; blocks until a byte is available
bool uart_byte_available(void); //returns true if a byte is available to read, false otherwise


#endif // NICK_UART_H