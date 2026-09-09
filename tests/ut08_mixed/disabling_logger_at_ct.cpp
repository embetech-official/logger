
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage) has to be a macro: logger.h reads it at preprocessing time
#define LOGGER_ENABLED 0 // Override value from logger_config.h
#define LOGGER_CHANNEL UT_06
#include <embetech/logger.h>
#include <gtest/gtest.h>

#include <sstream>

TEST(LOGGER, UI07_DisablingLoggerAtCT) {
  std::stringstream received;
  auto clear_output = [&received]() {
    received.str(std::string());
    received.clear();
  };

  ::LOGGER_SetOutput([](char c, void *context) { *static_cast<decltype(received) *>(context) << c; }, &received);
  EXPECT_TRUE(::LOGGER_Enable());
  EXPECT_TRUE(::LOGGER_IsEnabled());

  [[maybe_unused]] constexpr static auto message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis auctor elementum leo, at pretium "
                                                   "orci dignissim vehicula. Suspendisse quis scelerisque dolor, eu vestibulum augue.";

  LOGGER_NOTICE(message);
  EXPECT_EQ(received.str().size(), 0);
  clear_output();

  ::LOGGER_Disable();
  EXPECT_EQ(::LOGGER_IsEnabled(), false);
  LOGGER_NOTICE(message);
  EXPECT_EQ(received.str().size(), 0);
  clear_output();
}
