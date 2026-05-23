#include "include/invoker.hpp"

Invoker::Invoker(): count(0){}

bool Invoker::register_command(std::string_view name, std::unique_ptr<Command> command){
    if(count == MAX_COMMANDS){
        return false;
    }

    commands[count].name = name;
    commands[count].command = std::move(command);

    count++;
    return true;
}

void Invoker::process(std::string_view name_cmd, std::string_view parameters){
    for (size_t i = 0; i < count; ++i) {
        if (commands[i].name == name_cmd) {
            commands[i].command->execute(parameters);
            return;
        }
    }
}