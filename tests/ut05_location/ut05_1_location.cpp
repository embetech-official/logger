#include <embetech/logger.h>
#include <gtest/gtest.h>

#include <sstream>


extern "C" void log_test_message(void); // defined in ut05_1_location.c


TEST(LOGGER, UT05_CodeLocation) {
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
        char const* const expected = "DEFAULT (N) [OVERRIDEN:1]: test message\n";

#line 1
        LOGGER_NOTICE(msg);

        EXPECT_EQ(expected, received.str());
        clear_output();

        log_test_message();
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_DisableHeader();

    {
        char const* const expected = "test message\n";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        log_test_message();
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}