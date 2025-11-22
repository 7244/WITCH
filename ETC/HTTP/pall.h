#ifndef ETC_HTTP_pall_set_prefix
  #error ifndef ETC_HTTP_pall_set_prefix
#endif

#define ETC_HTTP_pall_p(p0) CONCAT3(ETC_HTTP_pall_set_prefix, _, p0)

/* http prefix */
#define ETC_HTTP_pall_hp(p) CONCAT3(ETC_HTTP_pall_set_prefix,_hp,p)

#define ETC_HTTP_set_prefix ETC_HTTP_pall_hp()
#include <WITCH/ETC/HTTP/HTTP.h>

/* http chunked prefix */
#define ETC_HTTP_pall_hcp(p) CONCAT3(ETC_HTTP_pall_set_prefix,_hcp,p)

#define ETC_HTTP_chunked_set_prefix ETC_HTTP_pall_hcp()
#include <WITCH/ETC/HTTP/chunked.h>

#include <WITCH/STR/psu.h>

typedef enum{
  ETC_HTTP_pall_p(ParseType_Done),
  ETC_HTTP_pall_p(ParseType_NotEnoughData),
  ETC_HTTP_pall_p(ParseType_Error),
  ETC_HTTP_pall_p(ParseType_HTTPHead),
  ETC_HTTP_pall_p(ParseType_HTTPHeader),
  ETC_HTTP_pall_p(ParseType_HTTPDone),
  ETC_HTTP_pall_p(ParseType_Payload)
}ETC_HTTP_pall_p(ParseType);

typedef struct{
  uint8_t State;
  ETC_HTTP_pall_hp(_decode_t) h;
  uint8_t TransferType;
  union{
    uint64_t ContentLength;
    ETC_HTTP_pall_hcp(_Parser_t) hc;
  }TransferData;
}ETC_HTTP_pall_p(t);

static
void
ETC_HTTP_pall_p(Open)
(
  ETC_HTTP_pall_p(t) *Parser
){
  Parser->State = 0;
  ETC_HTTP_pall_hp(_decode_init)(&Parser->h);
  Parser->TransferType = 0;
}
static
void
ETC_HTTP_pall_p(Close)
(
  ETC_HTTP_pall_p(t) *Parser
){
}
static
void
ETC_HTTP_pall_p(Reset)
(
  ETC_HTTP_pall_p(t) *Parser
){
  ETC_HTTP_pall_p(Open)(Parser);
}

typedef union{
  ETC_HTTP_pall_hp(_result_t) HTTP;
  struct{
    const void *Data;
    uintptr_t Size;
  }Payload;
}ETC_HTTP_pall_p(ParsedData);

static
ETC_HTTP_pall_p(ParseType)
ETC_HTTP_pall_p(Parse)
(
  ETC_HTTP_pall_p(t) *Parser,
  const uint8_t *Data,
  uintptr_t DataSize,
  uintptr_t *DataIndex,
  ETC_HTTP_pall_p(ParsedData) *ParsedData
){
  gt_State:;
  if(Parser->State == 0){
    sint32_t state = ETC_HTTP_pall_hp(_decode)(
      &Parser->h,
      (void*)Data,
      DataSize,
      DataIndex,
      &ParsedData->HTTP);
    if(state < 0){
      if(state == ~ETC_HTTP_pall_hp(_DecodeError_Done_e)){
        if(Parser->TransferType == 0){
          Parser->TransferData.ContentLength = 0;
          Parser->State = 1;
        }
        else{
          Parser->State = Parser->TransferType;
        }
        return ETC_HTTP_pall_p(ParseType_HTTPDone);
      }
      else{
        return ETC_HTTP_pall_p(ParseType_Error);
      }
    }
    else{
      switch(state){
        case ETC_HTTP_pall_hp(_ResultType_NotEnoughData_e):{
          return ETC_HTTP_pall_p(ParseType_NotEnoughData);
        }
        case HTTP_ResultType_head_e:{
          return ETC_HTTP_pall_p(ParseType_HTTPHead);
        }
        case ETC_HTTP_pall_hp(_ResultType_header_e):{
          const char *cl_str = "Content-Length";
          const char *te_str = "Transfer-Encoding";
          if(
            MEM_cstreu(cl_str) == ParsedData->HTTP.header.s[0] &&
            STR_ncmp(cl_str, ParsedData->HTTP.header.v[0], ParsedData->HTTP.header.s[0]) == 0
          ){
            if(Parser->TransferType != 0){
              return ETC_HTTP_pall_p(ParseType_Error);
            }
            Parser->TransferData.ContentLength = STR_psu64(ParsedData->HTTP.header.v[1], ParsedData->HTTP.header.s[1]);
            Parser->TransferType = 1;
            goto gt_State;
          }
          else if(
            MEM_cstreu(te_str) == ParsedData->HTTP.header.s[0] &&
            STR_ncmp(te_str, ParsedData->HTTP.header.v[0], ParsedData->HTTP.header.s[0]) == 0
          ){
            if(Parser->TransferType != 0){
              return ETC_HTTP_pall_p(ParseType_Error);
            }
            const char *c_str = "chunked";
            if(
              MEM_cstreu(c_str) == ParsedData->HTTP.header.s[1] &&
              STR_ncmp(c_str, ParsedData->HTTP.header.v[1], ParsedData->HTTP.header.s[1]) == 0
            ){
              Parser->TransferType = 2;
              ETC_HTTP_pall_hcp(_Parser_init)(&Parser->TransferData.hc);
            }
            else{
              return ETC_HTTP_pall_p(ParseType_Error);
            }
            goto gt_State;
          }
          else{
            return ETC_HTTP_pall_p(ParseType_HTTPHeader);
          }
        }
        default:{
          __unreachable_or(
            return 0;
          );
        }
      }
    }
  }
  else if(Parser->State == 1){
    uint64_t LeftSize = DataSize - *DataIndex;
    if(LeftSize > Parser->TransferData.ContentLength){
      LeftSize = Parser->TransferData.ContentLength;
    }
    else if(LeftSize == 0){
      if(Parser->TransferData.ContentLength != 0){
        return ETC_HTTP_pall_p(ParseType_NotEnoughData);
      }
      return ETC_HTTP_pall_p(ParseType_Done);
    }
    Parser->TransferData.ContentLength -= LeftSize;
    ParsedData->Payload.Data = &Data[*DataIndex];
    ParsedData->Payload.Size = LeftSize;
    *DataIndex += LeftSize;
    return ETC_HTTP_pall_p(ParseType_Payload);
  }
  else if(Parser->State == 2){
    if(*DataIndex == DataSize){
      return ETC_HTTP_pall_p(ParseType_NotEnoughData);
    }
    sintptr_t r = ETC_HTTP_pall_hcp(_Parse)(&Parser->TransferData.hc, Data, DataSize, DataIndex);
    if(r < 0){
      if(r != ETC_HTTP_pall_hcp(_Error_EndOfChunks)){
        return ETC_HTTP_pall_p(ParseType_Error);
      }
      return ETC_HTTP_pall_p(ParseType_Done);
    }
    ParsedData->Payload.Data = &Data[*DataIndex - r];
    ParsedData->Payload.Size = r;
    return ETC_HTTP_pall_p(ParseType_Payload);
  }
}

#undef ETC_HTTP_pall_hp

#undef ETC_HTTP_pall_p

#undef ETC_HTTP_pall_set_prefix
