#ifndef TEST_STRINGCONVERTER_H
#define TEST_STRINGCONVERTER_H

#include <utility>

class IntToStringSettings {
public:
    bool string_termination = true;
};

class FloatToStringSettings : public IntToStringSettings {
public:
    int32_t places_after_dot = 2;
    bool use_decimal_comma = false;
};

enum class StringConversionResult
{
    SUCCESS,
    ILLEGAL_BUFFER,
    BUFFER_INSUFFICIENT_FOR_DIGITS,
    BUFFER_INSUFFICIENT_FOR_MINUS,
    BUFFER_INSUFFICIENT_FOR_TERMINATION,
    BUFFER_INSUFFICIENT_FOR_DECIMAL_DOT,
    BUFFER_INSUFFICIENT_FOR_DIGITS_AFTER_DOT,
};

class StringConverter {
public:
    std::pair<StringConversionResult, int>
    FloatToString(float number, char *buffer, int bufflen);

    std::pair<StringConversionResult, int>
    FloatToString(float number, char *buffer, int bufflen, const FloatToStringSettings &settings);

    std::pair<StringConversionResult, int>
    IntegerToString(int number, char *buffer, int bufflen);

    std::pair<StringConversionResult, int>
    IntegerToString(int number, char *buffer, int bufflen, const IntToStringSettings &settings);
};


#endif //TEST_STRINGCONVERTER_H
