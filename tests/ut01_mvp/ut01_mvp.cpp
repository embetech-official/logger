#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>

extern "C" void log_from_c(void);

TEST(LOGGER, UT01_MinimalViableProduct) {
    std::stringstream received;
    auto              clear_output = [&received]() {
        received.str(std::string());
        received.clear();
    };

    EXPECT_FALSE(::LOGGER_IsEnabled());

    EXPECT_FALSE(::LOGGER_Enable());
    EXPECT_FALSE(::LOGGER_IsEnabled());
    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);


    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());
    ::LOGGER_DisableHeader();

    char const* const msg = "test message";

    {
        char const* const expected = "test message\n";

        LOGGER_DISABLED(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();

        LOGGER_TRACE(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();

        LOGGER_DEBUG(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();

        LOGGER_VERBOSE(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();

        LOGGER_INFO(msg);
        EXPECT_TRUE(received.str().empty());
        clear_output();

        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        LOGGER_ERROR(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        LOGGER_CRITICAL(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        LOGGER_ALERT(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();

        LOGGER_EMERGENCY(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        char const* const expected = "log_from_c\n";
        log_from_c();
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_EnableHeader();

    {
        char const* const expected = "DEFAULT (N): test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }


    ::LOGGER_Disable();
    EXPECT_FALSE(LOGGER_IsEnabled());

    LOGGER_NOTICE(msg);
    EXPECT_TRUE(received.str().empty());
    clear_output();

    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());

    ::LOGGER_SetOutput(nullptr, nullptr);
    EXPECT_FALSE(::LOGGER_IsEnabled());
}