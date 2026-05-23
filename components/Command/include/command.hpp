#ifndef COMMAND_HPP
#define COMMAND_HPP
#include <string_view>

using Callback = void(*)();

class Command{
    public:
        Command(Callback _callback);
        virtual void execute(std::string_view parameters) = 0;
    private:
        Callback callback;
};

#endif