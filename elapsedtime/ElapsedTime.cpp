#include "ElapsedTime.h"
#include <iostream>
HandlerType ElapsedTime::m_handler = [](const char* name, double el) {
    std::cout << name << ": " << el << "ms" << std::endl;
};
ElapsedTime::ElapsedTime(const char* name)
    : m_name(name),
      m_stopped(false)
{
    m_startTimepoint = std::chrono::steady_clock::now();

}

void ElapsedTime::stop()
{
    if(!m_stopped) {
        std::chrono::time_point<std::chrono::steady_clock> endTimepoint = std::chrono::steady_clock::now();
        double elapsed_time_ms = std::chrono::duration<double, std::milli>(endTimepoint-m_startTimepoint).count();
        m_handler(m_name, elapsed_time_ms);
        m_stopped = true;
    }
}

ElapsedTime::~ElapsedTime() {
    stop();
}

void ElapsedTime::installMessageHandler(HandlerType handler)
{
    m_handler = handler;
}
