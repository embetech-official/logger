#include <embetech/logger.h>
#include <gtest/gtest.h>

#include <list>
#include <mutex>
#include <sstream>
#include <thread>


TEST(LOGGER, UT06_ThreadSafety) {
    static std::timed_mutex mutex;


    std::stringstream received;

    auto clear_output = [&received]() {
        received.str(std::string());
        received.clear();
    };

    auto lock_function   = [](void* context) { return reinterpret_cast<decltype(mutex)*>(context)->try_lock_for(std::chrono::seconds(5)); };
    auto unlock_function = [](void* context) { return reinterpret_cast<decltype(mutex)*>(context)->unlock(); };

    constexpr static auto msg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
                                "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
                                "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

    constexpr static auto
        expected_line = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis "
                        "nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu "
                        "fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n";


    auto const thread_count    = 20;
    auto const msg_repetitions = 10;

    std::stringstream expected;
    for (auto i = 0; i != thread_count * msg_repetitions; ++i) {
        expected << expected_line;
    }


    ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
    ::LOGGER_DisableHeader();

    EXPECT_FALSE(::LOGGER_SetLockingMechanism(lock_function, nullptr, nullptr));
    EXPECT_FALSE(::LOGGER_SetLockingMechanism(nullptr, unlock_function, nullptr));
    EXPECT_TRUE(::LOGGER_SetLockingMechanism(lock_function, unlock_function, &mutex));

    EXPECT_TRUE(::LOGGER_Enable());
    EXPECT_TRUE(::LOGGER_IsEnabled());


    auto thread_body = [](int reps) {
        for (auto i = 0; i != reps; ++i) {
            LOGGER_NOTICE(msg);
        }
    };
    {
        std::list<std::jthread> threads;

        for (auto i = 0; i != thread_count; ++i) {
            threads.emplace_back(thread_body, msg_repetitions);
        }
        for (auto& t : threads) {
            t.join();
        }
    }


    EXPECT_EQ(received.str(), expected.str());
    clear_output();

    EXPECT_TRUE(::LOGGER_SetLockingMechanism(nullptr, nullptr, nullptr));
    auto is_mutex_free = mutex.try_lock();
    EXPECT_TRUE(is_mutex_free);
    if (is_mutex_free) {
        mutex.unlock();
    }
}