// Logger.cpp
#include "Logger.hpp"
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>


Logger::Logger() {}

void Logger::debug(const char* format, ...) {
    std::lock_guard<std::mutex> lock(mtx);
    va_list args;
    va_start(args, format);

    std::string formattedMessage = getCurrentTimestamp() + " [DEBUG] - ";
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    formattedMessage += buffer;

    log.debug("%s", formattedMessage.c_str());
    va_end(args);
}

void Logger::info(const char* format, ...) {
    std::lock_guard<std::mutex> lock(mtx);
    va_list args;
    va_start(args, format);
    
    std::string formattedMessage = getCurrentTimestamp() + " [INFO] - ";
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    formattedMessage += buffer;

    log.info("%s", formattedMessage.c_str());
    va_end(args);
}

void Logger::warning(const char* format, ...) {
    std::lock_guard<std::mutex> lock(mtx);
    va_list args;
    va_start(args, format);
    
    std::string formattedMessage = getCurrentTimestamp() + " [WARNING] - ";
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    formattedMessage += buffer;

    log.info("%s", formattedMessage.c_str());
    va_end(args);
}

void Logger::error(const char* format, ...) {
    std::lock_guard<std::mutex> lock(mtx);
    va_list args;
    va_start(args, format);
    
    std::string formattedMessage = getCurrentTimestamp() + " [ERROR] - ";
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    formattedMessage += buffer;

    log.info("%s", formattedMessage.c_str());
    va_end(args);
}

void Logger::draw(const ImVec2& size) {
    log.draw(size);
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    tm buf;
    #ifdef _WIN32
        localtime_s(&buf, &in_time_t);
    #else
        localtime_r(&in_time_t, &buf); // POSIX
    #endif
        ss << std::put_time(&buf, "%Y-%m-%d %X"); // Example format: 2024-04-11 15:46:00
        return ss.str();
}