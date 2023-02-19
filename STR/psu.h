#pragma once

#include _WITCH_PATH(STR/common/common.h)

uint32_t STR_psu32(const uint8_t *str, uintptr_t size){
	uint32_t res = 0;
	for(uintptr_t i = 0; i < size; i++)
		res = res * 10 + str[i] - '0';
	return res;
}
#define STR_psu32(str_m, size_m) \
	STR_psu32((const uint8_t *)(str_m), (uintptr_t)(size_m))
uint64_t STR_psu64(const uint8_t *str, uintptr_t size){
	uint64_t res = 0;
	for(uintptr_t i = 0; i < size; i++)
		res = res * 10 + str[i] - '0';
	return res;
}
#define STR_psu64(str_m, size_m) \
	STR_psu64((const uint8_t *)(str_m), (uintptr_t)(size_m))
#define STR_psu CONCAT(STR_psu, SYSTEM_BIT)

uint32_t STR_psu32_iguess(const uint8_t *str, uintptr_t *i){
	uint32_t res = 0;
	for(; STR_ischar_digit(str[*i]); (*i)++)
		res = res * 10 + str[*i] - '0';
	return res;
}
#define STR_psu32_iguess(str_m, i_m) \
	STR_psu32_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
uint64_t STR_psu64_iguess(const uint8_t *str, uintptr_t *i){
	uint64_t res = 0;
	for(; STR_ischar_digit(str[*i]); (*i)++)
		res = res * 10 + str[*i] - '0';
	return res;
}
#define STR_psu64_iguess(str_m, i_m) \
	STR_psu64_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
#define STR_psu_iguess CONCAT(STR_psu, _iguess)

uint32_t STR_psu32_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
	uint32_t res = 0;
	for(; *i < size; (*i)++){
		if(!STR_ischar_digit(str[*i]))
			break;
		res = res * 10 + str[*i] - '0';
	}
	return res;
}
#define STR_psu32_i(str_m, i_m, size_m) \
	STR_psu32_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
uint64_t STR_psu64_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
	uint64_t res = 0;
	for(; *i < size; (*i)++){
		if(!STR_ischar_digit(str[*i]))
			break;
		res = res * 10 + str[*i] - '0';
	}
	return res;
}
#define STR_psu64_i(str_m, i_m, size_m) \
	STR_psu64_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
#define STR_psu_i CONCAT3(STR_psu, SYSTEM_BIT, _i)