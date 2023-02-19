void MEM_copy(const void *src, void *dst, uintptr_t size){
	const uint8_t *_src = (const uint8_t *)src;
	uint8_t *_dst = (uint8_t *)dst;
	while(size--){
		*_dst++ = *_src++;
	}
}

void MEM_move(void *src, void *dst, uintptr_t size){
	const uint8_t *_src = (const uint8_t *)src;
	uint8_t *_dst = (uint8_t *)dst;
	uint8_t b;
	if(size){
		b = *_src++;
	}
	while(size--){
		uint8_t t;
		if(size){
			t = *_src++;
		}
		*_dst++ = b;
		b = t;
	}
}

void MEM_set(const uint8_t src, void *dst, const uintptr_t size){
	for(uintptr_t isize = 0; isize < size; isize++){
		((uint8_t *)dst)[isize] = src;
	}
}

bool MEM_cmp(const void *src, const void *dst, const uintptr_t size){
	for(uintptr_t isize = 0; isize < size; isize++){
		if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
			return 1;
		}
	}
	return 0;
}

bool MEM_ncmp(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
	if(src_size < dst_size){
		return 0;
	}
	for(uintptr_t isize = 0; isize < dst_size; isize++){
		if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
			return 1;
		}
	}
	return 0;
}

bool MEM_ncmpn(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
	if(src_size != dst_size){
		return 0;
	}
	for(uintptr_t isize = 0; isize < dst_size; isize++){
		if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
			return 1;
		}
	}
	return 0;
}

uint8_t *MEM_findmem(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
	const uint8_t *src_end = (const uint8_t *)src + src_size;
	while(((const uint8_t *)src + dst_size) < src_end){
		if(MEM_cmp(src, dst, dst_size)){
			return (uint8_t *)src;
		}
		src = (const void *)((const uint8_t *)src + 1);
	}
	return 0;
}

uint8_t *MEM_findchr(const void *src, const uintptr_t src_size, const uint8_t c){
	const uint8_t *_src = (const uint8_t *)src;
	const uint8_t *_src_limit = &_src[src_size];
	while(_src != _src_limit){
		if(*_src == c){
			return (uint8_t *)_src;
		}
		_src++;
	}
	return 0;
}

uint8_t *MEM_findchr0(const void *src, const uint8_t c){
	for(; *(uint8_t *)src != c; src = (const void *)((uint8_t *)src + 1));
	return (uint8_t *)src;
}

uintptr_t MEM_cstreu(const void *cstr){
	return (const uint8_t *)MEM_findchr0(cstr, 0) - (const uint8_t *)cstr;
}

uint8_t *MEM_cstrep(const void *cstr){
	return (uint8_t *)MEM_findchr0(cstr, 0);
}
