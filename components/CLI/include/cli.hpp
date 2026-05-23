#ifndef CLI_HPP
#define CLI_HPP
#include "invoker.hpp"
#include "command.hpp"
#include "uart.hpp"

class CLI{
    public:
        CLI();
        void receive_command();
    private:
        UART uart;
        Invoker invoker;
};

#endif