#ifndef INVOKER_HPP
#define INVOKER_HPP
#include "command.hpp"
#include <memory>
#include <array>
#include <string_view>

#define MAX_COMMANDS 10

class Invoker {
    public:
        Invoker();
        bool register_command(std::string_view name, std::unique_ptr<Command> command);
        void process(std::string_view name_cmd, std::string_view parameters);
    private:
        typedef struct{
            std::string_view name; // Es solo una vista de solo lectura de una cadena en memoria
            std::unique_ptr<Command> command;
        }CommandEntry;
        std::array<CommandEntry, MAX_COMMANDS>commands;
        uint32_t count;

};

#endif