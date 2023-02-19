#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(STR/common/common.h)

#define common(bit_m, ref_m) \
	CONCAT3(f, bit_m, _t) res = 0; \
	CONCAT3(f, bit_m, _t) sign; \
	switch(str[ref_m i]){ \
		case '-': \
			sign = -1; \
			(ref_m i)++; \
			break; \
		case '+': \
			sign = 1; \
			(ref_m i)++; \
			break; \
		default: \
			if(!STR_ischar_float(str[ref_m i])) \
				return 0; \
			sign = 1; \
	}

f32_t STR_psf32(const uint8_t *str, uintptr_t size){
	if(!size)
		return 0;
	uintptr_t i = 0;
	common(32,)
	for(; i < size; i++){
		if(str[i] == '.')
			goto after;
		res = res * 10 + str[i] - '0';
	}
	return res * sign;
	after:;
	f32_t div = 10;
	for(i++; i < size; i++){
		res += (f32_t)(str[i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf32(str_m, size_m) \
	STR_psf32((const uint8_t *)(str_m), (uintptr_t)(size_m))
f64_t STR_psf64(const uint8_t *str, uintptr_t size){
	if(!size)
		return 0;
	uintptr_t i = 0;
	common(64,)
	for(; i < size; i++){
		if(str[i] == '.')
			goto after;
		res = res * 10 + str[i] - '0';
	}
	return res * sign;
	after:;
	f64_t div = 10;
	for(i++; i < size; i++){
		res += (f64_t)(str[i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf64(str_m, size_m) \
	STR_psf64((const uint8_t *)(str_m), (uintptr_t)(size_m))
#define STR_psf CONCAT(STR_psf, SYSTEM_BIT)

f32_t STR_psf32_iguess(const uint8_t *str, uintptr_t *i){
	common(32, *)
	for(; STR_ischar_digit(str[*i]); (*i)++){
		if(str[*i] == '.')
			goto after;
		res = res * 10 + str[*i] - '0';
	}
	return res * sign;
	after:;
	f32_t div = 10;
	for((*i)++; str[*i]; (*i)++){
		res += (f32_t)(str[*i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf32_iguess(str_m, i_m) \
	STR_psf32_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
f64_t STR_psf64_iguess(const uint8_t *str, uintptr_t *i){
	common(64, *)
	for(; STR_ischar_digit(str[*i]); (*i)++){
		if(str[*i] == '.')
			goto after;
		res = res * 10 + str[*i] - '0';
	}
	return res * sign;
	after:;
	f64_t div = 10;
	for((*i)++; str[*i]; (*i)++){
		res += (f64_t)(str[*i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf64_iguess(str_m, i_m) \
	STR_psf64_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
#define STR_psf_iguess CONCAT3(STR_psf, SYSTEM_BIT, _iguess)

f32_t STR_psf32_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
	common(32, *)
	for(; *i < size; (*i)++){
		if(str[*i] == '.')
			goto after;
		else if(!STR_ischar_digit(str[*i]))
			break;
		res = res * 10 + str[*i] - '0';
	}
	return res * sign;
	after:;
	f32_t div = 10;
	for((*i)++; *i < size; (*i)++){
		res += (f32_t)(str[*i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf32_i(str_m, i_m, size_m) \
	STR_psf32_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
f64_t STR_psf64_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
	common(64, *)
	for(; *i < size; (*i)++){
		if(str[*i] == '.')
			goto after;
		else if(!STR_ischar_digit(str[*i]))
			break;
		res = res * 10 + str[*i] - '0';
	}
	return res * sign;
	after:;
	f64_t div = 10;
	for((*i)++; *i < size; (*i)++){
		res += (f64_t)(str[*i] - '0') / div;
		div *= 10;
	}
	return res * sign;
}
#define STR_psf64_i(str_m, i_m, size_m) \
	STR_psf64_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
#define STR_psf_i CONCAT3(STR_psf, SYSTEM_BIT, _i)

#undef common