/* this header is not for mortals to read */

/* what means DecodeError_*_e
  Done
    http decoding is done.
    decode function must be not called if next data is not part of http
  BadData
    Data breaks http rules or its not even related with http
    decode function must be not called
  LineLengthLimit
    Data has longer line length than what it susposed to have as limit.
    decode function must be not called from where you get Data
    and person who made this Data must be tortured in hell
  NotFriendlyData
    when Data looks like generated by slowloris like not friendly tool.
    decode function still can be called. this return only used for inform caller.
    so caller can inspect who gave Data more and torture him much as they like.
*/

/* what is State
  State is variable we hold at decode_t.priv.State
  its used for know what processing part we are in when data comes
  numbers of State looks very random because performance reasons
*/

/* explaining State 0 1 2 3 4 5
  State 0 2 4 is used when decode is happening with not cutted data
  if data gets cut while reading State 0 new State will be State + 1 which is 1. etc
  so State 1 3 5 are all means cutted while processing
  State 1 3 5 must be very rare or imposible to get without slowloris

  if DecodeError_NotFriendlyData_e is available
  State 1 3 5 will return ~DecodeError_NotFriendlyData_e instead of ResultType_NotEnoughData_e
*/
/* explaining State 6
  it just wants '\n'
*/

#include _WITCH_PATH(MEM/MEM.h)

#define _P(p0) CONCAT3(ETC_HTTP_set_prefix, _, p0)
#define _PP(p0) CONCAT4(_, ETC_HTTP_set_prefix, _, p0)

#if ETC_HTTP_set_LineLengthLimit < 0x100
  typedef uint8_t _P(LineOffset_t);
#elif ETC_HTTP_set_LineLengthLimit < 0x10000
  typedef uint16_t _P(LineOffset_t);
#endif

enum{
  _P(DecodeError_Done_e),
  _P(DecodeError_BadData_e),
  _P(DecodeError_LineLengthLimit_e)
  #if ETC_HTTP_set_NotFriendlyData == 1
    ,_P(DecodeError_NotFriendlyData_e)
  #endif
};
enum{
  _P(ResultType_NotEnoughData_e),
  _P(ResultType_head_e),
  _P(ResultType_header_e)
};

#if ETC_HTTP_set_PadStruct == 0
  #pragma pack(push, 1)
#endif
typedef struct{
  _P(LineOffset_t) v[3];
  _P(LineOffset_t) s[3];
}_PP(result_head_t);
typedef struct{
  _P(LineOffset_t) v[2];
  _P(LineOffset_t) s[2];
}_PP(result_header_t);

typedef struct{
  uint8_t *v[3];
  _P(LineOffset_t) s[3];
}_P(result_head_t);
typedef struct{
  uint8_t *v[2];
  _P(LineOffset_t) s[2];
}_P(result_header_t);

typedef union{
  _P(result_head_t) head;
  _P(result_header_t) header;
}_P(result_t);

typedef struct{
  uint8_t Buffer[ETC_HTTP_set_LineLengthLimit];

  struct{
    uint8_t State;
    union{
      struct{
        uint8_t BufferOffset;
        _PP(result_head_t) result_head;
      }head;
      struct{
        uint8_t BufferOffset;
        _PP(result_header_t) result_header;
      }header;
    }StateData;
  }priv;
}_P(decode_t);
#if ETC_HTTP_set_PadStruct == 0
  #pragma pack(pop)
#endif

void
_PP(decode_init_head)
(
  _P(decode_t) *decode
){
  decode->priv.State = 0x00;
  decode->priv.StateData.head.BufferOffset = 0;
}
void
_PP(decode_init_header)
(
  _P(decode_t) *decode
){
  decode->priv.State = 0x07;
  decode->priv.StateData.header.BufferOffset = 0;
}
void
_PP(decode_init_Done)
(
  _P(decode_t) *decode
){
  decode->priv.State = 0x0c;
}

void
_P(decode_init)
(
  _P(decode_t) *decode
){
  _PP(decode_init_head)(decode);
}

sint32_t
_PP(decode_CopyToBuffer_02_04)
(
  _P(decode_t) *decode,
  void *Data,
  uint8_t StateRS1
){
  uint8_t LoopTo = StateRS1;
  uint8_t LoopFrom = decode->priv.StateData.head.BufferOffset;
  for(; LoopFrom < LoopTo; ++LoopFrom){
    uintptr_t Start = decode->priv.StateData.head.result_head.v[LoopFrom];
    uintptr_t Size = decode->priv.StateData.head.result_head.s[LoopFrom];
    uintptr_t CopyTo;
    if(LoopFrom == 0){ /* i hate this */
      CopyTo = 0;
    }
    else{
      CopyTo =
        decode->priv.StateData.head.result_head.v[LoopFrom - 1] +
        decode->priv.StateData.head.result_head.s[LoopFrom - 1];
    }
    if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
      return ~_P(DecodeError_LineLengthLimit_e);
    }
    MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
    decode->priv.StateData.head.result_head.v[LoopFrom] = CopyTo;
    decode->priv.StateData.head.result_head.s[LoopFrom] = Size;
  }
  decode->priv.StateData.head.BufferOffset = LoopTo;
  return 0;
}

sint32_t
_PP(decode_CopyToBuffer_09)
(
  _P(decode_t) *decode,
  void *Data,
  uint8_t StateRS1
){
  uint8_t LoopTo = StateRS1;
  uint8_t LoopFrom = decode->priv.StateData.header.BufferOffset;
  for(; LoopFrom < LoopTo; ++LoopFrom){
    uintptr_t Start = decode->priv.StateData.header.result_header.v[LoopFrom];
    uintptr_t Size = decode->priv.StateData.header.result_header.s[LoopFrom];
    uintptr_t CopyTo;
    if(LoopFrom == 0){ /* i hate this */
      CopyTo = 0;
    }
    else{
      CopyTo =
        decode->priv.StateData.header.result_header.v[LoopFrom - 1] +
        decode->priv.StateData.header.result_header.s[LoopFrom - 1];
    }
    if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
      return ~_P(DecodeError_LineLengthLimit_e);
    }
    MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
    decode->priv.StateData.header.result_header.v[LoopFrom] = CopyTo;
    decode->priv.StateData.header.result_header.s[LoopFrom] = Size;
  }
  decode->priv.StateData.header.BufferOffset = LoopTo;
  return 0;
}

/*
  its undefined behaviour if
  DataSize equal or bigger than 2^sizeof(LineOffset_t)
  for negative return values ~DecodeError_*_e
  other returns values are ResultType_*_e
*/
sint32_t
_P(decode)
(
  _P(decode_t) *decode,
  void *Data,
  uintptr_t DataSize,
  uintptr_t *DataIndex,
  _P(result_t) *result
){
  label_SwitchState:
  switch(decode->priv.State){
    case 0x00:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      uintptr_t From = *DataIndex;
      uintptr_t To = DataSize - From;
      uintptr_t findchru = MEM_findchru(&((uint8_t *)Data)[From], To, ' ');
      (*DataIndex) += findchru;
      if(findchru == To){
        if(findchru > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[From], &decode->Buffer[0], findchru);
        decode->priv.StateData.head.BufferOffset = 1;
        decode->priv.StateData.head.result_head.v[0] = 0;
        decode->priv.StateData.head.result_head.s[0] = findchru;
        decode->priv.State = 1;
        return _P(ResultType_NotEnoughData_e);
      }
      else{
        (*DataIndex)++;
        decode->priv.StateData.head.result_head.v[0] = From;
        decode->priv.StateData.head.result_head.s[0] = findchru;
        decode->priv.State = 2;
        goto label_SwitchState;
      }
    }
    case 0x01:{
      uintptr_t Start = *DataIndex;
      uintptr_t CopyTo = decode->priv.StateData.head.result_head.s[0];
      while(*DataIndex < DataSize){
        if(((uint8_t *)Data)[*DataIndex] != ' '){
          ++(*DataIndex);
          continue;
        }
        uintptr_t Size = *DataIndex - Start;
        ++(*DataIndex);
        decode->priv.StateData.head.result_head.s[0] += Size;
        if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
        decode->priv.State = 2;
        goto label_SwitchState;
      }
      uintptr_t Size = *DataIndex - Start;
      decode->priv.StateData.head.result_head.s[0] += Size;
      if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
        return ~_P(DecodeError_LineLengthLimit_e);
      }
      MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
      return _P(ResultType_NotEnoughData_e);
    }
    case 0x02:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      uintptr_t From = *DataIndex;
      uintptr_t To = DataSize - From;
      uintptr_t findchru = MEM_findchru(&((uint8_t *)Data)[From], To, ' ');
      (*DataIndex) += findchru;
      if(findchru == To){
        {
          sint32_t r = _PP(decode_CopyToBuffer_02_04)(decode, Data, 1);
          if(r){
            return r;
          }
        }
        uintptr_t CopyTo = decode->priv.StateData.head.result_head.s[0];
        if(CopyTo + findchru > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[From], &decode->Buffer[CopyTo], findchru);
        decode->priv.StateData.head.BufferOffset = 2;
        decode->priv.StateData.head.result_head.v[1] = CopyTo;
        decode->priv.StateData.head.result_head.s[1] = findchru;
        decode->priv.State = 3;
        return _P(ResultType_NotEnoughData_e);
      }
      else{
        (*DataIndex)++;
        decode->priv.StateData.head.result_head.v[1] = From;
        decode->priv.StateData.head.result_head.s[1] = findchru;
        decode->priv.State = 4;
        goto label_SwitchState;
      }
    }
    case 0x03:{
      uintptr_t Start = *DataIndex;
      uintptr_t CopyTo =
        decode->priv.StateData.head.result_head.v[1] +
        decode->priv.StateData.head.result_head.s[1];
      while(*DataIndex < DataSize){
        if(((uint8_t *)Data)[*DataIndex] != ' '){
          ++(*DataIndex);
          continue;
        }
        uintptr_t Size = *DataIndex - Start;
        ++(*DataIndex);
        decode->priv.StateData.head.result_head.s[1] += Size;
        if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
        decode->priv.State = 4;
        goto label_SwitchState;
      }
      uintptr_t Size = *DataIndex - Start;
      decode->priv.StateData.head.result_head.s[1] += Size;
      if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
        return ~_P(DecodeError_LineLengthLimit_e);
      }
      MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
      return _P(ResultType_NotEnoughData_e);
    }
    case 0x04:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      uintptr_t From = *DataIndex;
      uintptr_t To = DataSize - From;
      uintptr_t findchru = MEM_findchru(&((uint8_t *)Data)[From], To, '\r');
      (*DataIndex) += findchru;
      if(findchru == To){
        {
          sint32_t r = _PP(decode_CopyToBuffer_02_04)(decode, Data, 2);
          if(r){
            return r;
          }
        }
        uintptr_t CopyTo =
          decode->priv.StateData.head.result_head.v[1] +
          decode->priv.StateData.head.result_head.s[1];
        if(CopyTo + findchru > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[From], &decode->Buffer[CopyTo], findchru);
        decode->priv.StateData.head.BufferOffset = 3;
        decode->priv.StateData.head.result_head.v[2] = CopyTo;
        decode->priv.StateData.head.result_head.s[2] = findchru;
        decode->priv.State = 5;
        return _P(ResultType_NotEnoughData_e);
      }
      else{
        (*DataIndex)++;
        decode->priv.StateData.head.result_head.v[2] = From;
        decode->priv.StateData.head.result_head.s[2] = findchru;
        decode->priv.State = 6;
        goto label_SwitchState;
      }
    }
    case 0x05:{
      uintptr_t Start = *DataIndex;
      uintptr_t CopyTo =
        decode->priv.StateData.head.result_head.v[2] +
        decode->priv.StateData.head.result_head.s[2];
      while(*DataIndex < DataSize){
        if(((uint8_t *)Data)[*DataIndex] != '\r'){
          ++(*DataIndex);
          continue;
        }
        uintptr_t Size = *DataIndex - Start;
        ++(*DataIndex);
        decode->priv.StateData.head.result_head.s[2] += Size;
        if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
        decode->priv.State = 6;
        goto label_SwitchState;
      }
      uintptr_t Size = *DataIndex - Start;
      decode->priv.StateData.head.result_head.s[2] += Size;
      if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
        return ~_P(DecodeError_LineLengthLimit_e);
      }
      MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
      return _P(ResultType_NotEnoughData_e);
    }
    case 0x06:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      if(((uint8_t *)Data)[(*DataIndex)++] != '\n'){
        return ~_P(DecodeError_BadData_e);
      }
      for(uint8_t i = 0; i < decode->priv.StateData.head.BufferOffset; i++){
        result->head.v[i] = &decode->Buffer[decode->priv.StateData.head.result_head.v[i]];
        result->head.s[i] = decode->priv.StateData.head.result_head.s[i];
      }
      for(uint8_t i = decode->priv.StateData.head.BufferOffset; i < 3; i++){
        result->head.v[i] = &((uint8_t *)Data)[decode->priv.StateData.head.result_head.v[i]];
        result->head.s[i] = decode->priv.StateData.head.result_head.s[i];
      }
      _PP(decode_init_header)(decode);
      return _P(ResultType_head_e);
    }
    case 0x07:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      if(((uint8_t *)Data)[*DataIndex] == '\r'){
        ++(*DataIndex);
        _PP(decode_init_Done)(decode);
        goto label_SwitchState;
      }
      uintptr_t From = *DataIndex;
      uintptr_t To = DataSize - From;
      uintptr_t findchru = MEM_findchru(&((uint8_t *)Data)[From], To, ':');
      (*DataIndex) += findchru + 1;
      if(findchru == To){
        if(findchru > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[From], &decode->Buffer[0], findchru);
        decode->priv.StateData.header.BufferOffset = 1;
        decode->priv.StateData.header.result_header.v[0] = 0;
        decode->priv.StateData.header.result_header.s[0] = findchru;
        decode->priv.State = 0x08;
        return _P(ResultType_NotEnoughData_e);
      }
      else{
        decode->priv.StateData.header.result_header.v[0] = From;
        decode->priv.StateData.header.result_header.s[0] = findchru;
        decode->priv.State = 0x09;
        goto label_SwitchState;
      }
    }
    case 0x08:{
      uintptr_t Start = *DataIndex;
      uintptr_t CopyTo = decode->priv.StateData.header.result_header.s[0];
      while(*DataIndex < DataSize){
        if(((uint8_t *)Data)[*DataIndex] != ':'){
          ++(*DataIndex);
          continue;
        }
        uintptr_t Size = *DataIndex - Start;
        ++(*DataIndex);
        decode->priv.StateData.header.result_header.s[0] += Size;
        if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
        decode->priv.State = 9;
        goto label_SwitchState;
      }
      uintptr_t Size = *DataIndex - Start;
      decode->priv.StateData.header.result_header.s[0] += Size;
      if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
        return ~_P(DecodeError_LineLengthLimit_e);
      }
      MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
      return _P(ResultType_NotEnoughData_e);
    }
    case 0x09:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      if(((uint8_t *)Data)[*DataIndex] == ' '){
        ++(*DataIndex);
      }
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      uintptr_t From = *DataIndex;
      uintptr_t To = DataSize - From;
      uintptr_t findchru = MEM_findchru(&((uint8_t *)Data)[From], To, '\r');
      (*DataIndex) += findchru + 1;
      if(findchru == To){
        {
          sint32_t r = _PP(decode_CopyToBuffer_09)(decode, Data, 1);
          if(r){
            return r;
          }
        }
        uintptr_t CopyTo = decode->priv.StateData.header.result_header.s[0];
        if(CopyTo + findchru > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[From], &decode->Buffer[CopyTo], findchru);
        decode->priv.StateData.header.BufferOffset = 2;
        decode->priv.StateData.header.result_header.v[1] = CopyTo;
        decode->priv.StateData.header.result_header.s[1] = findchru;
        decode->priv.State = 0x0a;
        return _P(ResultType_NotEnoughData_e);
      }
      else{
        decode->priv.StateData.header.result_header.v[1] = From;
        decode->priv.StateData.header.result_header.s[1] = findchru;
        decode->priv.State = 0x0b;
        goto label_SwitchState;
      }
    }
    case 0x0a:{
      uintptr_t Start = *DataIndex;
      uintptr_t CopyTo =
        decode->priv.StateData.header.result_header.v[1] +
        decode->priv.StateData.header.result_header.s[1];
      while(*DataIndex < DataSize){
        if(((uint8_t *)Data)[*DataIndex] != '\r'){
          ++(*DataIndex);
          continue;
        }
        uintptr_t Size = *DataIndex - Start;
        ++(*DataIndex);
        decode->priv.StateData.header.result_header.s[1] += Size;
        if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
          return ~_P(DecodeError_LineLengthLimit_e);
        }
        MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
        decode->priv.State = 0x0b;
        goto label_SwitchState;
      }
      uintptr_t Size = *DataIndex - Start;
      decode->priv.StateData.header.result_header.s[1] += Size;
      if(CopyTo + Size > ETC_HTTP_set_LineLengthLimit){
        return ~_P(DecodeError_LineLengthLimit_e);
      }
      MEM_copy(&((uint8_t *)Data)[Start], &decode->Buffer[CopyTo], Size);
      return _P(ResultType_NotEnoughData_e);
    }
    case 0x0b:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      if(((uint8_t *)Data)[(*DataIndex)++] != '\n'){
        return ~_P(DecodeError_BadData_e);
      }
      for(uint8_t i = 0; i < decode->priv.StateData.header.BufferOffset; i++){
        result->header.v[i] = &decode->Buffer[decode->priv.StateData.header.result_header.v[i]];
        result->header.s[i] = decode->priv.StateData.header.result_header.s[i];
      }
      for(uint8_t i = decode->priv.StateData.header.BufferOffset; i < 2; i++){
        result->header.v[i] = &((uint8_t *)Data)[decode->priv.StateData.header.result_header.v[i]];
        result->header.s[i] = decode->priv.StateData.header.result_header.s[i];
      }
      _PP(decode_init_header)(decode);
      return _P(ResultType_header_e);
    }
    case 0x0c:{
      if(*DataIndex == DataSize){
        return _P(ResultType_NotEnoughData_e);
      }
      if(((uint8_t *)Data)[(*DataIndex)++] != '\n'){
        return ~_P(DecodeError_BadData_e);
      }
      return ~_P(DecodeError_Done_e);
    }
  }
}

#undef _P
#undef _PP
