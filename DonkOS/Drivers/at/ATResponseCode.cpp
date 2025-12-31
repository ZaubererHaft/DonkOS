#include "ATResponseCode.h"
#include <cstdint>

const char *ATResponseCode_ToString(const ATResponseCode &code) {
    if (static_cast<std::size_t>(code) >= sizeof(ATResponseStringMap)) {
        return "AT_UNKNOWN";
    }
    return ATResponseStringMap[static_cast<std::int32_t>(code)];
}
