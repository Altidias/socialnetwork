// Logger.hpp
#pragma once

#include "imguial_term.h"
#include <mutex>
#include <memory>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Delete copy and move constructors and assign operators
    Logger(Logger const&) = delete;             // Copy construct
    Logger(Logger&&) = delete;                  // Move construct
    Logger& operator=(Logger const&) = delete;  // Copy assign
    Logger& operator=(Logger &&) = delete;      // Move assign

    // Thread-safe methods to log messages
    void debug(const char* format, ...);
    void info(const char* format, ...);
    void warning(const char* format, ...);
    void error(const char* format, ...);

    // Method to draw the log
    void draw(const ImVec2& size = ImVec2(0.0f, 0.0f));

private:
    Logger(); // Private constructor
    ~Logger() = default;

    ImGuiAl::BufferedLog<4096> log;
    std::mutex mtx;
    std::string getCurrentTimestamp();
};
