#pragma once

/*
  ETC_VEDC_Encode_DefineEncoder_<name of codec>
*/

#include _WITCH_PATH(ETC/PIXF/PIXF.h)
#include _WITCH_PATH(ETC/VCODECSTD/VCODECSTD.h)
#include _WITCH_PATH(MEM/MEM.h)

typedef struct{
  ETC_PIXF PixelFormat;
  uint32_t Stride[4];
  uint32_t SizeX;
  uint32_t SizeY;
}ETC_VEDC_Encode_ImageProperties_t;

typedef struct{
  ETC_VEDC_Encode_ImageProperties_t Properties; /* in */
  void *Data[4]; /* in */
  uint64_t TimeStamp; /* in */
}ETC_VEDC_Encode_Frame_t;

typedef enum{
  ETC_VEDC_Encode_WriteType_Unknown,
  ETC_VEDC_Encode_WriteType_Frame
}ETC_VEDC_Encode_WriteType;

typedef enum{
  ETC_VEDC_Encode_Error_Success,
  ETC_VEDC_Encode_Error_Unknown,
  ETC_VEDC_Encode_Error_EncoderDoesntSupportCodecStandard,
  ETC_VEDC_Encode_Error_NotImplemented,
  ETC_VEDC_Encode_Error_EncoderNotFound,
  ETC_VEDC_Encode_Error_EncoderAllocateFail,
  ETC_VEDC_Encode_Error_EncoderError,
  ETC_VEDC_Encode_Error_InvalidUsageType,
  ETC_VEDC_Encode_Error_InvalidRateControl,
  ETC_VEDC_Encode_Error_InvalidFrameSize,
  ETC_VEDC_Encode_Error_InvalidFrameRate,
  ETC_VEDC_Encode_Error_CudaInit,
  ETC_VEDC_Encode_Error_CudaNoDevice,
  ETC_VEDC_Encode_Error_CudaDeviceGet,
  ETC_VEDC_Encode_Error_CudaContextCreate,
  ETC_VEDC_Encode_Error_InvalidWriteType,
  ETC_VEDC_Encode_Error_NotImplementedPIXF
}ETC_VEDC_Encode_Error;

typedef enum{
  ETC_VEDC_Encoder_UniqueType_CudaContext
}ETC_VEDC_Encoder_UniqueType;

typedef enum{
  ETC_VEDC_Encode_PacketInfo_Nothing = 0x00,
  ETC_VEDC_Encode_PacketInfo_IDR = 0x01, /* Instantaneous Decoder Refresh */
  ETC_VEDC_Encode_PacketInfo_ICP = 0x02 /* Intra Coded Picture */
}ETC_VEDC_Encode_PacketInfo;

typedef enum{
  ETC_VEDC_EncoderSetting_UsageType_Realtime
}ETC_VEDC_EncoderSetting_UsageType;

typedef enum{
  ETC_VEDC_EncoderSetting_RateControlType_VBR, /* variable based bit rate */
  ETC_VEDC_EncoderSetting_RateControlType_TBR /* time based bit rate */
}ETC_VEDC_EncoderSetting_RateControlType;

typedef struct{
  ETC_VEDC_EncoderSetting_RateControlType Type;
  union{
    struct{
      uint32_t bps; /* bit per second */
    }VBR;
    struct{
      uint32_t bps; /* bit per second */
    }TBR;
  };
}ETC_VEDC_EncoderSetting_RateControl_t;

bool
ETC_VEDC_EncoderSetting_RateControl_IsEqual(
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl0,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl1
){
  ETC_VEDC_EncoderSetting_RateControlType Type = RateControl0->Type;
  if(Type != RateControl1->Type){
    return 0;
  }

  switch(Type){
    case ETC_VEDC_EncoderSetting_RateControlType_VBR:{
      return RateControl0->VBR.bps == RateControl1->VBR.bps;
    }
    case ETC_VEDC_EncoderSetting_RateControlType_TBR:{
      return RateControl0->TBR.bps == RateControl1->TBR.bps;
    }
  }

  return 1;
}

typedef struct{
  ETC_VCODECSTD CodecStandard;
  ETC_VEDC_EncoderSetting_UsageType UsageType;
  uint32_t FrameSizeX;
  uint32_t FrameSizeY;
  ETC_VEDC_EncoderSetting_RateControl_t RateControl;
  f32_t InputFrameRate;
}ETC_VEDC_EncoderSetting_t;

typedef ETC_VEDC_Encode_Error (*_ETC_VEDC_Encoder_Open_cb)(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_t *EncoderSetting,
  void *EncoderUnique);
typedef void *(*_ETC_VEDC_Encoder_GetUnique_cb)(
  void **EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType);
typedef void (*_ETC_VEDC_Encoder_Close_cb)(
  void **EncoderData);
typedef ETC_VEDC_Encode_Error (*_ETC_VEDC_Encoder_SetFrameSize_cb)(
  void **EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY);
typedef ETC_VEDC_Encode_Error (*_ETC_VEDC_Encoder_SetRateControl_cb)(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl);
typedef ETC_VEDC_Encode_Error (*_ETC_VEDC_Encoder_SetInputFrameRate_cb)(
  void **EncoderData,
  f32_t fps);
typedef bool (*_ETC_VEDC_Encoder_IsWriteType_cb)(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType);
typedef ETC_VEDC_Encode_Error (*_ETC_VEDC_Encoder_Write_cb)(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void *WriteData);
typedef bool (*_ETC_VEDC_Encoder_IsReadable_cb)(
  void **EncoderData);
typedef sint32_t (*_ETC_VEDC_Encoder_Read_cb)(
  void **EncoderData,
  ETC_VEDC_Encode_PacketInfo *PacketInfo,
  void **Data /* will point where is output */);

typedef struct{
  const char *Name;
  _ETC_VEDC_Encoder_Open_cb Open_cb;
  _ETC_VEDC_Encoder_GetUnique_cb GetUnique_cb;
  _ETC_VEDC_Encoder_Close_cb Close_cb;
  _ETC_VEDC_Encoder_SetFrameSize_cb SetFrameSize_cb;
  _ETC_VEDC_Encoder_SetRateControl_cb SetRateControl_cb;
  _ETC_VEDC_Encoder_SetInputFrameRate_cb SetInputFrameRate_cb;
  _ETC_VEDC_Encoder_IsWriteType_cb IsWriteType_cb;
  _ETC_VEDC_Encoder_Write_cb Write_cb;
  _ETC_VEDC_Encoder_IsReadable_cb IsReadable_cb;
  _ETC_VEDC_Encoder_Read_cb Read_cb;
}_ETC_VEDC_EncoderInfo;

#include "Encode/Encoder/Nothing/Nothing.h"
#ifdef ETC_VEDC_Encode_DefineEncoder_OpenH264
  #include "Encode/Encoder/OpenH264/OpenH264.h"
#endif
#ifdef ETC_VEDC_Encode_DefineEncoder_x264
  #include "Encode/Encoder/x264/x264.h"
#endif
#ifdef ETC_VEDC_Encode_DefineEncoder_nvenc
  #include "Encode/Encoder/nvenc/nvenc.h"
#endif

_ETC_VEDC_EncoderInfo _ETC_VEDC_EncoderList[] = {
  {
    .Name = "Nothing",
    .Open_cb = _ETC_VEDC_Encode_Encoder_Nothing_Open,
    .GetUnique_cb = _ETC_VEDC_Encode_Encoder_Nothing_GetUnique,
    .Close_cb = _ETC_VEDC_Encode_Encoder_Nothing_Close,
    .SetFrameSize_cb = _ETC_VEDC_Encode_Encoder_Nothing_SetFrameSize,
    .SetRateControl_cb = _ETC_VEDC_Encode_Encoder_Nothing_SetRateControl,
    .SetInputFrameRate_cb = _ETC_VEDC_Encode_Encoder_Nothing_SetInputFrameRate,
    .IsWriteType_cb = _ETC_VEDC_Encode_Encoder_Nothing_IsWriteType,
    .Write_cb = _ETC_VEDC_Encode_Encoder_Nothing_Write,
    .IsReadable_cb = _ETC_VEDC_Encode_Encoder_Nothing_IsReadable,
    .Read_cb = _ETC_VEDC_Encode_Encoder_Nothing_Read
  }
  #ifdef ETC_VEDC_Encode_DefineEncoder_OpenH264
    ,{
      .Name = "OpenH264",
      .Open_cb = _ETC_VEDC_Encode_Encoder_OpenH264_Open,
      .GetUnique_cb = _ETC_VEDC_Encode_Encoder_OpenH264_GetUnique,
      .Close_cb = _ETC_VEDC_Encode_Encoder_OpenH264_Close,
      .SetFrameSize_cb = _ETC_VEDC_Encode_Encoder_OpenH264_SetFrameSize,
      .SetRateControl_cb = _ETC_VEDC_Encode_Encoder_OpenH264_SetRateControl,
      .SetInputFrameRate_cb = _ETC_VEDC_Encode_Encoder_OpenH264_SetInputFrameRate,
      .IsWriteType_cb = _ETC_VEDC_Encode_Encoder_OpenH264_IsWriteType,
      .Write_cb = _ETC_VEDC_Encode_Encoder_OpenH264_Write,
      .IsReadable_cb = _ETC_VEDC_Encode_Encoder_OpenH264_IsReadable,
      .Read_cb = _ETC_VEDC_Encode_Encoder_OpenH264_Read
    }
  #endif
  #ifdef ETC_VEDC_Encode_DefineEncoder_x264
    ,{
      .Name = "x264",
      .Open_cb = _ETC_VEDC_Encode_Encoder_x264_Open,
      .GetUnique_cb = _ETC_VEDC_Encode_Encoder_x264_GetUnique,
      .Close_cb = _ETC_VEDC_Encode_Encoder_x264_Close,
      .SetFrameSize_cb = _ETC_VEDC_Encode_Encoder_x264_SetFrameSize,
      .SetRateControl_cb = _ETC_VEDC_Encode_Encoder_x264_SetRateControl,
      .SetInputFrameRate_cb = _ETC_VEDC_Encode_Encoder_x264_SetInputFrameRate,
      .IsWriteType_cb = _ETC_VEDC_Encode_Encoder_x264_IsWriteType,
      .Write_cb = _ETC_VEDC_Encode_Encoder_x264_Write,
      .IsReadable_cb = _ETC_VEDC_Encode_Encoder_x264_IsReadable,
      .Read_cb = _ETC_VEDC_Encode_Encoder_x264_Read
    }
  #endif
  #ifdef ETC_VEDC_Encode_DefineEncoder_nvenc
    ,{
      .Name = "nvenc",
      .Open_cb = _ETC_VEDC_Encode_Encoder_nvenc_Open,
      .GetUnique_cb = _ETC_VEDC_Encode_Encoder_nvenc_GetUnique,
      .Close_cb = _ETC_VEDC_Encode_Encoder_nvenc_Close,
      .SetFrameSize_cb = _ETC_VEDC_Encode_Encoder_nvenc_SetFrameSize,
      .SetRateControl_cb = _ETC_VEDC_Encode_Encoder_nvenc_SetRateControl,
      .SetInputFrameRate_cb = _ETC_VEDC_Encode_Encoder_nvenc_SetInputFrameRate,
      .IsWriteType_cb = _ETC_VEDC_Encode_Encoder_nvenc_IsWriteType,
      .Write_cb = _ETC_VEDC_Encode_Encoder_nvenc_Write,
      .IsReadable_cb = _ETC_VEDC_Encode_Encoder_nvenc_IsReadable,
      .Read_cb = _ETC_VEDC_Encode_Encoder_nvenc_Read
    }
  #endif
};

typedef struct{
  uintptr_t EncoderID;
  void *EncoderData;
}ETC_VEDC_Encode_t;

bool ETC_VEDC_Encode_IsSame(ETC_VEDC_Encode_t *Encode, uintptr_t EncoderNameSize, const void *EncoderName){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  uintptr_t Size = MEM_cstreu(di->Name);
  return MEM_ncmpn(di->Name, Size, EncoderName, EncoderNameSize) != 0;
}

void
ETC_VEDC_Encode_OpenNothing(
  ETC_VEDC_Encode_t *Encode
){
  Encode->EncoderID = 0;
}

ETC_VEDC_Encode_Error
ETC_VEDC_Encode_Open(
  ETC_VEDC_Encode_t *Encode,
  uintptr_t EncoderNameSize,
  const void *EncoderName,
  ETC_VEDC_EncoderSetting_t *EncoderSetting,
  void *EncoderUnique
){
  uintptr_t EncoderAmount = sizeof(_ETC_VEDC_EncoderList) / sizeof(_ETC_VEDC_EncoderList[0]);
  for(uintptr_t i = 1; i < EncoderAmount; i++){
    _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[i];
    uintptr_t Size = MEM_cstreu(di->Name);
    if(MEM_ncmpn(di->Name, Size, EncoderName, EncoderNameSize) != 0){
      ETC_VEDC_Encode_Error r = di->Open_cb(&Encode->EncoderData, EncoderSetting, EncoderUnique);
      if(r != ETC_VEDC_Encode_Error_Success){
        return r;
      }
      Encode->EncoderID = i;
      return ETC_VEDC_Encode_Error_Success;
    }
  }
  return ETC_VEDC_Encode_Error_EncoderNotFound;
}

void *
ETC_VEDC_Encode_GetUnique(
  ETC_VEDC_Encode_t *Encode,
  ETC_VEDC_Encoder_UniqueType UniqueType
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->GetUnique_cb(&Encode->EncoderData, UniqueType);
}

void
ETC_VEDC_Encode_Close(
  ETC_VEDC_Encode_t *Encode
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  di->Close_cb(&Encode->EncoderData);
}

ETC_VEDC_Encode_Error
ETC_VEDC_Encode_SetFrameSize(
  ETC_VEDC_Encode_t *Encode,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->SetFrameSize_cb(&Encode->EncoderData, FrameSizeX, FrameSizeY);
}

ETC_VEDC_Encode_Error
ETC_VEDC_Encode_SetRateControl(
  ETC_VEDC_Encode_t *Encode,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->SetRateControl_cb(&Encode->EncoderData, RateControl);
}

ETC_VEDC_Encode_Error
ETC_VEDC_Encode_SetInputFrameRate(
  ETC_VEDC_Encode_t *Encode,
  f32_t fps
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->SetInputFrameRate_cb(&Encode->EncoderData, fps);
}

ETC_VEDC_Encode_Error
ETC_VEDC_Encode_Write(
  ETC_VEDC_Encode_t *Encode,
  ETC_VEDC_Encode_WriteType WriteType,
  void *WriteData
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->Write_cb(&Encode->EncoderData, WriteType, WriteData);
}

bool
ETC_VEDC_Encode_IsReadable(
  ETC_VEDC_Encode_t *Encode
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->IsReadable_cb(&Encode->EncoderData);
}

sint32_t
ETC_VEDC_Encode_Read(
  ETC_VEDC_Encode_t *Encode,
  ETC_VEDC_Encode_PacketInfo *PacketInfo,
  void **Data
){
  _ETC_VEDC_EncoderInfo *di = &_ETC_VEDC_EncoderList[Encode->EncoderID];
  return di->Read_cb(&Encode->EncoderData, PacketInfo, Data);
}
