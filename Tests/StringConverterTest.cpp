#include <gtest/gtest.h>
#include "StringConverter.h"
#include <string>
#include <random>
#include <cstring>


class StringConverterTest : public testing::Test {
protected:
    StringConverterTest() {

    }

    static constexpr int32_t BufferSize = 32;
    StringConverter converter{};
    char buffer[BufferSize]{};
};


TEST_F(StringConverterTest, TestInteger_InvalidBuffer) {

    auto [result1, index1] = converter.IntegerToString(123, nullptr, BufferSize);
    ASSERT_FALSE(result1);

    auto [result2, index2] = converter.IntegerToString(123, nullptr, 0);
    ASSERT_FALSE(result2);

    for (int i = 0; i <= 4; ++i) {
        auto [result3, index3] = converter.IntegerToString(-123, buffer, i);
        ASSERT_FALSE(result3);
    }

    for (int i = 0; i <= 3; ++i) {
        auto [result4, index4] = converter.IntegerToString(123, buffer, i);
        ASSERT_FALSE(result4);
    }
}

TEST_F(StringConverterTest, TestInteger_RandomBruteForce) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(INT32_MIN, INT32_MAX);

    for (int n = 0; n < 500'000; ++n) {
        int32_t number = distr(gen);
        auto [result, length] = converter.IntegerToString(number, buffer, BufferSize);

        ASSERT_TRUE(result);
        ASSERT_EQ(std::string{buffer}, std::to_string(number))
                                    << "Test number was " << number << " result is " << buffer << " and expected was "
                                    << std::to_string(number) << "\n";
        ASSERT_EQ(length, std::string{buffer}.length());
    }
}

TEST_F(StringConverterTest, TestInteger_EdgeCases) {

    auto [result0, index0] = converter.IntegerToString(0, buffer, 1);
    ASSERT_FALSE(result0);

    auto [result1, index1] = converter.IntegerToString(0, buffer, BufferSize);
    ASSERT_TRUE(result1);
    ASSERT_EQ(std::string{"0"}, std::string{buffer});

    std::memset(buffer, 0x0, BufferSize);
    buffer[0] = buffer[1] = (char) 0xFF;
    auto [result2, index2] = converter.IntegerToString(0, buffer, 1, false);
    ASSERT_TRUE(result2);
    ASSERT_EQ('0', buffer[0]);
    ASSERT_EQ((char) 0xFF, buffer[1]);

    std::memset(buffer, 0x0, BufferSize);
    buffer[3] = (char) 0xFF;
    auto [result3, index3] = converter.IntegerToString(-12, buffer, 3, false);
    ASSERT_TRUE(result3);
    ASSERT_EQ('-', buffer[0]);
    ASSERT_EQ('1', buffer[1]);
    ASSERT_EQ('2', buffer[2]);
    ASSERT_EQ((char) 0xFF, buffer[3]);

    std::memset(buffer, 0x0, BufferSize);
    auto [result4, index4] = converter.IntegerToString(INT32_MIN, buffer, BufferSize);
    ASSERT_TRUE(result4);
    ASSERT_EQ(std::string{"-2147483648"}, std::string{buffer});
}

TEST_F(StringConverterTest, TestFloat) {

    std::pair<float, const char *> testData[]{{1.1f,                                     "1.10"},
                                              {0.5,                                      "0.50"},
                                              {-114.25,                                  "-114.25"},
                                              {12,                                       "12.00"},
                                              {std::numeric_limits<float>::quiet_NaN(), "nan"},
                                              {std::numeric_limits<float>::infinity(), "inf"},
                                              };

    for (auto &test_set: testData) {
        float number = test_set.first;
        const char *expected = test_set.second;

        auto [result, length] = converter.FloatToString(number, 2, buffer, BufferSize);


        ASSERT_TRUE(result);
        ASSERT_EQ(std::string{buffer}, std::string{expected})
                                    << "Test number was " << number << " result is " << buffer << " and expected was "
                                    << expected << "\n";
    }


}
