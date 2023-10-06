#define LOGGER_CHANNEL UT_04
#include <gtest/gtest.h>
#include <embetech/logger.h>

#include <sstream>

TEST(LOGGER, UT08_DisablingChannelAtCT) {
    EXPECT_TRUE(LOGGER_CompileTimeDescriptor.loggerEnabled);
    std::stringstream received;
    auto              clear_output = [&received]() {
        received.str(std::string());
        received.clear();
    };

    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());

    [[maybe_unused]] constexpr static auto
        message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis auctor elementum leo, at pretium orci dignissim vehicula. Suspendisse quis scelerisque dolor, eu vestibulum augue.";

    LOGGER_DISABLED(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_EMERGENCY(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_ALERT(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_CRITICAL(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_ERROR(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_WARNING(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_NOTICE(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_INFO(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_VERBOSE(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_DEBUG(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();

    LOGGER_TRACE(message);
    EXPECT_EQ(received.str().size(), 0);
    clear_output();
}