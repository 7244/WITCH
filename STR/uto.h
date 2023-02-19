#pragma once

bool STR_uto32(uint32_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
	const uint8_t base_table[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	*data += 31;
	uint8_t *origdata = *data;
	do{
		**data = base_table[v % base];
		(*data)--;
		v /= base;
	}while(v);
	(*data)++;
	*size = origdata - *data + 1;
	return 0;
}
#define STR_uto32(v_m, base_m, data_m, size_m) \
	STR_uto32((uint32_t)(v_m), (const uint8_t)(base_m), (uint8_t **)(data_m), (uintptr_t *)(size_m))
bool STR_uto64(uint64_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
	const uint8_t base_table[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	*data += 63;
	uint8_t *origdata = *data;
	do{
		**data = base_table[v % base];
		(*data)--;
		v /= base;
	}while(v);
	(*data)++;
	*size = origdata - *data + 1;
	return 0;
}
#define STR_uto64(v_m, base_m, data_m, size_m) \
	STR_uto64((uint64_t)(v_m), (const uint8_t)(base_m), (uint8_t **)(data_m), (uintptr_t *)(size_m))
#define STR_uto CONCAT(STR_uto, SYSTEM_BIT)

bool STR_uto32_wise(uint32_t v, const uint8_t base, const uintptr_t seek, uint8_t *data){
	const uint8_t base_table[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	data += seek;
	do{
		*data-- = base_table[v % base];
		v /= base;
	}while(v);
	return 0;
}
#define STR_uto32_wise(v_m, base_m, seek_m, data_m) \
	STR_uto32_wise((uint32_t)(v_m), (const uint8_t)(base_m), (const uintptr_t)(seek_m), (uint8_t *)(data_m))
bool STR_uto64_wise(uint64_t v, const uint8_t base, const uintptr_t seek, uint8_t *data){
	const uint8_t base_table[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'a', 'b',
		'c', 'd', 'e', 'f'
	};
	data += seek;
	do{
		*data-- = base_table[v % base];
		v /= base;
	}while(v);
	return 0;
}
#define STR_uto64_wise(v_m, base_m, seek_m, data_m) \
	STR_uto64_wise((uint64_t)(v_m), (const uint8_t)(base_m), (const uintptr_t)(seek_m), (uint8_t *)(data_m))
#define STR_uto_wise CONCAT(STR_uto, _wise)