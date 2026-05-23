#include "include/test.hpp"

void TestCliCommand::execute(std::string_view parameters){
    if (uart == nullptr) return;

    const char* text_test = "\n>>> CORRIENDO: TEST_CLI <<<\n";
    uart->send(text_test, strlen(text_test));
    
}