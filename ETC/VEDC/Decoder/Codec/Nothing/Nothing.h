ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_Nothing_Open(
  void **DecoderData,
  void *DecoderUnique
){
  return ETC_VEDC_Decoder_Error_OK;
}

void *_ETC_VEDC_Decoder_Codec_Nothing_GetUnique(
  void **DecoderData,
  ETC_VEDC_Decoder_UniqueType UniqueType
){
  return NULL;
}

void _ETC_VEDC_Decoder_Codec_Nothing_Close(
  void **DecoderData
){
}

sintptr_t _ETC_VEDC_Decoder_Codec_Nothing_Write(
  void **DecoderData,
  uint8_t *Data,
  uintptr_t Size
){
  return Size;
}

bool _ETC_VEDC_Decoder_Codec_Nothing_IsReadable(
  void **DecoderData
){
  return 0;
}

bool _ETC_VEDC_Decoder_Codec_Nothing_IsReadType(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType
){
  return 0;
}

void _ETC_VEDC_Decoder_Codec_Nothing_GetReadImageProperties(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t *ImageProperties
){
}

ETC_VEDC_Decoder_Error
_ETC_VEDC_Decoder_Codec_Nothing_Read(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  return ETC_VEDC_Decoder_Error_NotReadable;
}

void _ETC_VEDC_Decoder_Codec_Nothing_ReadClear(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
}
