#include <cstdint>
#include <cmath>
#include "StringConverter.h"

static constexpr float zero_threshold = 0.00001;

std::pair<bool, int>
StringConverter::FloatToString(float number, int places_after_dot, char *buffer, int bufflen, bool stringTermination) {
    auto is_nan = false;
    auto success = false;
    int index;

    // ugly edge cases - deal with nan and inf manually
    if (std::isnan(number)) {
        is_nan = true;
        if (bufflen >= 3) {
            buffer[0] = 'n';
            buffer[1] = 'a';
            buffer[2] = 'n';
            index = 3;
            success = true;
        }
    }
    if (std::isinf(number)) {
        is_nan = true;
        if (bufflen >= 3) {
            buffer[0] = 'i';
            buffer[1] = 'n';
            buffer[2] = 'f';
            index = 3;
            success = true;
        }
    }

    if (is_nan) {
        if (stringTermination) {
            if (bufflen >= 4) {
                buffer[3] = '\0';
                index = 4;
            } else {
                success = false;
            }
        }
    }
    // if not, perform regular conversion
    else {
        // convert integer part: -12.23 | "-12"
        auto result = IntegerToString((int) number, buffer, bufflen);
        success = result.first;
        index = result.second;

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
                //
                // now a while (not do-while) loop because "number" might already be zero (in case of an integer value stored in a float)
                while (number > zero_threshold && index < bufflen && places_after_dot > 0) {
                    number = number * 10.0f;
                    uint32_t place = number;
                    buffer[index] = '0' + place;
                    number -= place;
                    index++;
                    places_after_dot--;
                }

                // exited loop regularly?
                if (index < bufflen) {
                    // ... try to fill remaining places with missing zeros
                    while (places_after_dot > 0 && index < bufflen) {
                        buffer[index] = '0';
                        index++;
                        places_after_dot--;
                    }

                    // add line termination if requested and space available
                    if (stringTermination) {
                        if (index < bufflen) {
                            buffer[index] = '\0';
                            success = true;
                        }
                    }
                        // if not, we are succeeding if all places have been converted
                    else {
                        success = places_after_dot == 0;
                    }
                }
            }
        }
    }

    if (!success) {
        index = -1;
    }
    return {success, index};
}

std::pair<bool, int> StringConverter::IntegerToString(int number, char *buffer, int bufflen, bool stringTermination) {
    bool success = false;
    auto index = 0;

    if (bufflen > 0 && buffer != nullptr) {
        auto negative = number < 0;
        auto int_min = number == INT32_MIN;

        if (negative) {
            if (int_min) {
                number = INT32_MAX;
            } else {
                number *= -1;
            }
        }

        // now: fill buffers with individual places but in wrong order (the least significant first)
        do {
            auto place = number % 10;
            // handle case where number = INT_MIN by adding 1 to the first digit that got lost due to conversion to INT_MAX above
            if (int_min && index == 0) {
                place++;
            }
            buffer[index] = '0' + place;
            index++;
            number /= 10;
        } while (number > 0 && index < bufflen);

        // if number == 0  -> we can make sure that number was converted - This might already be sufficient for success if number is positive and no line termination requested
        if (number <= 0) {
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
                if (stringTermination) {
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

    if (!success) {
        index = -1;
    }

    return {success, index};
}
