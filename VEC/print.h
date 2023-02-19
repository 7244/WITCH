#pragma once

#include _WITCH_PATH(VEC/VEC.h)
#include _WITCH_PATH(STR/ttcc.h)

bool _VEC_vprint_buf(STR_ttcc_t *ttcc){
	VEC_t *vec = (VEC_t *)ttcc->arg;
	vec->Current += ttcc->c;
	VEC_handle(vec);
	ttcc->ptr = &vec->ptr[vec->Current];
	ttcc->c = 0;
	ttcc->p = vec->Possible - vec->Current;
	return 0;
}
bool VEC_vprint(VEC_t *vec, const char *format, va_list argv){
	STR_ttcc_t ttcc;
	ttcc.ptr = &vec->ptr[vec->Current];
	ttcc.c = 0;
	ttcc.p = vec->Possible - vec->Current;
	ttcc.f = _VEC_vprint_buf;
	ttcc.arg = (void *)vec;
	if(STR_vFSttcc(&ttcc, format, argv)){
		return 1;
	}
	if(ttcc.c){
		return _VEC_vprint_buf(&ttcc);
	}
	return 0;
}
bool VEC_print(VEC_t *vec, const char *format, ...){
	va_list argv;
	va_start(argv, format);
	bool r = VEC_vprint(vec, format, argv);
	va_end(argv);
	return r;
}
