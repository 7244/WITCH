#include <wels/codec_api.h>

typedef struct{
  ISVCDecoder *de;
  struct{
    SBufferInfo BufferInfo;
    unsigned char *Data[4];
  }wrd;
}_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t;

ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_OpenH264_Open(
  void **DecoderData,
  void *DecoderUnique
){
  ETC_VEDC_Decoder_Error Error = ETC_VEDC_Decoder_Error_Unknown;

  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder =
    (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)A_resize(0, sizeof(_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t));
  *DecoderData = (void *)Decoder;

  long ir; /* internal return */

  ir = WelsCreateDecoder(&Decoder->de);
  if(ir != 0){
    Error = ETC_VEDC_Decoder_Error_DecoderAllocateFail;
    goto gt_err0;
  }

  {
    SDecodingParam InternalSetting = {0};
    InternalSetting.eEcActiveIdc = ERROR_CON_SLICE_MV_COPY_CROSS_IDR;
    ir = Decoder->de->Initialize(&InternalSetting);
    if(ir != 0){
      Error = ETC_VEDC_Decoder_Error_DecoderError;
      goto gt_err1;
    }
  }

  return ETC_VEDC_Decoder_Error_OK;

  gt_err1:
  WelsDestroyDecoder(Decoder->de);
  gt_err0:
  A_resize(Decoder, 0);
  return Error;
}

void *_ETC_VEDC_Decoder_Codec_OpenH264_GetUnique(
  void **DecoderData,
  ETC_VEDC_Decoder_UniqueType UniqueType
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  return NULL;
}

void _ETC_VEDC_Decoder_Codec_OpenH264_Close(
  void **DecoderData
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  Decoder->de->Uninitialize();
  WelsDestroyDecoder(Decoder->de);
  A_resize(Decoder, 0);
}

sint32_t _ETC_VEDC_Decoder_Codec_OpenH264_Write(
  void **DecoderData,
  uint8_t *Data,
  uint32_t Size
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  SParserBsInfo ParserInfo;
  DECODING_STATE r = Decoder->de->DecodeFrame2(
    Data,
    Size,
    Decoder->wrd.Data,
    &Decoder->wrd.BufferInfo);

  return 0;
}

bool _ETC_VEDC_Decoder_Codec_OpenH264_IsReadable(
  void **DecoderData
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  /* Read is able to call FlushFrame which gives frame no matter what */
  return 1;
}

bool _ETC_VEDC_Decoder_Codec_OpenH264_IsReadType(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_Frame:{
      return 1;
    }
    default:{
      return 0;
    }
  }
}

void _ETC_VEDC_Decoder_Codec_OpenH264_GetReadImageProperties(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t *ImageProperties
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_Frame:{
      ImageProperties->PixelFormat = ETC_PIXF_YUV420p;

      ImageProperties->Stride[0] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[0];
      ImageProperties->Stride[1] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[1];
      ImageProperties->Stride[2] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[1];

      ImageProperties->SizeX = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iWidth;
      ImageProperties->SizeY = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iHeight;
      return;
    }
    default:{
      return;
    }
  }
}

ETC_VEDC_Decoder_Error
_ETC_VEDC_Decoder_Codec_OpenH264_Read(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  if(Decoder->wrd.BufferInfo.iBufferStatus == 0){
    DECODING_STATE r = Decoder->de->FlushFrame(
      Decoder->wrd.Data,
      &Decoder->wrd.BufferInfo);
    if(r >= 0x1000){
      return ETC_VEDC_Decoder_Error_NotReadable;
    }
    if(Decoder->wrd.BufferInfo.iBufferStatus == 0){
      return ETC_VEDC_Decoder_Error_NotReadable;
    }
  }

  if(Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iFormat != videoFormatI420){
    return ETC_VEDC_Decoder_Error_NotImplemented;
  }

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_Frame:{
      ETC_VEDC_Decoder_Frame_t *Frame = (ETC_VEDC_Decoder_Frame_t *)ReadData;

      Frame->Properties.PixelFormat = ETC_PIXF_YUV420p;

      Frame->Data[0] = Decoder->wrd.Data[0];
      Frame->Data[1] = Decoder->wrd.Data[1];
      Frame->Data[2] = Decoder->wrd.Data[2];

      Frame->Properties.Stride[0] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[0];
      Frame->Properties.Stride[1] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[1];
      Frame->Properties.Stride[2] = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iStride[1];

      Frame->Properties.SizeX = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iWidth;
      Frame->Properties.SizeY = Decoder->wrd.BufferInfo.UsrData.sSystemBuffer.iHeight;

      return ETC_VEDC_Decoder_Error_OK;
    }
    default:{
      return ETC_VEDC_Decoder_Error_InvalidReadType;
    }
  }
}

void _ETC_VEDC_Decoder_Codec_OpenH264_ReadClear(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_OpenH264_Decoder_t *)*DecoderData;

  Decoder->wrd.BufferInfo.iBufferStatus = 0;
}
