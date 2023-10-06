#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>

TEST(LOGGER, UT06_ThreadSafety) {
    static std::stringstream received;

    auto clear_output = []() {
        received.str(std::string());
        received.clear();
    };
    auto flush_hook = []() { received << 'x'; };

    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());
    ::LOGGER_DisableHeader();

    char const* const msg = "test message";
    {
        char const* const expected = "test message\n";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        ::LOGGER_SetFlushHook(flush_hook, false);
        char const* const expected1 = "test message\n";
        char const* const expected2 = "test message\nx";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected1, received.str());
        ::LOGGER_Flush();
        EXPECT_EQ(expected2, received.str());
        clear_output();
    }

    {
        ::LOGGER_SetFlushHook(flush_hook, true);
        char const* const expected = "test message\nx";
        LOGGER_WARNING(msg);
        EXPECT_EQ(expected, received.str());
        clear_output();
    }

    {
        char const* const expected = "\n\n\n\n\nx";
        LOGGER_WARNING("\n\n\n\n");
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
    
    {
        char const* const expected = "\nx\nx\nx\nx";
        LOGGER_PrintNL();
        LOGGER_PrintNL();
        LOGGER_PrintNL();
        LOGGER_PrintNL();
        EXPECT_EQ(expected, received.str());
        clear_output();
    }
}