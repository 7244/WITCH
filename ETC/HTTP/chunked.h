#ifndef ETC_HTTP_chunked_set_prefix
  #error ifndef ETC_HTTP_chunked_set_prefix
#endif

#include _WITCH_PATH(STR/psh.h)

#define ETC_HTTP_chunked_p(p0) CONCAT3(ETC_HTTP_chunked_set_prefix, _, p0)

typedef enum{
  ETC_HTTP_chunked_p(Error_EndOfChunks) = -1,
  ETC_HTTP_chunked_p(Error_Parse) = -2
}ETC_HTTP_chunked_p(Error_t);

typedef struct{
  uint64_t Length;
  uint8_t StringHead[18];
  uint8_t StringHeadSize;
  bool ShouldEnd;
}ETC_HTTP_chunked_p(Parser_t);

static
void
ETC_HTTP_chunked_p(Parser_init)
(
  ETC_HTTP_chunked_p(Parser_t) *Parser
){
  Parser->Length = (uint64_t)-1;
  Parser->StringHeadSize = 0;
}

static
sintptr_t
ETC_HTTP_chunked_p(Parse)
(
  ETC_HTTP_chunked_p(Parser_t) *Parser,
  const uint8_t *Data,
  uintptr_t DataSize,
  uintptr_t *DataIndex
){
  gt_begin:;
  if(Parser->Length == (uint64_t)-1){
    while(*DataIndex < DataSize){
      uint8_t c = Data[(*DataIndex)++];
      Parser->StringHead[Parser->StringHeadSize++] = c;
      if(c == '\n'){
        if(Parser->StringHeadSize < 3){
          return ETC_HTTP_chunked_p(Error_Parse);
        }
        if(Parser->StringHead[Parser->StringHeadSize - 2] != '\r'){
          return ETC_HTTP_chunked_p(Error_Parse);
        }
        Parser->Length = STR_psh64_digit(Parser->StringHead, Parser->StringHeadSize - 2);
        Parser->ShouldEnd = Parser->Length == 0;
        Parser->StringHeadSize = 0;
        goto gt_begin;
      }
    }
    return 0;
  }
  else if(Parser->Length > 0){
    uintptr_t LeftDataSize = DataSize - *DataIndex;
    uintptr_t ToGo = Parser->Length < LeftDataSize ? Parser->Length : LeftDataSize;
    *DataIndex += ToGo;
    Parser->Length -= ToGo;
    return ToGo;
  }
  else if(Parser->Length == 0){
    while(*DataIndex < DataSize){
      const uint8_t comboarr[2] = {'\r', '\n'};

      uint8_t c = Data[(*DataIndex)++];

      if(comboarr[Parser->StringHeadSize] != c){
        return ETC_HTTP_chunked_p(Error_Parse);
      }
      if(++Parser->StringHeadSize == 2){
        if(Parser->ShouldEnd == true){
          return ETC_HTTP_chunked_p(Error_EndOfChunks);
        }
        else{
          ETC_HTTP_chunked_p(Parser_init)(Parser);
          return 0;
        }
      }
    }
  }
}

#undef ETC_HTTP_chunked_p

#undef ETC_HTTP_chunked_set_prefix