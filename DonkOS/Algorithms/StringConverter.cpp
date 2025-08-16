#include <cstdint>
#include <algorithm>
#include "StringConverter.h"

std::pair<bool, int>
StringConverter::ToString(float number, int places, char *buffer, int bufflen, bool lineTermination) {

    // 1: convert integer part.
    auto [success, index] = ToString((int) number, buffer, bufflen);

    if (success) {
        success = false;
        auto negative = number < 0;

        if (negative) {
            number *= -1;
        }

        number = number - ((uint32_t) number);

        // decimal dot if space available
        if (index < bufflen) {
            buffer[index] = '.';
            index++;
        }

        if (index < bufflen) {
            // get all places after the decimal dot
            //remark: this function has a conversion error (due to float representation and multiplication)
            do {
                number = number * 10.0f;
                uint32_t place = number;
                buffer[index] = 48 + place;
                number -= place;
                index++;
                places--;
            } while (number > 0 && index < bufflen && places > 0);
        }

        // all places converted?
        if (places <= 0) {

            // add line termination if necessary and space available
            if (lineTermination) {
                if (index < bufflen) {
                    buffer[index] = '\0';
                    success = true;
                }
            } else {
                success = true;
            }
        }

    }
    return {success, index};
}

std::pair<bool, int> StringConverter::ToString(int number, char *buffer, int bufflen, bool lineTermination) {
    bool success = false;
    auto index = 0;

    if (bufflen > 0) {
        auto negative = number < 0;

        if (negative) {
            number *= -1;
        }

        // fill buffers with individual places but in wrong order (the least significant first)
        do {
            auto place = number % 10;
            buffer[index] = 48 + place;
            index++;
            number /= 10;
        } while (number > 0 && index < bufflen);

        // if number is 0 we can make sure that all places have been converted. This might already be sufficient for success if number is positive and no line termination requested
        if (number == 0) {

            // add minus sign for negative numbers if space in buffer available and save success as intermediate step
            if (negative) {
                if (index < bufflen) {
                    buffer[index] = '-';
                    index++;
                    success = true;
                }
            } else {
                success = true;
            }

            // have we been successful in the previous step? -> try to add line terminating character if requested and space is available
            if (success) {
                if (lineTermination) {
                    if (index < bufflen) {
                        success = true;
                        buffer[index] = '\0';
                    } else {
                        success = false;
                    }
                }
            }

            //finally: all steps were successful -> swap array to get the correct number
            if (success) {
                for (int i = 0; i < index / 2; ++i) {
                    int swapIndex = index - 1 - i;
                    auto tmp = buffer[i];
                    buffer[i] = buffer[swapIndex];
                    buffer[swapIndex] = tmp;
                }
            }
        }
    }

    return {success, index};
}
