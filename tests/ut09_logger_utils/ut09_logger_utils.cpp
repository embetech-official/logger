
#include <gtest/gtest.h>

#include <sstream>
#define LOGGER_CHANNEL UT09

#include <embetech/logger.h>


TEST(LOGGER, LOGGER_UTILS) {
    LOGGER_DEBUG("debug message");
    LOGGER_VERBOSE("warn message");
    LOGGER_INFO("info message");
    LOGGER_NOTICE("notice message");
    LOGGER_WARNING("warn message");
    LOGGER_ERROR("error message");
    LOGGER_CRITICAL("critical message");
    LOGGER_ALERT("alert message");
    LOGGER_EMERGENCY("emergency message");
}