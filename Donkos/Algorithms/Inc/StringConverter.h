#ifndef TEST_STRINGCONVERTER_H
#define TEST_STRINGCONVERTER_H

#include <utility>

class StringConverter {
public:
    std::pair<bool, int> ToString(float number, int places, char *buffer, int bufflen);

    std::pair<bool, int> ToString(int number, char *buffer, int bufflen);
};


#endif //TEST_STRINGCONVERTER_H
