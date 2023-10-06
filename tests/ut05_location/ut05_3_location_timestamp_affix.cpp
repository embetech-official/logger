#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>


TEST(LOGGER, UT05_CodeLocationWithPrefixAndTimestamp) {
    std::stringstream received;
    auto              clear_output = [&received]() {
        received.str(std::string());
        received.clear();
    };

    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());
    ::LOGGER_SetPrefix("prefix", 6U);
    ::LOGGER_SetSuffix("suffix", 6U);

    char const* const msg = "test message";
    {
#line 1
        char const* const expected = "prefix0 DEFAULT (N) [OVERRIDEN:2]: test message\nsuffix";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    ::LOGGER_DisableHeader();

    {
        char const* const expected = "prefixtest message\nsuffix";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}