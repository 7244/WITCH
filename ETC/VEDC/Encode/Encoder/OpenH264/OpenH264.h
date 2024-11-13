#define NAL_UNKNOWN _ETC_VEDC_Encode_Encoder_OpenH264_NAL_UNKNOWN
#define NAL_SLICE _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SLICE
#define NAL_SLICE_DPA _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SLICE_DPA
#define NAL_SLICE_DPB _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SLICE_DPB
#define NAL_SLICE_DPC _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SLICE_DPC
#define NAL_SLICE_IDR _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SLICE_IDR
#define NAL_SEI _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SEI
#define NAL_SPS _ETC_VEDC_Encode_Encoder_OpenH264_NAL_SPS
#define NAL_PPS _ETC_VEDC_Encode_Encoder_OpenH264_NAL_PPS
#define NAL_PRIORITY_DISPOSABLE _ETC_VEDC_Encode_Encoder_OpenH264_NAL_PRIORITY_DISPOSABLE
#define NAL_PRIORITY_LOW _ETC_VEDC_Encode_Encoder_OpenH264_NAL_PRIORITY_LOW
#define NAL_PRIORITY_HIGH _ETC_VEDC_Encode_Encoder_OpenH264_NAL_PRIORITY_HIGH
#define NAL_PRIORITY_HIGHEST _ETC_VEDC_Encode_Encoder_OpenH264_NAL_PRIORITY_HIGHEST
#include <wels/codec_api.h>
#undef NAL_UNKNOWN
#undef NAL_SLICE
#undef NAL_SLICE_DPA
#undef NAL_SLICE_DPB
#undef NAL_SLICE_DPC
#undef NAL_SLICE_IDR
#undef NAL_SEI
#undef NAL_SPS
#undef NAL_PPS
#undef NAL_PRIORITY_DISPOSABLE
#undef NAL_PRIORITY_LOW
#undef NAL_PRIORITY_HIGH
#undef NAL_PRIORITY_HIGHEST

#include _WITCH_PATH(A/A.h)
#include <pixfconv/st/st.h>

typedef struct{
  ISVCEncoder *en;
  SEncParamExt InternalSetting;
  struct{
    SFrameBSInfo info;
    uint32_t i;
  }wrd;
}_ETC_VEDC_Encode_Encoder_OpenH264_t;

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_ToInternal_RateControl(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  SEncParamExt *InternalSetting = &Encoder->InternalSetting;

  switch(RateControl->Type){
    case ETC_VEDC_EncoderSetting_RateControlType_VBR:{
      InternalSetting->iRCMode = RC_BITRATE_MODE;
      InternalSetting->iTargetBitrate = RateControl->VBR.bps;
      return ETC_VEDC_Encode_Error_Success;
    }
    case ETC_VEDC_EncoderSetting_RateControlType_TBR:{
      InternalSetting->iRCMode = RC_TIMESTAMP_MODE;
      InternalSetting->iTargetBitrate = RateControl->TBR.bps;
      return ETC_VEDC_Encode_Error_Success;
    }
    default:{
      return ETC_VEDC_Encode_Error_InvalidRateControl;
    }
  }
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_ToInternal_EncoderSetting(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder,
  ETC_VEDC_EncoderSetting_t *Setting
){
  SEncParamExt *InternalSetting = &Encoder->InternalSetting;

  Encoder->en->GetDefaultParams(InternalSetting);

  {
    /* hardcore value setting */
    /* its forced because openh264's default settings are not acceptable */

    /* default value for libx264 */
    InternalSetting->iMinQp = 23;
  }

  switch(Setting->UsageType){
    case ETC_VEDC_EncoderSetting_UsageType_Realtime:{
      InternalSetting->iUsageType = CAMERA_VIDEO_REAL_TIME;
      break;
    }
    default:{
      return ETC_VEDC_Encode_Error_InvalidUsageType;
    }
  }

  InternalSetting->iPicWidth = Setting->FrameSizeX;
  InternalSetting->iPicHeight = Setting->FrameSizeY;

  {
    ETC_VEDC_Encode_Error r = _ETC_VEDC_Encode_Encoder_OpenH264_ToInternal_RateControl(Encoder, &Setting->RateControl);
    if(r != ETC_VEDC_Encode_Error_Success){
      return r;
    }
  }

  InternalSetting->fMaxFrameRate = Setting->InputFrameRate;

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_Allocate(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder
){
  if(WelsCreateSVCEncoder(&Encoder->en) != 0){
    return ETC_VEDC_Encode_Error_EncoderAllocateFail;
  }
  return ETC_VEDC_Encode_Error_Success;
}
void
_ETC_VEDC_Encode_Encoder_OpenH264_Deallocate(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder
){
  WelsDestroySVCEncoder(Encoder->en);
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_Open_(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder
){
  int ir = Encoder->en->InitializeExt(&Encoder->InternalSetting);
  if(ir != 0){
    return ETC_VEDC_Encode_Error_EncoderError;
  }

  return ETC_VEDC_Encode_Error_Success;
}
void
_ETC_VEDC_Encode_Encoder_OpenH264_Close_(
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder
){
  int ir = Encoder->en->Uninitialize();
  if(ir != 0){
    PR_abort();
  }
}

ETC_VEDC_Encode_Error _ETC_VEDC_Encode_Encoder_OpenH264_Open(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_t *EncoderSetting,
  void *EncoderUnique
){
  if(EncoderSetting->CodecStandard != ETC_VCODECSTD_H264){
    return ETC_VEDC_Encode_Error_EncoderDoesntSupportCodecStandard;
  }

  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder =
    (_ETC_VEDC_Encode_Encoder_OpenH264_t *)A_resize(0, sizeof(_ETC_VEDC_Encode_Encoder_OpenH264_t));
  *EncoderData = (void *)Encoder;

  ETC_VEDC_Encode_Error r;

  r = _ETC_VEDC_Encode_Encoder_OpenH264_Allocate(Encoder);
  if(r != ETC_VEDC_Encode_Error_Success){
    goto gt_err0;
  }

  r = _ETC_VEDC_Encode_Encoder_OpenH264_ToInternal_EncoderSetting(Encoder, EncoderSetting);
  if(r != ETC_VEDC_Encode_Error_Success){
    goto gt_err1;
  }

  r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
  if(r != ETC_VEDC_Encode_Error_Success){
    goto gt_err1;
  }

  Encoder->wrd.i = 0;

  return ETC_VEDC_Encode_Error_Success;

  gt_err1:
  _ETC_VEDC_Encode_Encoder_OpenH264_Deallocate(Encoder);
  gt_err0:
  A_resize(Encoder, 0);
  return r;
}

void *_ETC_VEDC_Encode_Encoder_OpenH264_GetUnique(
  void **EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  return NULL;
}

void
_ETC_VEDC_Encode_Encoder_OpenH264_Close(
  void **EncoderData
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  _ETC_VEDC_Encode_Encoder_OpenH264_Close_(Encoder);
  _ETC_VEDC_Encode_Encoder_OpenH264_Deallocate(Encoder);
  A_resize(Encoder, 0);
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_SetFrameSize(
  void **EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  _ETC_VEDC_Encode_Encoder_OpenH264_Close_(Encoder);

  int OldX = Encoder->InternalSetting.iPicWidth;
  int OldY = Encoder->InternalSetting.iPicHeight;
  Encoder->InternalSetting.iPicWidth = FrameSizeX;
  Encoder->InternalSetting.iPicHeight = FrameSizeY;

  ETC_VEDC_Encode_Error r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
  if(r != ETC_VEDC_Encode_Error_Success){
    Encoder->InternalSetting.iPicWidth = OldX;
    Encoder->InternalSetting.iPicHeight = OldY;
    r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
    if(r != ETC_VEDC_Encode_Error_Success){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidFrameSize;
  }

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_SetRateControl(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  _ETC_VEDC_Encode_Encoder_OpenH264_Close_(Encoder);

  ETC_VEDC_Encode_Error r;

  r = _ETC_VEDC_Encode_Encoder_OpenH264_ToInternal_RateControl(Encoder, RateControl);
  if(r != ETC_VEDC_Encode_Error_Success){
    return r;
  }

  SEncParamExt OldP = Encoder->InternalSetting;
  r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
  if(r != ETC_VEDC_Encode_Error_Success){
    Encoder->InternalSetting = OldP;
    r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
    if(r != ETC_VEDC_Encode_Error_Success){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_SetInputFrameRate(
  void **EncoderData,
  f32_t fps
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  _ETC_VEDC_Encode_Encoder_OpenH264_Close_(Encoder);

  f32_t Old = Encoder->InternalSetting.fMaxFrameRate;
  Encoder->InternalSetting.fMaxFrameRate = fps;

  ETC_VEDC_Encode_Error r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
  if(r != ETC_VEDC_Encode_Error_Success){
    Encoder->InternalSetting.fMaxFrameRate = Old;
    r = _ETC_VEDC_Encode_Encoder_OpenH264_Open_(Encoder);
    if(r != ETC_VEDC_Encode_Error_Success){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidFrameRate;
  }

  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_OpenH264_IsWriteType(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  switch(WriteType){
    case ETC_VEDC_Encode_WriteType_Frame:{
      return 1;
    }
    default:{
      return 0;
    }
  }
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_OpenH264_Write(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void *WriteData
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  ETC_VEDC_Encode_Error r;

  switch(WriteType){
    case ETC_VEDC_Encode_WriteType_Frame:{
      bool Alloced = 0;
      ETC_VEDC_Encode_Frame_t FrameStack;
      ETC_VEDC_Encode_Frame_t *Frame;
      uint8_t ImageCount = PIXF_GetImageCount(PIXF_YUV420p);
      {
        ETC_VEDC_Encode_Frame_t *ParameterFrame = (ETC_VEDC_Encode_Frame_t *)WriteData;
        if(ParameterFrame->Properties.PixelFormat != PIXF_YUV420p){
          Frame = &FrameStack;
          Alloced = 1;
          Frame->Properties.PixelFormat = PIXF_YUV420p;
          Frame->Properties.SizeX = ParameterFrame->Properties.SizeX;
          Frame->Properties.SizeY = ParameterFrame->Properties.SizeY;
          Frame->TimeStamp = ParameterFrame->TimeStamp;
          for(uint8_t i = 0; i < ImageCount; i++){
            uint32_t x = Frame->Properties.SizeX;
            uint32_t y = Frame->Properties.SizeY;
            PIXF_GetImageSize(PIXF_YUV420p, i, &x, &y);
            Frame->Properties.Stride[i] = x;
            Frame->Data[i] = A_resize(0, x * y);
          }

          pixfconv_st_convert(
            ParameterFrame->Properties.PixelFormat,
            PIXF_YUV420p,
            Frame->Properties.SizeX,
            Frame->Properties.SizeY,
            (const uint32_t *)ParameterFrame->Properties.Stride,
            (const uint32_t *)Frame->Properties.Stride,
            (const uint8_t *const *)ParameterFrame->Data,
            (uint8_t *const *)Frame->Data);
        }
        else{
          Frame = ParameterFrame;
        }
      }

      SSourcePicture InternalSource;

      switch(Frame->Properties.PixelFormat){
        case PIXF_YUV420p:{
          InternalSource.iColorFormat = videoFormatI420;
          break;
        }
        default:{
          r = ETC_VEDC_Encode_Error_NotImplementedPIXF;
          goto gt_err0;
        }
      }

      for(uint8_t i = 0; i < 4; i++) { InternalSource.iStride[i] = Frame->Properties.Stride[i]; }
      for(uint8_t i = 0; i < 4; i++) { InternalSource.pData[i] = (unsigned char *)Frame->Data[i]; }

      InternalSource.iPicWidth = Frame->Properties.SizeX;
      InternalSource.iPicHeight = Frame->Properties.SizeY;

      InternalSource.uiTimeStamp = Frame->TimeStamp / 1000000;

      {
        int ir = Encoder->en->EncodeFrame(&InternalSource, &Encoder->wrd.info);
        if(ir != 0){
          r = ETC_VEDC_Encode_Error_EncoderError;
          goto gt_err0;
        }
      }

      Encoder->wrd.i = 0;

      r = ETC_VEDC_Encode_Error_Success;
      gt_err0:
      if(Alloced){
        for(uint8_t i = 0; i < ImageCount; i++){
          A_resize(Frame->Data[i], 0);
        }
      }
      return r;
    }
    default:{
      return ETC_VEDC_Encode_Error_InvalidWriteType;
    }
  }
}

bool
_ETC_VEDC_Encode_Encoder_OpenH264_IsReadable(
  void **EncoderData
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  return Encoder->wrd.info.iLayerNum != 0;
}

sint32_t
_ETC_VEDC_Encode_Encoder_OpenH264_Read(
  void **EncoderData,
  ETC_VEDC_Encode_PacketInfo *PacketInfo,
  void **Data
){
  _ETC_VEDC_Encode_Encoder_OpenH264_t *Encoder = (_ETC_VEDC_Encode_Encoder_OpenH264_t *)*EncoderData;

  if(Encoder->wrd.i == Encoder->wrd.info.iLayerNum){
    return 0;
  }

  SLayerBSInfo *linfo = &Encoder->wrd.info.sLayerInfo[Encoder->wrd.i];

  switch(linfo->eFrameType){
    case videoFrameTypeIDR:
    {
      *PacketInfo = ETC_VEDC_Encode_PacketInfo_IDR;
      break;
    }
    case videoFrameTypeI:
    {
      *PacketInfo = ETC_VEDC_Encode_PacketInfo_ICP;
      break;
    }
    default:
    {
      *PacketInfo = ETC_VEDC_Encode_PacketInfo_Nothing;
      break;
    }
  }

  uint32_t r = 0;
  for(uint32_t iNal = 0; iNal < linfo->iNalCount; iNal++){
    r += linfo->pNalLengthInByte[iNal];
  }

  *Data = linfo->pBsBuf;

  Encoder->wrd.i++;

  return r;
}
