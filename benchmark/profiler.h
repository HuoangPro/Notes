#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <cstring>

#define PROFILING 1
#ifdef PROFILING
    #define PROFILE_SCOPE_OUT(name) InstrumentationTimer timer##__LINE__(name, true)
    #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION_OUT()  PROFILE_SCOPE_OUT(__PRETTY_FUNCTION__, true)
    #define PROFILE_FUNCTION()  PROFILE_SCOPE(__PRETTY_FUNCTION__ )
#else
    #define PROFILE_SCOPE(name)
#endif

struct ProfileResult
{
    const std::string name;
    long long start, end;
    uint32_t threadID;
};

class InstrumentationTimer;
class Instrumentor
{
    friend InstrumentationTimer;

private:
    std::string                 m_sessionName   = "None";
    std::mutex                  m_lock;
    bool                        m_activeSession = false;
    std::vector<ProfileResult>  m_profileResultList;
    std::vector<std::thread>    m_threadList;
    Instrumentor() { }

public:

    static Instrumentor& Instance()
    {
        static Instrumentor instance;
        return instance;
    }

    ~Instrumentor()
    {
        endSession();
        for(auto& t : m_threadList) {
            if(t.joinable()) {
                t.join();
            }
        }
    }

    void beginSession(const std::string& name)
    {
        if (m_activeSession) { endSession(); }

        m_profileResultList.clear();
        m_activeSession = true;
        m_sessionName = name;
    }

    void endSession()
    {
        if (!m_activeSession) { return; }
        m_activeSession = false;
        m_threadList.emplace_back(std::thread(&Instrumentor::writeFile, m_sessionName, m_profileResultList));
    }

private:
    void addProfile(const ProfileResult& result) 
    {
        std::lock_guard<std::mutex> lock(m_lock);
        m_profileResultList.emplace_back(result);
    }

    static std::string getExecutablePath()
    {
        char buffer[256];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (len != -1)
        {
            buffer[len] = '\0';
            return std::string(buffer);
        }
        else
        {
            // Handle error
            return "";
        }
    }

    static char *strdup(const char *s)
    {
        size_t slen = strlen(s);
        char *result = (char*)malloc(slen + 1);
        if (result == NULL)
        {
            return NULL;
        }

        memcpy(result, s, slen + 1);
        return result;
    }

    static std::string getExecutableDirectory()
    {
        std::string executablePath = getExecutablePath();
        if (!executablePath.empty())
        {
            char *pathCopy = strdup(executablePath.c_str());
            std::string directoryPath = dirname(pathCopy);
            free(pathCopy);
            return directoryPath;
        }
        return "";
    }

    static void writeFile(const std::string session, const std::vector<ProfileResult> profileResultList) {
        const std::string& filepath = getExecutableDirectory() + "/" + session + ".json";
        std::ofstream outputStream;
        int profileCount = 0;
        outputStream.open(filepath);
        //header
        outputStream << "{\"otherData\": {},\"traceEvents\":[";
        //content
        for(const auto& result : profileResultList) {

            if (profileCount++ > 0)
            {
                outputStream << ",";
            }

            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            outputStream << "{";
            outputStream << "\"cat\":\"function\",";
            outputStream << "\"dur\":" << (result.end - result.start) << ',';
            outputStream << "\"name\":\"" << name << "\",";
            outputStream << "\"ph\":\"X\",";
            outputStream << "\"pid\":0,";
            outputStream << "\"tid\":" << result.threadID << ",";
            outputStream << "\"ts\":" << result.start;
            outputStream << "}";
        }
        //footer
        outputStream << "]}";
        outputStream.close();

    }

};

class InstrumentationTimer
{
    ProfileResult m_result;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
    bool m_stopped;
    bool m_isPrintOut;

public:

    InstrumentationTimer(const std::string & name, bool isPrintOut = false)
        : m_result({ name, 0, 0, 0 })
        , m_stopped(false)
        , m_isPrintOut(isPrintOut)
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_stopped) { stop(); }
    }

    void stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        m_result.end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        m_result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Instance().addProfile(m_result);

        m_stopped = true;

        if(m_isPrintOut) {
            std::cerr << m_result.start << ":" << m_result.threadID << ":" << m_result.name << ": took " << m_result.end - m_result.start << "(ms)" << std::endl; 
        }
    }
};


#endif // PROFILER_H
