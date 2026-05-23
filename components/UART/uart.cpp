#include "include/uart.hpp"

UART::UART(uart_port_t port, int baud_rate, uint32_t rx_pin, uint32_t tx_pin):
           uart_port(port)
{
    ESP_ERROR_CHECK(uart_driver_install(uart_port, 
        DEFAULT_BUFFER_SIZE * 2, 
        0, 
        0, 
        NULL,
        0));
    
    uart_config_t uart_config = {}; 

    uart_config.baud_rate = 115200;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_port, rx_pin, tx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(uart_port, UART_MODE_UART));
}

uint32_t UART::receive(uint8_t* buffer, uint32_t length){
    return uart_read_bytes(uart_port, buffer, length-1, portMAX_DELAY);
}

void UART::send(const char* data, int len){
    uart_write_bytes(uart_port, data, len);
}

UART::~UART(){
    uart_driver_delete(uart_port);
}