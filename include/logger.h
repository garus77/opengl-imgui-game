#pragma once // logger.h
#include <iostream>
#include <mutex>

class Logger
{
  public:
    // Public accessor
    static Logger &instance()
    {
        static Logger instance; // created on first call, thread-safe in C++11+
        return instance;
    }

    // Example API
    void log(const std::string &msg)
    {
        std::lock_guard<std::mutex> lock(m_mutex); // serialize output
        std::cout << "[LOG] " << msg << "\n";
    }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger &operator=(Logger &&) = delete;

  private:
    Logger() { std::cout << "Logger initialized\n"; }
    ~Logger() { std::cout << "Logger shutting down\n"; }

    std::mutex m_mutex;
};