#include "logging_system_app.h"
#include <cassert>

int main() {
    assert(format_log_message(LogLevel::INFO, "Test message")
           == "INFO: Test message\n");
}