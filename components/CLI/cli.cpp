#include "include/cli.hpp"

CLI::CLI(): uart(UART(UART_PORT_0)), invoker(Invoker()){
    invoker.register_command("test-cli", std::make_unique<TestCliCommand>(&uart));
}

void CLI::receive_command(){
    uint32_t size_buf = DEFAULT_BUFFER_SIZE;
    uint8_t buf[size_buf];
    
    uint32_t len = uart.receive(buf, size_buf);
    if(len > 0){
        std::string_view cmd(reinterpret_cast<const char*>(buf), len);
        if (cmd.empty()) return;

        size_t space_pos = cmd.find(' ');
        std::string_view name;
        std::string_view parameters;
        
        if (space_pos == std::string_view::npos) {
            name = cmd;
            parameters = "";
        }else {
            name = cmd.substr(0, space_pos);
            parameters = cmd.substr(space_pos + 1);
            
            size_t first_not_space = parameters.find_first_not_of(' ');
            if (first_not_space != std::string_view::npos) {
                parameters = parameters.substr(first_not_space);
            }
        }

        invoker.process(name, parameters);
    }
}