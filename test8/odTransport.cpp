#include "odTransport.hpp"

// Get OD entry sizes

#define ENUM_VAL0(name, size) size
#define ENUM_VAL(name, size) size
#define ENUM_CONSTANT(name, value) 0

uint8_t odSize[] = {
#include "od_vals.h"
};

#undef ENUM_VAL0
#undef ENUM_VAL
#undef ENUM_CONSTANT

