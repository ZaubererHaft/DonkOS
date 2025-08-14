#include <cstdint>
#include <algorithm>
#include "StringConverter.h"

std::pair<bool, int> StringConverter::ToString(float number, int places, char *buffer, int bufflen) {

    auto [success, index] = ToString((int) number, buffer, bufflen);

    if (success) {
        success = false;
        auto negative = number < 0;

        if (negative) {
            number *= -1;
        }

        number = number - ((uint32_t) number);

        if (index < bufflen) {
            buffer[index] = '.';
            index++;
        }

        if (index < bufflen) {
            do {
                number = number * 10.0f;
                uint32_t place = number;
                buffer[index] = 48 + place;
                number -= place;
                index++;
                places--;
            } while (number > 0 && index < bufflen && places > 0);
        }

        if (index < bufflen) {
            buffer[index] = '\0';
            success = true;
        }
    }
    return {success, index};
}

std::pair<bool, int> StringConverter::ToString(int number, char *buffer, int bufflen) {
    bool success = false;
    auto index = 0;

    if (bufflen > 0) {
        auto negative = number < 0;

        if (negative) {
            number *= -1;
        }

        do {
            auto place = number % 10;
            buffer[index] = 48 + place;
            index++;
            number /= 10;
        } while (number > 0 && index < bufflen);

        if (index < bufflen) {
            if (negative) {
                buffer[index] = '-';
                index++;
            }

            if (index < bufflen) {
                buffer[index] = '\0';

                for (int i = 0; i < index / 2; ++i) {
                    int swapIndex = index - 1 - i;
                    auto tmp = buffer[i];
                    buffer[i] = buffer[swapIndex];
                    buffer[swapIndex] = tmp;
                }

                success = true;
            }
        }
    }

    return {success, index};
}
