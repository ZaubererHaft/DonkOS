#ifndef TEST_STRINGCONVERTER_H
#define TEST_STRINGCONVERTER_H

#include <utility>

class IntToStringSettings {
public:
    bool stringTermination = true;
    int32_t minInteger = INT32_MIN;
    int32_t maxInteger = INT32_MAX;
};

class FloatToStringSettings : public IntToStringSettings {
public:
    int32_t placesAfterDot = 2;
    bool useDecimalComma = false;
};

class StringConverter {
public:
    std::pair<bool, int>
    FloatToString(float number, char *buffer, int bufflen);

    std::pair<bool, int>
    FloatToString(float number, char *buffer, int bufflen, const FloatToStringSettings &settings);

    std::pair<bool, int>
    IntegerToString(int number, char *buffer, int bufflen);

    std::pair<bool, int>
    IntegerToString(int number, char *buffer, int bufflen, const IntToStringSettings &settings);
};


#endif //TEST_STRINGCONVERTER_H
