#include <gtest/gtest.h>
#include "StringConverter.h"
#include <string>
#include <random>


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

TEST_F(StringConverterTest, TestFloat) {

    std::pair<float, const char *> testData[]{{1.1f, "1.1"}, {0.5, "0.5"}, {-114.25, "-114.25"}, {12, "12"}};

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
