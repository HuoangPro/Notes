#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

#include <functional>
#include <chrono>

using HandlerType = std::function<void(const char* name, double el)>;

class ElapsedTime
{
public:
    ElapsedTime(const char* name);
    ~ElapsedTime();

    static void installMessageHandler(HandlerType handler);
private:
    void stop();
    const char* m_name;
    std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
    bool m_stopped;
    static HandlerType m_handler;
};

#endif // ELAPSEDTIME_H
