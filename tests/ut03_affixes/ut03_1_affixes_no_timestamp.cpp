#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>


TEST(LOGGER, UT02_RawTimestamp) {
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
        ::LOGGER_SetPrefix("prefix", 3U);
        char const* const expected = "preDEFAULT (N): test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        ::LOGGER_SetSuffix("suffix", 6U);
        char const* const expected = "preDEFAULT (N): test message\nsuffix";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_DisableHeader();


    {
        ::LOGGER_SetSuffix(nullptr, 0);
        ::LOGGER_SetPrefix("prefix", 6U);
        char const* const expected = "prefixtest message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        ::LOGGER_SetSuffix("suffix", 6U);
        char const* const expected = "prefixtest message\nsuffix";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}