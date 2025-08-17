#include <cstdint>
#include <cmath>
#include "StringConverter.h"

static constexpr float zero_threshold = 0.00001;

std::pair<bool, int>
StringConverter::FloatToString(float number, int places, char *buffer, int bufflen, bool lineTermination) {

    // convert integer part: -12.23 | "-12"
    auto [success, index] = IntegerToString((int) number, buffer, bufflen);

    // now continue with float part
    if (success) {
        success = false;

        //negativity has already been dealt with in the integer conversion. continue with positive number: -12.23 -> 12.23 | "-12"
        if (number < 0) {
            number *= -1;
        }

        // remove places before decimal dot: 12.23 -> 0.23 | "-12" ).
        // remark: adds additional conversion error dut to float manipulation and narrowing
        number = number - ((uint32_t) number);

        if (number < zero_threshold) {
            // special case: float is actually an integer and we are already done
            success = true;
        } else {
            // decimal dot if space for it is available: 0.23 | "-12."
            if (index < bufflen) {
                buffer[index] = '.';
                index++;
            }

            if (index < bufflen) {
                // get all places after the decimal dot
                // idea: multiply with 10 to shift one place after the decimal dot to one before it
                // then cut off float part and convert digit to number and add it to the buffer
                // repeat as long as the original floating point is not (close to) zero
                // remark: adds additional conversion error dut to float manipulation and narrowing
                // In the example: 1) 0.23 -> 2.3 -> 0.3 | "-12.2"
                //                 2) 0.3  -> 3.0 -> 0.0 | "-12.23"
                // (ideal sequence)
                do {
                    number = number * 10.0f;
                    uint32_t place = number;
                    buffer[index] = '0' + place;
                    number -= place;
                    index++;
                    places--;
                } while (number > zero_threshold && index < bufflen && places > 0);

                // all places or number fully converted?
                if (places == 0 || std::fabs(number) <= zero_threshold) {

                    // add line termination if requested and space available
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
        }
    }

    return {success, index};
}

std::pair<bool, int> StringConverter::IntegerToString(int number, char *buffer, int bufflen, bool lineTermination) {
    bool success = false;
    auto index = 0;

    if (bufflen > 0 && buffer != nullptr) {
        auto negative = number < 0;

        if (negative) {
            number *= -1;
        }

        // fill buffers with individual places but in wrong order (the least significant first)
        do {
            auto place = number % 10;
            buffer[index] = '0' + place;
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
