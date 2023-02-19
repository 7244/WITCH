#pragma once

#include _WITCH_PATH(WITCH.h)

#include _WITCH_PATH(include/openssl.h)

#define HASH_SHA256_size 32

void HASH_SHA256(void *hash, const void *data, uintptr_t size){
	SHA256((const unsigned char *)data, size, (unsigned char *)hash);
}

#define HASH_SHA512_size 64

void HASH_SHA512(void *hash, const void *data, uintptr_t size){
	SHA512((const unsigned char *)data, size, (unsigned char *)hash);
}
