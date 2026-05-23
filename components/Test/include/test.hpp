#ifndef TEST_HPP
#define TEST_HPP
#include "Command.h"
#include "uart.hpp"
#include <string_view>

class TestCliCommand: public Command{
    public:
        TestCommand(UART* uart_ptr) : Command(uart_ptr){};
        void execute(std::string_view parameters) override;
};

#endif