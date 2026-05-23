#include <stdio.h>
#include "cli.hpp"

extern "C" void app_main(void)
{    
    CLI cli;
    while(1){
        cli.receive_command();
    }
}
