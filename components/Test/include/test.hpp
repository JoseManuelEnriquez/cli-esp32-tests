#ifndef TEST_HPP
#define TEST_HPP

#include "command.hpp"
#include "uart.hpp"
#include <string_view>

class TestCliCommand: public Command{
    public:
        TestCliCommand(UART* uart_ptr): Command(uart_ptr){};
        void execute(std::string_view parameters);
};

#endif