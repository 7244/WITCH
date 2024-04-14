#define NAL_UNKNOWN _ETC_VEDC_Encode_Encoder_x264_NAL_UNKNOWN
#define NAL_SLICE _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE
#define NAL_SLICE_DPA _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE_DPA
#define NAL_SLICE_DPB _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE_DPB
#define NAL_SLICE_DPC _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE_DPC
#define NAL_SLICE_IDR _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE_IDR
#define NAL_SEI _ETC_VEDC_Encode_Encoder_x264_NAL_SEI
#define NAL_SPS _ETC_VEDC_Encode_Encoder_x264_NAL_SPS
#define NAL_PPS _ETC_VEDC_Encode_Encoder_x264_NAL_PPS
#define NAL_PRIORITY_DISPOSABLE _ETC_VEDC_Encode_Encoder_x264_NAL_PRIORITY_DISPOSABLE
#define NAL_PRIORITY_LOW _ETC_VEDC_Encode_Encoder_x264_NAL_PRIORITY_LOW
#define NAL_PRIORITY_HIGH _ETC_VEDC_Encode_Encoder_x264_NAL_PRIORITY_HIGH
#define NAL_PRIORITY_HIGHEST _ETC_VEDC_Encode_Encoder_x264_NAL_PRIORITY_HIGHEST
#include <x264.h>
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
#include _WITCH_PATH(ETC/CC/st/st.h)

typedef struct{
  x264_t *en;
  x264_param_t InternalSetting;
  struct{
    x264_nal_t *pnal;
    int inal;
    uint32_t i;
  }wrd;
}_ETC_VEDC_Encode_Encoder_x264_t;

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_x264_ToInternal_RateControl(
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  x264_param_t *InternalSetting = &Encoder->InternalSetting;

  InternalSetting->i_nal_hrd = X264_NAL_HRD_CBR;
  InternalSetting->rc.i_rc_method = X264_RC_ABR;

  switch(RateControl->Type){
    case ETC_VEDC_EncoderSetting_RateControlType_VBR:{
      InternalSetting->rc.i_bitrate = RateControl->VBR.bps / 1024;
      InternalSetting->rc.i_vbv_max_bitrate = RateControl->VBR.bps / 1024;
      InternalSetting->rc.i_vbv_buffer_size = RateControl->VBR.bps / 1024;
      return ETC_VEDC_Encode_Error_Success;
    }
    case ETC_VEDC_EncoderSetting_RateControlType_TBR:{
      InternalSetting->rc.i_bitrate = RateControl->TBR.bps / 1024;
      InternalSetting->rc.i_vbv_max_bitrate = RateControl->TBR.bps / 1024;
      InternalSetting->rc.i_vbv_buffer_size = RateControl->TBR.bps / 1024;
      return ETC_VEDC_Encode_Error_Success;
    }
    default:{
      return ETC_VEDC_Encode_Error_InvalidRateControl;
    }
  }
}

ETC_VEDC_Encode_Error _ETC_VEDC_Encode_Encoder_x264_Open(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_t *EncoderSetting,
  void *EncoderUnique
){
  if(EncoderSetting->CodecStandard != ETC_VCODECSTD_H264){
    return ETC_VEDC_Encode_Error_EncoderDoesntSupportCodecStandard;
  }

  _ETC_VEDC_Encode_Encoder_x264_t *Encoder =
    (_ETC_VEDC_Encode_Encoder_x264_t *)A_resize(0, sizeof(_ETC_VEDC_Encode_Encoder_x264_t));
  *EncoderData = (void *)Encoder;

  ETC_VEDC_Encode_Error r;

  if(x264_param_default_preset(&Encoder->InternalSetting, "veryfast", "zerolatency") != 0){
    r = ETC_VEDC_Encode_Error_EncoderError;
    goto gt_err0;
  }
  if(x264_param_apply_profile(&Encoder->InternalSetting, "high") != 0){
    r = ETC_VEDC_Encode_Error_EncoderError;
    goto gt_err0;
  }

  Encoder->InternalSetting.i_csp = X264_CSP_I420;
  Encoder->InternalSetting.i_width = EncoderSetting->FrameSizeX;
  Encoder->InternalSetting.i_height = EncoderSetting->FrameSizeY;
  Encoder->InternalSetting.b_vfr_input = 0;
  Encoder->InternalSetting.b_repeat_headers = 1;
  Encoder->InternalSetting.b_annexb = 1;

  Encoder->InternalSetting.i_fps_num = EncoderSetting->InputFrameRate;
  Encoder->InternalSetting.i_fps_den = 1;
  Encoder->InternalSetting.i_timebase_num = 1000;
  Encoder->InternalSetting.i_timebase_den = 1;

  Encoder->InternalSetting.rc.i_qp_min = 23;
  {
    r = _ETC_VEDC_Encode_Encoder_x264_ToInternal_RateControl(Encoder, &EncoderSetting->RateControl);
    if(r != ETC_VEDC_Encode_Error_Success){
      goto gt_err0;
    }
  }
  Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
  if(Encoder->en == NULL){
    r = ETC_VEDC_Encode_Error_EncoderError;
    goto gt_err0;
  }

  return ETC_VEDC_Encode_Error_Success;

  gt_err0:
  A_resize(Encoder, 0);
  return r;
}

void *_ETC_VEDC_Encode_Encoder_x264_GetUnique(
  void **EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  return NULL;
}

void
_ETC_VEDC_Encode_Encoder_x264_Close(
  void **EncoderData
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  x264_encoder_close(Encoder->en);
  A_resize(Encoder, 0);
}

ETC_VEDC_Encode_Error _ETC_VEDC_Encode_Encoder_x264_SetFrameSize(
  void **EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  x264_encoder_close(Encoder->en);

  int OldX = Encoder->InternalSetting.i_width;
  int OldY = Encoder->InternalSetting.i_height;
  Encoder->InternalSetting.i_width = FrameSizeX;
  Encoder->InternalSetting.i_height = FrameSizeY;

  Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
  if(Encoder->en == NULL){
    Encoder->InternalSetting.i_width = OldX;
    Encoder->InternalSetting.i_height = OldY;
    Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
    if(Encoder->en == NULL){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidFrameSize;
  }

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_x264_SetRateControl(
  void **EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t *RateControl
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  x264_encoder_close(Encoder->en);

  ETC_VEDC_Encode_Error r;

  x264_param_t OldP = Encoder->InternalSetting;

  r = _ETC_VEDC_Encode_Encoder_x264_ToInternal_RateControl(Encoder, RateControl);
  if(r != ETC_VEDC_Encode_Error_Success){
    return r;
  }

  Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
  if(Encoder->en == NULL){
    Encoder->InternalSetting = OldP;
    Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
    if(Encoder->en == NULL){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_x264_SetInputFrameRate(
  void **EncoderData,
  f32_t fps
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  x264_encoder_close(Encoder->en);

  int Old = Encoder->InternalSetting.i_fps_num;
  Encoder->InternalSetting.i_fps_num = fps;
  Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
  if(Encoder->en == NULL){
    Encoder->InternalSetting.i_fps_num = Old;
    Encoder->en = x264_encoder_open(&Encoder->InternalSetting);
    if(Encoder->en == NULL){
      /* faulty encoder */
      PR_abort();
    }
    return ETC_VEDC_Encode_Error_InvalidFrameRate;
  }

  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_x264_IsWriteType(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

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
_ETC_VEDC_Encode_Encoder_x264_Write(
  void **EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void *WriteData
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  ETC_VEDC_Encode_Error r;

  switch(WriteType){
    case ETC_VEDC_Encode_WriteType_Frame:{
      bool Alloced = 0;
      ETC_VEDC_Encode_Frame_t FrameStack;
      ETC_VEDC_Encode_Frame_t *Frame;
      uint8_t ImageCount = ETC_PIXF_GetImageCount(ETC_PIXF_YUV420p);
      {
        ETC_VEDC_Encode_Frame_t *ParameterFrame = (ETC_VEDC_Encode_Frame_t *)WriteData;
        if(ParameterFrame->Properties.PixelFormat != ETC_PIXF_YUV420p){
          Frame = &FrameStack;
          Alloced = 1;
          Frame->Properties.PixelFormat = ETC_PIXF_YUV420p;
          Frame->Properties.SizeX = ParameterFrame->Properties.SizeX;
          Frame->Properties.SizeY = ParameterFrame->Properties.SizeY;
          Frame->TimeStamp = ParameterFrame->TimeStamp;
          for(uint8_t i = 0; i < ImageCount; i++){
            uint32_t x = Frame->Properties.SizeX;
            uint32_t y = Frame->Properties.SizeY;
            ETC_PIXF_GetImageSize(ETC_PIXF_YUV420p, i, &x, &y);
            Frame->Properties.Stride[i] = x;
            Frame->Data[i] = A_resize(0, x * y);
          }

          CC_st_convert(
            ParameterFrame->Properties.PixelFormat,
            ETC_PIXF_YUV420p,
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

      x264_picture_t pic;
      x264_picture_init(&pic);

      switch(Frame->Properties.PixelFormat){
        case ETC_PIXF_YUV420p:{
          pic.img.i_csp = X264_CSP_I420;
          break;
        }
        default:{
          r = ETC_VEDC_Encode_Error_NotImplementedPIXF;
          goto gt_err0;
        }
      }

      for(uint8_t i = 0; i < 4; i++) { pic.img.i_stride[i] = Frame->Properties.Stride[i]; }
      for(uint8_t i = 0; i < 4; i++) { pic.img.plane[i] = (uint8_t *)Frame->Data[i]; }

      pic.i_pts = Frame->TimeStamp / 1000000;

      {
        x264_picture_t pic_filler;
        int ir = x264_encoder_encode(
          Encoder->en,
          &Encoder->wrd.pnal,
          &Encoder->wrd.inal,
          &pic,
          &pic_filler);
        if(ir < 0){
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
_ETC_VEDC_Encode_Encoder_x264_IsReadable(
  void **EncoderData
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  return Encoder->wrd.inal != 0;
}

sint32_t
_ETC_VEDC_Encode_Encoder_x264_Read(
  void **EncoderData,
  ETC_VEDC_Encode_PacketInfo *PacketInfo,
  void **Data
){
  _ETC_VEDC_Encode_Encoder_x264_t *Encoder = (_ETC_VEDC_Encode_Encoder_x264_t *)*EncoderData;

  if(Encoder->wrd.i == Encoder->wrd.inal){
    return 0;
  }

  x264_nal_t *nal = &Encoder->wrd.pnal[Encoder->wrd.i++];

  switch(nal->i_type){
    case _ETC_VEDC_Encode_Encoder_x264_NAL_SLICE_IDR:
    {
      *PacketInfo = ETC_VEDC_Encode_PacketInfo_IDR;
      break;
    }
    case _ETC_VEDC_Encode_Encoder_x264_NAL_SPS:
    case _ETC_VEDC_Encode_Encoder_x264_NAL_PPS:
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

  *Data = nal->p_payload;

  return nal->i_payload - nal->i_padding;
}
