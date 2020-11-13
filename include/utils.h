/*
 * Copyright 2020 Bryson Banks and David Campbell.  All rights reserved.
 */

#ifndef _WSDS_UTILS_DEFINE
#define _WSDS_UTILS_DEFINE

#include <math.h>

namespace WSDS {

/*
 * Internal data structures and functions not expected to be used
 * by user applications utilizing the WSDS user-level scheduler.
 */
namespace internal {

// is the input value a power of 2?
inline bool is_pow2(unsigned int val) {
    return val && !(val & (val - 1));
}

// calculate the "next" power of 2 >= the input value
inline int next_pow2(unsigned int val) {
    if (is_pow2(val)) {
        return val;
    }

	val--;
	val |= (val >> 1);
	val |= (val >> 2);
	val |= (val >> 4);
	val |= (val >> 8);
	val |= (val >> 16);

	return val + 1;
}

} // namespace internal

} // namespace WSDS

#endif // _WSDS_UTILS_DEFINE
