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
        char const* const expected = "0 DEFAULT (N): test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        ::LOGGER_SetTimeSource([]() { return std::uint32_t(42); });

        char const* const expected = "42 DEFAULT (N): test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }


    {
        ::LOGGER_SetTimeSource([]() {
            static unsigned i;
            return std::uint32_t(++i);
        });

        char const* const expected1 = "1 DEFAULT (N): test message\n";
        char const* const expected2 = "2 DEFAULT (N): test message\n";

        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected1, received.str());
        clear_output();

        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected2, received.str());
        clear_output();
    }

    ::LOGGER_DisableHeader();
    {
        char const* const expected = "test message\n";
        LOGGER_NOTICE(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}