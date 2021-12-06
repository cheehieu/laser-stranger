#include "eg_common.h"

bool eg_validate_byte(uint8_t val, uint8_t min, uint8_t max) {
    if (val>=min && val<=max) {
	return true;
    } else {
	return false;
    }
}

bool eg_validate_word(uint32_t val, uint32_t min, uint32_t max) {
    if (val>=min && val<=max) {
	return true;
    } else {
	return false;
    }
}
