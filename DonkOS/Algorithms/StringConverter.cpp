#include <cstdint>
#include <cmath>
#include "StringConverter.h"

static constexpr float zero_threshold = 0.00001;

std::pair<StringConversionResult, int>
StringConverter::FloatToString(float number, char *buffer, int bufflen, const FloatToStringSettings &settings) {
    auto is_nan = false;
    int index;
    if (bufflen < 0 || buffer == nullptr) {
        return {StringConversionResult::ILLEGAL_BUFFER, -1};
    }

    // ugly edge cases - deal with nan and inf manually
    if (std::isnan(number)) {
        is_nan = true;
        if (bufflen >= 3) {
            buffer[0] = 'n';
            buffer[1] = 'a';
            buffer[2] = 'n';
            index = 3;
        } else {
            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS, -1};
        }
    }
    if (std::isinf(number)) {
        is_nan = true;
        if (bufflen >= 3) {
            buffer[0] = 'i';
            buffer[1] = 'n';
            buffer[2] = 'f';
            index = 3;
        } else {
            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS, -1};
        }
    }

    if (is_nan) {
        if (settings.string_termination) {
            if (bufflen > index) {
                buffer[index] = '\0';
                index++;
            } else {
                return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS, -1};
            }
        }
    }
        // if not, perform regular conversion
    else {
        // convert integer part: -12.23 | "-12"
        auto [status, new_index] = IntegerToString((int) number, buffer, bufflen, settings);

        if (status != StringConversionResult::SUCCESS) {
            return {status, -1};
        }
        // now continue with float part
        else {
            index = new_index;

            //negativity has already been dealt with in the integer conversion. continue with positive number: -12.23 -> 12.23 | "-12"
            if (number < 0) {
                number *= -1;
            }

            // remove places before decimal dot: 12.23 -> 0.23 | "-12" ).
            // remark: adds additional conversion error dut to float manipulation and narrowing
            number = number - ((uint32_t) number);

            // decimal dot if space for it is available: 0.23 | "-12."
            if (index < bufflen) {
                buffer[index] = settings.use_decimal_comma ? ',' : '.';
                index++;
            } else {
                return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DECIMAL_DOT, -1};
            }

            auto places_after_dot = settings.places_after_dot;

            // get all places after the decimal dot
            // idea: Multiply by 10 to extract the next decimal digit
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

            // exited loop regularly? (it might be that number is still positive because we only want to display a part of it.
            // it might also be possible that places_after_dot is greater than zero in the case number got zero before, and we need to fill up the remaining places))
            if (index < bufflen) {
                // ... try to fill remaining places with missing zeros, if necessary
                while (places_after_dot > 0 && index < bufflen) {
                    buffer[index] = '0';
                    index++;
                    places_after_dot--;
                }

                if (places_after_dot == 0) {
                    // add line termination if requested and space available
                    if (settings.string_termination) {
                        if (index < bufflen) {
                            buffer[index] = '\0';
                        } else {
                            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_TERMINATION, -1};
                        }
                    }
                }
            } else {
                return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS_AFTER_DOT, -1};
            }
        }
    }

    return {StringConversionResult::SUCCESS, index};
}

std::pair<StringConversionResult, int>
StringConverter::IntegerToString(int number, char *buffer, int bufflen, const IntToStringSettings &settings) {
    if (bufflen < 0 || buffer == nullptr) {
        return {StringConversionResult::ILLEGAL_BUFFER, -1};
    }

    // handle case where number == INT_MIN
    // idea: convert to INT_MAX and remember that number was negative. then, increment the places during conversion
    // to fix the number that was cut off
    bool negative = number < 0;
    bool int_min = number == INT32_MIN;
    if (negative) {
        if (int_min) {
            number = INT32_MAX;
        } else {
            number *= -1;
        }
    }

    int32_t index = 0;
    int32_t add = int_min ? 1 : 0;

    // now: fill buffers with individual places but in wrong order (the least significant first)
    do {
        auto place = number % 10;

        // handle case where number = INT_MIN by adding 1 to the digit that got changed due to conversion to INT_MAX above
        // in addition: propagate carry if place was 9 (only if INT_MAX was of the form xxx..xxx9)
        place += add;
        add = place / 10;
        place %= 10;

        // convert place to character and store it to string
        buffer[index] = static_cast<char>(static_cast<int32_t>('0') + place);
        index++;
        number /= 10;
    } while (number > 0 && index < bufflen);

    // if number == 0  -> we can make sure that number was converted - This might already be sufficient for success if number is positive and no line termination requested
    // (because then index == bufflen, but we are already done)
    if (number > 0) {
        return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS, -1};
    }

    //if carry from INT_MIN / INT_MAX conversion was transported to the MSB, add it to string
    if (add > 0) {
        if (index < bufflen) {
            buffer[index] = '1';
            index++;
        } else {
            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_DIGITS, -1};
        }
    }

    // add minus sign for negative numbers if space in buffer is available
    if (negative) {
        if (index < bufflen) {
            buffer[index] = '-';
            index++;
        } else {
            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_MINUS, -1};
        }
    }

    // have we been successful in the previous step? -> try to add line terminating character if requested and space is available
    if (settings.string_termination) {
        if (index < bufflen) {
            buffer[index] = '\0';
        } else {
            return {StringConversionResult::BUFFER_INSUFFICIENT_FOR_TERMINATION, -1};
        }
    }

    //finally: all steps were successful -> swap array to get the correct number
    for (int i = 0; i < index / 2; ++i) {
        int swapIndex = index - 1 - i;
        auto tmp = buffer[i];
        buffer[i] = buffer[swapIndex];
        buffer[swapIndex] = tmp;
    }

    return {StringConversionResult::SUCCESS, index};
}

std::pair<StringConversionResult, int> StringConverter::FloatToString(float number, char *buffer, int bufflen) {
    return FloatToString(number, buffer, bufflen, {});
}

std::pair<StringConversionResult, int> StringConverter::IntegerToString(int number, char *buffer, int bufflen) {
    return IntegerToString(number, buffer, bufflen, {});
}
