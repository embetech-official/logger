// #define LOGGER_CHANNEL UT_10


// #include <cfloat>
// #include <cinttypes>
// #include <cstdint>
// using namespace std::literals;

// #include <gtest/gtest.h>
// #include <embetech/logger.h>

// #include <sstream>

// TEST(LOGGER, UT08_FormatSupport) {
//     std::stringstream received;
//     auto              clear_output = [&received]() {
//         received.str(std::string());
//         received.clear();
//     };

//     ::LOGGER_SetOutput([](char c, void* context) { *reinterpret_cast<decltype(received)*>(context) << c; }, &received);
//     EXPECT_TRUE(::LOGGER_Enable());
//     EXPECT_TRUE(::LOGGER_IsEnabled());

//     [[maybe_unused]] constexpr static auto
//         message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis auctor elementum leo, at pretium orci dignissim vehicula. Suspendisse quis scelerisque dolor, eu vestibulum augue.";


//     LOGGER_NOTICE(message);
//     EXPECT_EQ(received.str().size(), 0);
//     clear_output();


//     LOGGER_DisableHeader();

//     auto messageUint8 = "uint8_t: 255 0xff 0XFF 0377"sv;
//     LOGGER_NOTICE("uint8_t: %" PRIu8 " %#" PRIx8 " %#" PRIX8 " %#" PRIo8, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX);
//     EXPECT_EQ(get_message(), messageUint8);
//     clear_message();

//     auto messageInt8 = "int8_t: 127 127 -128 -128"sv;
//     LOGGER_NOTICE("int8_t: %" PRId8 " %" PRIi8 " %" PRIi8 " %" PRIi8, INT8_MAX, INT8_MAX, INT8_MIN, INT8_MIN);
//     EXPECT_EQ(get_message(), messageInt8);
//     clear_message();

//     auto messageUint16 = "uint16_t: 65535 0xffff 0XFFFF 0177777"sv;
//     LOGGER_NOTICE("uint16_t: %" PRIu16 " %#" PRIx16 " %#" PRIX16 " %#" PRIo16, UINT16_MAX, UINT16_MAX, UINT16_MAX, UINT16_MAX);
//     EXPECT_EQ(get_message(), messageUint16);
//     clear_message();

//     auto messageInt16 = "int16_t: 32767 32767 -32768 -32768"sv;
//     LOGGER_NOTICE("int16_t: %" PRId8 " %" PRIi8 " %" PRIi8 " %" PRIi8, INT16_MAX, INT16_MAX, INT16_MIN, INT16_MIN);
//     EXPECT_EQ(get_message(), messageInt16);
//     clear_message();

//     auto messageUint32 = "uint32_t: 4294967295 0xffffffff 0XFFFFFFFF 037777777777"sv;
//     LOGGER_NOTICE("uint32_t: %" PRIu32 " %#" PRIx32 " %#" PRIX32 " %#" PRIo32, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
//     EXPECT_EQ(get_message(), messageUint32);
//     clear_message();

//     auto messageInt32 = "int32_t: 2147483647 2147483647 -2147483648 -2147483648"sv;
//     LOGGER_NOTICE("int32_t: %" PRId32 " %" PRIi32 " %" PRIi32 " %" PRIi32, INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN);
//     EXPECT_EQ(get_message(), messageInt32);
//     clear_message();

//     auto messageUint64 = "uint64_t: 18446744073709551615 0xffffffffffffffff 0XFFFFFFFFFFFFFFFF 01777777777777777777777"sv;
//     LOGGER_NOTICE("uint64_t: %" PRIu64 " %#" PRIx64 " %#" PRIX64 " %#" PRIo64, UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX);
//     EXPECT_EQ(get_message(), messageUint64);
//     clear_message();

//     auto messageInt64 = "int64_t: 9223372036854775807 9223372036854775807 -9223372036854775808 -9223372036854775808"sv;
//     LOGGER_NOTICE("int64_t: %" PRId64 " %" PRIi64 " %" PRIi64 " %" PRIi64, INT64_MAX, INT64_MAX, INT64_MIN, INT64_MIN);
//     EXPECT_EQ(get_message(), messageInt64);
//     clear_message();

//     auto messageChar = "char: a b c";
//     LOGGER_NOTICE("char: %c %c %c", 'a', 'b', 'c');
//     EXPECT_EQ(get_message(), messageChar);
//     clear_message();

//     auto messageCstr = "char: abc";
//     LOGGER_NOTICE("char: %s", "abc");
//     EXPECT_EQ(get_message(), messageCstr);
//     clear_message();
// }