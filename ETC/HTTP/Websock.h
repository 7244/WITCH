#ifndef ETC_HTTP_Websock_set_prefix
  #error ifndef ETC_HTTP_Websock_set_prefix
#endif

#define ETC_HTTP_Websock_p(p0) CONCAT3(ETC_HTTP_Websock_set_prefix, _, p0)

const uint32_t ETC_HTTP_Websock_p(opcode_binary) = 0x2;

const uint8_t ETC_HTTP_Websock_p(MaskBit) = (1u << 7u);

const uint8_t ETC_HTTP_Websock_p(Flag_Fin) = (1u << 7u);

const uint8_t ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_LARGE) = 0x7e;
const uint8_t ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_HUGE) = 0x7f;

const size_t ETC_HTTP_Websock_p(MAX_PAYLOAD_LENGTH_SMALL) = 0x7d;
const size_t ETC_HTTP_Websock_p(MAX_PAYLOAD_LENGTH_LARGE) = 65535;

typedef struct{
  uint8_t b[1 + 1 + 8 + 4];
}ETC_HTTP_Websock_p(WebsockHead_t);

uint8_t
ETC_HTTP_Websock_p(MakeWebsockHead)
(
  ETC_HTTP_Websock_p(WebsockHead_t) *Head,
  uintptr_t DataSize
){
  uint32_t opcode = ETC_HTTP_Websock_p(opcode_binary);

  uint8_t pos = 0;
  Head->b[pos++] = ETC_HTTP_Websock_p(Flag_Fin) | opcode;

  if (DataSize <= ETC_HTTP_Websock_p(MAX_PAYLOAD_LENGTH_SMALL))
  {
    Head->b[pos++] = (unsigned int)DataSize;
  }
  else if (DataSize <= ETC_HTTP_Websock_p(MAX_PAYLOAD_LENGTH_LARGE))
  {
    Head->b[pos++] = ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_LARGE);
    Head->b[pos++] = (DataSize >> 8) & 0xff;
    Head->b[pos++] = DataSize & 0xff;
  }
  else
  {
    Head->b[pos++] = ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_HUGE);
    const uint64_t len = DataSize;
    for (int i = sizeof(uint64_t) - 1; i >= 0; i--){
      Head->b[pos++] = ((len >> i * 8) & 0xff);
    }
  }

  Head->b[1] |= ETC_HTTP_Websock_p(MaskBit);
  Head->b[pos++] = 0;
  Head->b[pos++] = 0;
  Head->b[pos++] = 0;
  Head->b[pos++] = 0;

  return pos;
}

typedef enum{
  ETC_HTTP_Websock_p(Error_Done) = -1
}ETC_HTTP_Websock_p(Error_t);

typedef struct{
  uint8_t State; // 0
  uint8_t Flag;
  bool MaskBit;
  uint64_t PayloadSize;
  uint8_t ExtraSize;
}ETC_HTTP_Websock_p(Parser_t);

static
void
ETC_HTTP_Websock_p(Parser_init)
(
  ETC_HTTP_Websock_p(Parser_t) *Parser
){
  Parser->State = 0;
}

static
sintptr_t
ETC_HTTP_Websock_p(Parse)
(
  ETC_HTTP_Websock_p(Parser_t) *Parser,
  const uint8_t *Data,
  uintptr_t DataSize,
  uintptr_t *DataIndex,
  ETC_HTTP_Websock_p(Error_t) *Error
){
  while(*DataIndex != DataSize){
    if(Parser->State == 0){
      Parser->Flag = Data[(*DataIndex)++];
      Parser->State = 1;
      continue;
    }
    else if(Parser->State == 1){
      uint8_t b = Data[(*DataIndex)++];
      if(b & ETC_HTTP_Websock_p(MaskBit)){
        PR_abort(); /* how to deal with MaskBit? */
        Parser->MaskBit = true;
        b ^= ETC_HTTP_Websock_p(MaskBit);
      }

      if(b == ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_LARGE)){
        Parser->PayloadSize = 0;
        Parser->ExtraSize = 2;
        Parser->State = 2;
      }
      else if(b == ETC_HTTP_Websock_p(PAYLOAD_LENGTH_MAGIC_HUGE)){
        Parser->ExtraSize = 8;
        Parser->State = 2;
      }
      else{
        Parser->PayloadSize = b;
        Parser->State = 3;
      }
      continue;
    }
    else if(Parser->State == 2){
      Parser->PayloadSize |= (uint64_t)Data[(*DataIndex)++] << --Parser->ExtraSize * 8;
      if(Parser->ExtraSize == 0){
        Parser->State = 3;
      }
      continue;
    }
    else if(Parser->State == 3){
      uint64_t LeftSize = DataSize - *DataIndex;
      if(LeftSize >= Parser->PayloadSize){
        LeftSize = Parser->PayloadSize;
        *Error = ETC_HTTP_Websock_p(Error_Done);
      }
      else{
        Parser->PayloadSize -= LeftSize;
        *Error = 0;
      }
      *DataIndex += LeftSize;
      return LeftSize;
    }
  }
  *Error = 0;
  return 0;
}

#undef ETC_HTTP_Websock_p

#undef ETC_HTTP_Websock_set_prefix