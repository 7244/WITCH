#pragma once

static bool STR_uto32(uint32_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
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
static bool STR_uto64(uint64_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
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
static bool STR_uto(uint64_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
  return CONCAT(STR_uto, SYSTEM_BIT)(v, base, data, size);
}

static bool STR_uto32_wise(uint32_t v, const uint8_t base, const uintptr_t seek, uint8_t *data){
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
static bool STR_uto64_wise(uint64_t v, const uint8_t base, const uintptr_t seek, uint8_t *data){
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
static bool STR_uto_wise(uint64_t v, const uint8_t base, const uintptr_t seek, uint8_t *data){
  return CONCAT3(STR_uto, SYSTEM_BIT, _wise)(v, base, seek, data);
}
