#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>


TEST(LOGGER, UT04_RuntimeVerbosity) {
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
        char const* const expected = "DEFAULT (N): test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        char const* const expected = "DEFAULT (W): test message\n";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        char const* const expected = "DEFAULT (E): test message\n";
        LOGGER_ERROR(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_SetRuntimeLevel(LOGGER_LEVEL_ERROR);

    {
        LOGGER_NOTICE(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();
    }

    {
        LOGGER_WARNING(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();
    }

    {
        char const* const expected = "DEFAULT (E): test message\n";
        LOGGER_ERROR(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}