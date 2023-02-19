#pragma once

#include _WITCH_PATH(T/T.h)

uint32_t _RAND_seed;
PRE{
	_RAND_seed = T_nowi();
}

uint16_t RAND_soft_16(void){
	_RAND_seed = (_RAND_seed * 1103515245 + 12345) & 0x7fffffff;
	return _RAND_seed;
}
uint8_t RAND_soft_8(void){
	return RAND_soft_16();
}
uint32_t RAND_soft_32(void){
	uint32_t ret = 0;
	ret |= RAND_soft_16() << 0;
	ret |= RAND_soft_16() << 16;
	return ret;
}
uint64_t RAND_soft_64(void){
	uint64_t ret = 0;
	ret |= RAND_soft_32() << 0;
	ret |= RAND_soft_32() << 32;
	return ret;
}
uintptr_t RAND_soft_0(void){
	return CONCAT(RAND_soft_, SYSTEM_BIT)();
}

void RAND_soft_ram(void *ptr, uintptr_t size){
	uint16_t *iptr = (uint16_t *)ptr;
	if(size & 1){
		*(uint8_t *)iptr = RAND_soft_8();
		iptr = (uint16_t *)(((uint8_t *)iptr) + 1);
	}
	size >>= 1;
	while(size--){
		*iptr = RAND_soft_16();
		iptr++;
	}
}

sint8_t RAND_soft_snum8(sint8_t min, sint8_t max){
	return RAND_soft_8() % ((max - min) + 1) + min;
}
sint16_t RAND_soft_snum16(sint16_t min, sint16_t max){
	return RAND_soft_16() % ((max - min) + 1) + min;
}
sint32_t RAND_soft_snum32(sint32_t min, sint32_t max){
	return RAND_soft_32() % ((max - min) + 1) + min;
}
sint64_t RAND_soft_snum64(sint64_t min, sint64_t max){
	return RAND_soft_64() % ((max - min) + 1) + min;
}
sintptr_t RAND_soft_snum(sintptr_t min, sintptr_t max){
	return RAND_soft_0() % ((max - min) + 1) + min;
}

#if defined(WOS_UNIX)
	#include <sys/random.h>
#elif defined(WOS_WINDOWS)
	#include <wincrypt.h>
	#pragma comment(lib, "Advapi32.lib")
	HCRYPTPROV _RAND_hCryptProv;
	PRE{
		if(CryptAcquireContext(&_RAND_hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == 0){
			PR_abort();
		}
	}
#endif

void RAND_hard_ram(void *ptr, uintptr_t size){
	#if defined(WOS_UNIX)
		if(getrandom(ptr, size, GRND_RANDOM) < 0){
			PR_abort();
		}
	#elif defined(WOS_WINDOWS)
		if(CryptGenRandom(_RAND_hCryptProv, size, (BYTE *)ptr) == 0){
			PR_abort();
		}
	#else
		#error ?
	#endif
}

uint8_t RAND_hard_8(void){
	uint8_t r;
	RAND_hard_ram(&r, 1);
	return r;
}
uint16_t RAND_hard_16(void){
	uint16_t r;
	RAND_hard_ram(&r, 2);
	return r;
}
uint32_t RAND_hard_32(void){
	uint32_t r;
	RAND_hard_ram(&r, 4);
	return r;
}
uint64_t RAND_hard_64(void){
	uint64_t r;
	RAND_hard_ram(&r, 8);
	return r;
}
uintptr_t RAND_hard_0(void){
	return CONCAT(RAND_hard_, SYSTEM_BIT)();
}

sint8_t RAND_hard_snum8(sint8_t min, sint8_t max){
	return RAND_hard_8() % ((max - min) + 1) + min;
}
sint16_t RAND_hard_snum16(sint16_t min, sint16_t max){
	return RAND_hard_16() % ((max - min) + 1) + min;
}
sint32_t RAND_hard_snum32(sint32_t min, sint32_t max){
	return RAND_hard_32() % ((max - min) + 1) + min;
}
sint64_t RAND_hard_snum64(sint64_t min, sint64_t max){
	return RAND_hard_64() % ((max - min) + 1) + min;
}
sintptr_t RAND_hard_snum(sintptr_t min, sintptr_t max){
	return RAND_hard_0() % ((max - min) + 1) + min;
}
