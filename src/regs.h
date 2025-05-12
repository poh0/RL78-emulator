#pragma once

#include <stdint.h>

typedef union {
	uint8_t R[2];
	uint16_t RP;
} GPR_pair_u;

typedef union {
	uint8_t R[8];
	uint16_t RP[4];
} GPR_u;