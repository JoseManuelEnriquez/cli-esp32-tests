#ifndef UART_HPP
#define UART_HPP
#include "driver/uart.h"
#include <string>
#include <cstring> // memcpy

#define UART_PORT_0 UART_NUM_0
#define UART_PORT_1 UART_NUM_1
#define UART_PORT_2 UART_NUM_2

#define DEFAULT_BUFFER_SIZE (1024)

class UART{
    private: 
        uart_port_t uart_port;
    public:
        UART(uart_port_t port,
            int baud_rate = 115200,
            uint32_t rx_pin = UART_PIN_NO_CHANGE,
            uint32_t tx_pin = UART_PIN_NO_CHANGE
            );
        
        uint32_t receive(uint8_t* buffer, uint32_t length);
        void send(const char* data, int len);
        ~UART();
};

#endif