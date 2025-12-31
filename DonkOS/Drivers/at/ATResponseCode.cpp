#include "ATResponseCode.h"
#include <cstdint>

const char *ATResponseCode_ToString(const ATResponseCode &code) {
    return ATResponseStringMap[static_cast<std::int32_t>(code)];
}
