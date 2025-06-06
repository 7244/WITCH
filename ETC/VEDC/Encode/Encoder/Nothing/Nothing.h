ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_Nothing_Open(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_t *EncoderSetting,
  void *EncoderUnique
){
  return ETC_VEDC_Encode_Error_Success;
}

void *
_ETC_VEDC_Encode_Encoder_Nothing_GetUnique(
  void **EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType
){
  return NULL;
}

void
_ETC_VEDC_Encode_Encoder_Nothing_Close(
  void **EncoderData
){
  return;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_Nothing_SetFrameSize(
  void **EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
){
  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_Nothing_SetRateControl(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_Nothing_SetInputFrameRate(
  void **EncoderData,
  f32_t fps
){
  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_Nothing_IsWriteType(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType
){
  return 1;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_Nothing_Write(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void *WriteData,
  uint8_t Flags
){
  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_Nothing_IsReadable(
  void **EncoderData
){
  return 0;
}

sint32_t
_ETC_VEDC_Encode_Encoder_Nothing_Read(
  void **EncoderData,
  ETC_VEDC_Encode_PacketInfo *PacketInfo,
  void **Data
){
  return 0;
}
