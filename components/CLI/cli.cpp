#include "include/cli.hpp"

CLI::CLI(): uart(UART(UART_PORT_0)), invoker(Invoker()){
    // Aqui tengo que llamar a invoker.register_command() para que se pueda utilizar
}

void CLI::receive_command(){
    uint32_t size_buf = DEFAULT_BUFFER_SIZE;
    uint8_t buf[size_buf];
    
    uint32_t len = uart.receive(buf, size_buf);
    if(len > 0){
        std::string_view cmd(reinterpret_cast<const char*>(buf), len);
        // tengo que hacer el parser
        std::string_view name;
        std::string_view parameters;
        invoker.process(name, parameters);
    }
}