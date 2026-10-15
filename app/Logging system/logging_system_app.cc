//** Saladin   */



#include "logging_system_app.h"

std::string format_log_message(LogLevel level, const char* message) {
    const char* level_str = "UNKNOWN";

    switch (level) {
        case LogLevel::DEBUG:    level_str = "DEBUG"; break;
        case LogLevel::INFO:     level_str = "INFO"; break;
        case LogLevel::WARNING:  level_str = "WARNING"; break;
        case LogLevel::ERROR:    level_str = "ERROR"; break;
        case LogLevel::CRITICAL: level_str = "CRITICAL"; break;
    }

    return std::string(level_str) + ": " + message + "\n";
}