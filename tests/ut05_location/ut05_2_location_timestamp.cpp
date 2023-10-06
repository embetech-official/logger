#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>


TEST(LOGGER, UT05_CodeLocationWithTimestamp) {
    std::stringstream received;
    auto              clear_output = [&received]() {
        received.str(std::string());
        received.clear();
    };

    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());

    char const* const msg = "test message";
    {
#line 1
        char const* const expected = "0 DEFAULT (N) [OVERRIDEN:2]: test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_DisableHeader();

    {
        char const* const expected = "test message\n";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}