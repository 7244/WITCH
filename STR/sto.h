#pragma once

static bool STR_sto32(const sint32_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
  const uint8_t base_table[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
  };
  uint32_t _v = RSIGN(v);
  *data += 32;
  uint8_t *origdata = *data;
  do{
    **data = base_table[_v % base];
    (*data)--;
    _v /= base;
  }while(_v);
  if(v < 0)
    **data = '-';
  else
    (*data)++;
  *size = origdata - *data + 1;
  return 0;
}
static bool STR_sto64(const sint64_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
  const uint8_t base_table[16] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
  };
  uint64_t _v = RSIGN(v);
  *data += 64;
  uint8_t *origdata = *data;
  do{
    **data = base_table[_v % base];
    (*data)--;
    _v /= base;
  }while(_v);
  if(v < 0)
    **data = '-';
  else
    (*data)++;
  *size = origdata - *data + 1;
  return 0;
}
static bool STR_sto(const sint64_t v, const uint8_t base, uint8_t **data, uintptr_t *size){
  return CONCAT(STR_sto, SYSTEM_BIT)(v, base, data, size);
}
