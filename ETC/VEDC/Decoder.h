#pragma once

/*
  ETC_VEDC_Decoder_DefineCodec_<name of codec>
*/

#include <PIXF/PIXF.h>
#include _WITCH_PATH(MEM/MEM.h)

typedef struct{
  PIXF PixelFormat;
  uint32_t Stride[4];
  uint32_t SizeX;
  uint32_t SizeY;
}ETC_VEDC_Decoder_ImageProperties_t;

typedef struct{
  ETC_VEDC_Decoder_ImageProperties_t Properties; /* out */
  void *Data[4]; /* out */
}ETC_VEDC_Decoder_Frame_t;
#ifdef __GPU_CUDA
  typedef struct{
    /* user need to know pixel format and size to open cudaArrays for write from decoder */
    cudaArray_t Array[4]; /* in, out */
  }ETC_VEDC_Decoder_CudaArrayFrame_t;
#endif

typedef enum{
  ETC_VEDC_Decoder_ReadType_Unknown,
  ETC_VEDC_Decoder_ReadType_Frame
  #ifdef __GPU_CUDA
    ,ETC_VEDC_Decoder_ReadType_CudaArrayFrame
  #endif
}ETC_VEDC_Decoder_ReadType;

typedef enum{
  ETC_VEDC_Decoder_Error_OK,
  ETC_VEDC_Decoder_Error_Unknown,
  ETC_VEDC_Decoder_Error_NotImplemented,
  ETC_VEDC_Decoder_Error_DecoderNotFound,
  ETC_VEDC_Decoder_Error_DecoderAllocateFail,
  ETC_VEDC_Decoder_Error_DecoderError,
  ETC_VEDC_Decoder_Error_CudaInit,
  ETC_VEDC_Decoder_Error_CudaNoDevice,
  ETC_VEDC_Decoder_Error_CudaDeviceGet,
  ETC_VEDC_Decoder_Error_CudaContextCreate,
  ETC_VEDC_Decoder_Error_ParserCreate,
  ETC_VEDC_Decoder_Error_NotReadable,
  ETC_VEDC_Decoder_Error_InvalidReadType
}ETC_VEDC_Decoder_Error;

typedef enum{
  #ifdef __GPU_CUDA
    ETC_VEDC_Decoder_UniqueType_CudaContext,
  #endif
    ETC_VEDC_Decoder_UniqueType_D3D11Device,
}ETC_VEDC_Decoder_UniqueType;

typedef ETC_VEDC_Decoder_Error (*_ETC_VEDC_Decoder_Open_cb)(
  void **DecoderData,
  void *DecoderUnique);
typedef void *(*_ETC_VEDC_Decoder_GetUnique_cb)(
  void **DecoderData,
  ETC_VEDC_Decoder_UniqueType UniqueType);
typedef void (*_ETC_VEDC_Decoder_Close_cb)(
  void **DecoderData);
typedef sintptr_t (*_ETC_VEDC_Decoder_Write_cb)(
  void **DecoderData,
  uint8_t *Data,
  uintptr_t Size);
typedef bool (*_ETC_VEDC_Decoder_IsReadable_cb)(
  void **DecoderData);
typedef bool (*_ETC_VEDC_Decoder_IsReadType_cb)(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType);
typedef void (*_ETC_VEDC_Decoder_GetReadImageProperties_cb)(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t *ImageProperties);
typedef ETC_VEDC_Decoder_Error (*_ETC_VEDC_Decoder_Read_cb)(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData);
typedef void (*_ETC_VEDC_Decoder_ReadClear_cb)(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData);

typedef struct{
  const char *Name;
  _ETC_VEDC_Decoder_Open_cb Open_cb;
  _ETC_VEDC_Decoder_GetUnique_cb GetUnique_cb;
  _ETC_VEDC_Decoder_Close_cb Close_cb;
  _ETC_VEDC_Decoder_Write_cb Write_cb;
  _ETC_VEDC_Decoder_IsReadable_cb IsReadable_cb;
  _ETC_VEDC_Decoder_IsReadType_cb IsReadType_cb;
  _ETC_VEDC_Decoder_GetReadImageProperties_cb GetReadImageProperties_cb;
  _ETC_VEDC_Decoder_Read_cb Read_cb;
  _ETC_VEDC_Decoder_ReadClear_cb ReadClear_cb;
}_ETC_VEDC_DecoderInfo;

#include "Decoder/Codec/Nothing/Nothing.h"
#ifdef ETC_VEDC_Decoder_DefineCodec_OpenH264
  #include "Decoder/Codec/OpenH264/OpenH264.h"
#endif
#ifdef ETC_VEDC_Decoder_DefineCodec_cuvid
  #include "Decoder/Codec/cuvid/cuvid.h"
#endif
#ifdef ETC_VEDC_Decoder_DefineCodec_va
  #include "Decoder/Codec/va/va.h"
#endif
#ifdef ETC_VEDC_Decoder_DefineCodec_amf
  //AMD
  #include "Decoder/Codec/amf/amf.h"
#endif
#ifdef ETC_VEDC_Decoder_DefineCodec_va
  #include "Decoder/Codec/va/va.h"
#endif

_ETC_VEDC_DecoderInfo _ETC_VEDC_DecoderList[] = {
  {
    .Name = "Nothing",
    .Open_cb = _ETC_VEDC_Decoder_Codec_Nothing_Open,
    .GetUnique_cb = _ETC_VEDC_Decoder_Codec_Nothing_GetUnique,
    .Close_cb = _ETC_VEDC_Decoder_Codec_Nothing_Close,
    .Write_cb = _ETC_VEDC_Decoder_Codec_Nothing_Write,
    .IsReadable_cb = _ETC_VEDC_Decoder_Codec_Nothing_IsReadable,
    .IsReadType_cb = _ETC_VEDC_Decoder_Codec_Nothing_IsReadType,
    .GetReadImageProperties_cb = _ETC_VEDC_Decoder_Codec_Nothing_GetReadImageProperties,
    .Read_cb = _ETC_VEDC_Decoder_Codec_Nothing_Read,
    .ReadClear_cb = _ETC_VEDC_Decoder_Codec_Nothing_ReadClear
  }
  #ifdef ETC_VEDC_Decoder_DefineCodec_OpenH264
    ,{
      .Name = "OpenH264",
      .Open_cb = _ETC_VEDC_Decoder_Codec_OpenH264_Open,
      .GetUnique_cb = _ETC_VEDC_Decoder_Codec_OpenH264_GetUnique,
      .Close_cb = _ETC_VEDC_Decoder_Codec_OpenH264_Close,
      .Write_cb = _ETC_VEDC_Decoder_Codec_OpenH264_Write,
      .IsReadable_cb = _ETC_VEDC_Decoder_Codec_OpenH264_IsReadable,
      .IsReadType_cb = _ETC_VEDC_Decoder_Codec_OpenH264_IsReadType,
      .GetReadImageProperties_cb = _ETC_VEDC_Decoder_Codec_OpenH264_GetReadImageProperties,
      .Read_cb = _ETC_VEDC_Decoder_Codec_OpenH264_Read,
      .ReadClear_cb = _ETC_VEDC_Decoder_Codec_OpenH264_ReadClear
    }
  #endif
  #ifdef ETC_VEDC_Decoder_DefineCodec_cuvid
    ,{
      .Name = "cuvid",
      .Open_cb = _ETC_VEDC_Decoder_Codec_cuvid_Open,
      .GetUnique_cb = _ETC_VEDC_Decoder_Codec_cuvid_GetUnique,
      .Close_cb = _ETC_VEDC_Decoder_Codec_cuvid_Close,
      .Write_cb = _ETC_VEDC_Decoder_Codec_cuvid_Write,
      .IsReadable_cb = _ETC_VEDC_Decoder_Codec_cuvid_IsReadable,
      .IsReadType_cb = _ETC_VEDC_Decoder_Codec_cuvid_IsReadType,
      .GetReadImageProperties_cb = _ETC_VEDC_Decoder_Codec_cuvid_GetReadImageProperties,
      .Read_cb = _ETC_VEDC_Decoder_Codec_cuvid_Read,
      .ReadClear_cb = _ETC_VEDC_Decoder_Codec_cuvid_ReadClear
    }
  #endif
  #ifdef ETC_VEDC_Decoder_DefineCodec_amf
  ,{
    .Name = "amf",
    .Open_cb = _ETC_VEDC_Decoder_Codec_amf_Open,
    .GetUnique_cb = _ETC_VEDC_Decoder_Codec_amf_GetUnique,
    .Close_cb = _ETC_VEDC_Decoder_Codec_amf_Close,
    .Write_cb = _ETC_VEDC_Decoder_Codec_amf_Write,
    .IsReadable_cb = _ETC_VEDC_Decoder_Codec_amf_IsReadable,
    .IsReadType_cb = _ETC_VEDC_Decoder_Codec_amf_IsReadType,
    .GetReadImageProperties_cb = _ETC_VEDC_Decoder_Codec_amf_GetReadImageProperties,
    .Read_cb = _ETC_VEDC_Decoder_Codec_amf_Read,
    .ReadClear_cb = _ETC_VEDC_Decoder_Codec_amf_ReadClear
  }
  #endif
  #ifdef ETC_VEDC_Decoder_DefineCodec_va
  ,{
    .Name = "va",
    .Open_cb = _ETC_VEDC_Decoder_Codec_va_Open,
    .GetUnique_cb = _ETC_VEDC_Decoder_Codec_va_GetUnique,
    .Close_cb = _ETC_VEDC_Decoder_Codec_va_Close,
    .Write_cb = _ETC_VEDC_Decoder_Codec_va_Write,
    .IsReadable_cb = _ETC_VEDC_Decoder_Codec_va_IsReadable,
    .IsReadType_cb = _ETC_VEDC_Decoder_Codec_va_IsReadType,
    .GetReadImageProperties_cb = _ETC_VEDC_Decoder_Codec_va_GetReadImageProperties,
    .Read_cb = _ETC_VEDC_Decoder_Codec_va_Read,
    .ReadClear_cb = _ETC_VEDC_Decoder_Codec_va_ReadClear
  }
  #endif
};

typedef struct{
  uintptr_t DecoderID;
  void *DecoderData;
}ETC_VEDC_Decoder_t;

void
ETC_VEDC_Decoder_OpenNothing(
  ETC_VEDC_Decoder_t *Decoder
){
  Decoder->DecoderID = 0;
}

ETC_VEDC_Decoder_Error
ETC_VEDC_Decoder_Open(
  ETC_VEDC_Decoder_t *Decoder,
  uintptr_t DecoderNameSize,
  const void *DecoderName,
  void *DecoderUnique
){
  uintptr_t DecoderAmount = sizeof(_ETC_VEDC_DecoderList) / sizeof(_ETC_VEDC_DecoderList[0]);
  for(uintptr_t i = 1; i < DecoderAmount; i++){
    _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[i];
    uintptr_t Size = MEM_cstreu(di->Name);
    if(MEM_ncmpn(di->Name, Size, DecoderName, DecoderNameSize) != 0){
      ETC_VEDC_Decoder_Error r = di->Open_cb(&Decoder->DecoderData, DecoderUnique);
      if(r != ETC_VEDC_Decoder_Error_OK){
        return r;
      }
      Decoder->DecoderID = i;
      return ETC_VEDC_Decoder_Error_OK;
    }
  }
  return ETC_VEDC_Decoder_Error_DecoderNotFound;
}

void *
ETC_VEDC_Decoder_GetUnique(
  ETC_VEDC_Decoder_t *Decoder,
  ETC_VEDC_Decoder_UniqueType UniqueType
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  return di->GetUnique_cb(&Decoder->DecoderData, UniqueType);
}

void
ETC_VEDC_Decoder_Close(
  ETC_VEDC_Decoder_t *Decoder
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  di->Close_cb(&Decoder->DecoderData);
}

sintptr_t
ETC_VEDC_Decoder_Write(
  ETC_VEDC_Decoder_t *Decoder,
  uint8_t *Data,
  uintptr_t Size
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  return di->Write_cb(&Decoder->DecoderData, Data, Size);
}
bool
ETC_VEDC_Decoder_IsReadable(
  ETC_VEDC_Decoder_t *Decoder
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  return di->IsReadable_cb(&Decoder->DecoderData);
}
bool
ETC_VEDC_Decoder_IsReadType(
  ETC_VEDC_Decoder_t *Decoder,
  ETC_VEDC_Decoder_ReadType ReadType
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  return di->IsReadType_cb(&Decoder->DecoderData, ReadType);
}
void
ETC_VEDC_Decoder_GetReadImageProperties(
  ETC_VEDC_Decoder_t *Decoder,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t *ImageProperties
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  di->GetReadImageProperties_cb(&Decoder->DecoderData, ReadType, ImageProperties);
}
ETC_VEDC_Decoder_Error
ETC_VEDC_Decoder_Read(
  ETC_VEDC_Decoder_t *Decoder,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  return di->Read_cb(&Decoder->DecoderData, ReadType, ReadData);
}
/* must be called after Read */
void
ETC_VEDC_Decoder_ReadClear(
  ETC_VEDC_Decoder_t *Decoder,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_DecoderInfo *di = &_ETC_VEDC_DecoderList[Decoder->DecoderID];
  di->ReadClear_cb(&Decoder->DecoderData, ReadType, ReadData);
}
