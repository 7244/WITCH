#pragma once

#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(STR/uto.h)

#if SYSTEM_BIT == 8
	#define bdn10 2
#elif SYSTEM_BIT == 16
	#define bdn10 4
#elif SYSTEM_BIT == 32
	#define bdn10 9
#elif SYSTEM_BIT == 64
	#define bdn10 19
#else
	#error ?
#endif

const uintptr_t _STR_fto_digit10[bdn10 + 1] = {
	LITERAL(uintptr_t, 0),
#if SYSTEM_BIT >= 8
	LITERAL(uintptr_t, 10),
	LITERAL(uintptr_t, 100)
#endif
#if SYSTEM_BIT >= 16
	,
	LITERAL(uintptr_t, 1000),
	LITERAL(uintptr_t, 10000)
#endif
#if SYSTEM_BIT >= 32
	,
	LITERAL(uintptr_t, 100000),
	LITERAL(uintptr_t, 1000000),
	LITERAL(uintptr_t, 10000000),
	LITERAL(uintptr_t, 100000000),
	LITERAL(uintptr_t, 1000000000)
#endif
#if SYSTEM_BIT >= 64
	,
	LITERAL(uintptr_t, 10000000000),
	LITERAL(uintptr_t, 100000000000),
	LITERAL(uintptr_t, 1000000000000),
	LITERAL(uintptr_t, 10000000000000),
	LITERAL(uintptr_t, 100000000000000),
	LITERAL(uintptr_t, 1000000000000000),
	LITERAL(uintptr_t, 10000000000000000),
	LITERAL(uintptr_t, 100000000000000000),
	LITERAL(uintptr_t, 1000000000000000000),
	LITERAL(uintptr_t, 10000000000000000000)
#endif
};

bool STR_fto128_base10(f128_t v, uintptr_t precision, uint8_t *data, uintptr_t *size){
	const uint8_t *origdata = data;
	if(v < 0)
		*data++ = '-';
	v = RSIGN(v);
	uintptr_t left = v;
	v -= left;
	{
		uint8_t *utostr = data;
		uintptr_t utosize;
		if(STR_uto(left, 10, &utostr, &utosize))
			return 1;
		MEM_move(utostr, data, utosize);
		data += utosize;
	}
	if(precision)
		*data++ = '.';
	while(precision){
		uintptr_t n = precision > bdn10 ? bdn10 : precision;
		precision -= n;
		v *= _STR_fto_digit10[n];
		left = v;
		v -= left;
		MEM_set('0', data, n);
		if(STR_uto_wise(left, 10, n - 1, data))
			return 1;
		data += n;
	}
	*size = data - origdata;
	return 0;
}
#define STR_fto128_base10(v_m, precision_m, data_m, size_m) \
	STR_fto128_base10((f128_t)(v_m), (uintptr_t)(precision_m), (uint8_t *)(data_m), (uintptr_t *)(size_m))

#undef bdn10