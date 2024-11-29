#include <unistd.h>
#include "profiler.h"

void test() {
    PROFILE_FUNCTION();
    usleep(5000);
}
void test(int) {
    PROFILE_FUNCTION();
    usleep(5000);
}


int main(int argc, char *argv[])
{
    Instrumentor::Instance().beginSession("test");
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();
    test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();test();

    Instrumentor::Instance().endSession();


    Instrumentor::Instance().beginSession("test2");
    test();
    // Instrumentor::Instance().endSession();


    usleep(5000);
    return 0;
}
