#include "ElapsedTime.h"
#include <iostream>

void printElapsedTime(const char* name, double el) {
    std::cout << "[custom] " << name << ": " << el << "ms" << std::endl;
}

void f() {
    ElapsedTime el("f");
    for(int i = 0; i < 1000000; i++) {
//        std::cout << "i";
    }
    std::cout << std::endl;
}
int main(int argc, char *argv[])
{
    ElapsedTime::installMessageHandler(printElapsedTime);
    f();
    return 0;
}
