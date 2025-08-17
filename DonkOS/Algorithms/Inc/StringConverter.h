#ifndef TEST_STRINGCONVERTER_H
#define TEST_STRINGCONVERTER_H

#include <utility>

class StringConverter {
public:
    std::pair<bool, int> FloatToString(float number, int places, char *buffer, int bufflen, bool lineTermination = true);

    std::pair<bool, int> IntegerToString(int number, char *buffer, int bufflen, bool lineTermination = true);
};


#endif //TEST_STRINGCONVERTER_H
