#include <stdio.h>
#include "uart.hpp"

extern "C" void app_main(void)
{
    UART uart(UART_PORT_0);
    uint32_t size_buf = DEFAULT_BUFFER_SIZE;
    uint8_t* buffer = (uint8_t *) malloc(size_buf);
    
    while(1){
        int len = uart.receive(buffer, size_buf);
        buffer[len-1] = '\0';
        uart.send((const char*) buffer, len);
    }
}
