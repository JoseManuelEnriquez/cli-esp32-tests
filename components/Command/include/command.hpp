#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <string_view>
#include "uart.hpp"

class Command{
    public:
        Command(UART* uart_ptr);
        virtual void execute(std::string_view parameters) = 0;
    protected:
        UART* uart;
};

#endif