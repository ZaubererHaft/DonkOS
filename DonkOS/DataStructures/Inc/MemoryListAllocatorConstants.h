/**
 * \file
 * \{
 * \defgroup Grp_OUtlMemConstants OUtlMemConstants
 * \{
 * \ingroup  Grp_OUtlMem
 * \brief    Constants for memory allocation.
 * \details
 *
 * \author   l.kratzl
 */

#ifndef OUTLMEMLISTALLOCATORCONSTANTS_H_
#define OUTLMEMLISTALLOCATORCONSTANTS_H_

#include <cstdint>

namespace OUtlMemConstants {
    static constexpr std::uintptr_t st_cPtrSize = sizeof(uintptr_t);
    static constexpr std::uintptr_t st_cOverheadInPtrSize = 2U;
    static constexpr std::uintptr_t st_cOverheadInBytes = st_cOverheadInPtrSize * st_cPtrSize;
    static constexpr std::uintptr_t st_cUsedMask = static_cast<std::uintptr_t>(1) << (st_cPtrSize * 8U - 1U);
    static constexpr std::uintptr_t st_cLenMask = st_cUsedMask - 1U;
} // namespace OUtlMemConstants

#endif // OUTLMEMLISTALLOCATORCONSTANTS_H_

/**
 * \}
 * \}
 */