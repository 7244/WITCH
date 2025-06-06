/*
  this file belongs to
  github.com/6413
*/

#include "nvec_cuda.h"
#include <vector>
#include <memory>

typedef struct {
  CUdevice Device;
  CUcontext Context;
  NvEncoderCuda en;
  NV_ENC_INITIALIZE_PARAMS InternalSetting;
  NV_ENC_CONFIG encodeConfig;
  struct {
    std::vector<std::vector<uint8_t>> vPacket;
    bool Readed;
  }wrd;
}_ETC_VEDC_Encode_Encoder_nvenc_t;

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_nvenc_Open(
  void** EncoderData,
  ETC_VEDC_EncoderSetting_t* EncoderSetting,
  void* EncoderUnique
) {
  if (EncoderSetting->CodecStandard != ETC_VCODECSTD_H264) {
    return ETC_VEDC_Encode_Error_EncoderDoesntSupportCodecStandard;
  }
  if (EncoderSetting->RateControl.Type != ETC_VEDC_EncoderSetting_RateControlType_VBR) {
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  if (__GPU_IS_CUDA_INITED == 0) {
    if (cuInit(0) != CUDA_SUCCESS) {
      return ETC_VEDC_Encode_Error_CudaInit;
    }
    __GPU_IS_CUDA_INITED = 1;
  }

  {
    int device_count;
    if (cuDeviceGetCount(&device_count) != CUDA_SUCCESS) {
      __abort();
    }
    if (device_count == 0) {
      return ETC_VEDC_Encode_Error_CudaNoDevice;
    }
  }

  ETC_VEDC_Encode_Error r;

  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder =
    (_ETC_VEDC_Encode_Encoder_nvenc_t*)A_resize(0, sizeof(_ETC_VEDC_Encode_Encoder_nvenc_t));
  *EncoderData = (void*)Encoder;

  if (cuDeviceGet(&Encoder->Device, 0) != CUDA_SUCCESS) {
    r = ETC_VEDC_Encode_Error_CudaNoDevice;
    goto gt_err0;
  }

  if (cuCtxCreate(&Encoder->Context, 0, Encoder->Device) != CUDA_SUCCESS) {
    r = ETC_VEDC_Encode_Error_CudaContextCreate;
    goto gt_err0;
  }

  std::construct_at(&Encoder->en, Encoder->Context, EncoderSetting->FrameSizeX, EncoderSetting->FrameSizeY, NV_ENC_BUFFER_FORMAT_ARGB);
  {
    Encoder->InternalSetting = { NV_ENC_INITIALIZE_PARAMS_VER };
    Encoder->encodeConfig = { NV_ENC_CONFIG_VER };
    Encoder->InternalSetting.encodeConfig = &Encoder->encodeConfig;
    Encoder->en.CreateDefaultEncoderParams(&Encoder->InternalSetting, NV_ENC_CODEC_H264_GUID, NV_ENC_PRESET_LOW_LATENCY_HP_GUID);
    Encoder->InternalSetting.frameRateDen = 1000;
    Encoder->InternalSetting.frameRateNum = EncoderSetting->InputFrameRate * Encoder->InternalSetting.frameRateDen;
    Encoder->InternalSetting.encodeConfig->rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;
    Encoder->InternalSetting.encodeConfig->rcParams.averageBitRate = EncoderSetting->RateControl.VBR.bps;
    Encoder->InternalSetting.encodeConfig->rcParams.maxBitRate = EncoderSetting->RateControl.VBR.bps * 1.2;

    Encoder->en.CreateEncoder(&Encoder->InternalSetting);
  }
  std::construct_at(&Encoder->wrd.vPacket);

  Encoder->wrd.Readed = 1;

  return ETC_VEDC_Encode_Error_Success;

gt_err1:
  cuCtxDestroy(Encoder->Context);
gt_err0:
  A_resize(Encoder, 0);
  return r;
}

void*
_ETC_VEDC_Encode_Encoder_nvenc_GetUnique(
  void** EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  switch (UniqueType) {
    case ETC_VEDC_Encoder_UniqueType_CudaContext: {
      return (void*)Encoder->Context;
    }
  }
  return NULL;
}

void
_ETC_VEDC_Encode_Encoder_nvenc_Close(
  void** EncoderData
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  std::destroy_at(&Encoder->wrd.vPacket);
  std::destroy_at(&Encoder->en);
  cuCtxDestroy(Encoder->Context);
  A_resize(Encoder, 0);
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_nvenc_SetFrameSize(
  void** EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  NV_ENC_RECONFIGURE_PARAMS params{ NV_ENC_RECONFIGURE_PARAMS_VER };
  Encoder->InternalSetting.maxEncodeWidth = FrameSizeX;
  Encoder->InternalSetting.maxEncodeHeight = FrameSizeY;
  Encoder->InternalSetting.encodeWidth = FrameSizeX;
  Encoder->InternalSetting.encodeHeight = FrameSizeY;
  params.reInitEncodeParams = Encoder->InternalSetting;
  Encoder->en.Reconfigure(&params);

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_nvenc_SetRateControl(
  void** EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t* RateControl
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  if (RateControl->Type != ETC_VEDC_EncoderSetting_RateControlType_VBR) {
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  NV_ENC_RECONFIGURE_PARAMS params{ NV_ENC_RECONFIGURE_PARAMS_VER };
  Encoder->InternalSetting.encodeConfig->rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR;
  Encoder->InternalSetting.encodeConfig->rcParams.averageBitRate = RateControl->VBR.bps;
  Encoder->InternalSetting.encodeConfig->rcParams.maxBitRate = RateControl->VBR.bps;
  Encoder->InternalSetting.encodeConfig->rcParams.vbvBufferSize = RateControl->VBR.bps;
  params.reInitEncodeParams = Encoder->InternalSetting;
  Encoder->en.Reconfigure(&params);

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_nvenc_SetInputFrameRate(
  void** EncoderData,
  f32_t fps
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  NV_ENC_RECONFIGURE_PARAMS params{ NV_ENC_RECONFIGURE_PARAMS_VER };
  Encoder->InternalSetting.frameRateNum = fps * 1000;
  params.reInitEncodeParams = Encoder->InternalSetting;
  Encoder->en.Reconfigure(&params);

  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_nvenc_IsWriteType(
  void** EncoderData,
  ETC_VEDC_Encode_WriteType WriteType
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  switch (WriteType) {
    case ETC_VEDC_Encode_WriteType_Frame: {
      return 1;
    }
    default: {
      return 0;
    }
  }
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_nvenc_Write(
  void** EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void* WriteData,
  uint8_t Flags
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  ETC_VEDC_Encode_Error r;

  switch (WriteType) {
    case ETC_VEDC_Encode_WriteType_Frame: {
      ETC_VEDC_Encode_Frame_t* Frame = (ETC_VEDC_Encode_Frame_t*)WriteData;

      int nFrameSize = Encoder->en.GetFrameSize();

      const NvEncInputFrame* encoderInputFrame = Encoder->en.GetNextInputFrame();
      NvEncoderCuda::CopyToDeviceFrame(Encoder->Context, (void*)Frame->Data[0], 0, (CUdeviceptr)encoderInputFrame->inputPtr,
        (int)encoderInputFrame->pitch,
        Encoder->en.GetEncodeWidth(),
        Encoder->en.GetEncodeHeight(),
        CU_MEMORYTYPE_HOST,
        encoderInputFrame->bufferFormat,
        encoderInputFrame->chromaOffsets,
        encoderInputFrame->numChromaPlanes);
      NV_ENC_PIC_PARAMS picParams = {};
      if (Flags & ETC_VEDC_EncoderFlag_ResetIDR) {
        picParams.encodePicFlags |= NV_ENC_PIC_FLAG_FORCEIDR;
      }
      Encoder->en.EncodeFrame(Encoder->wrd.vPacket, &picParams);

      Encoder->wrd.Readed = 0;

      return ETC_VEDC_Encode_Error_Success;
    }
    default: {
      return ETC_VEDC_Encode_Error_InvalidWriteType;
    }
  }
}

bool
_ETC_VEDC_Encode_Encoder_nvenc_IsReadable(
  void** EncoderData
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  return Encoder->wrd.Readed == 0;
}

sint32_t
_ETC_VEDC_Encode_Encoder_nvenc_Read(
  void** EncoderData,
  ETC_VEDC_Encode_PacketInfo* PacketInfo,
  void** Data
) {
  _ETC_VEDC_Encode_Encoder_nvenc_t* Encoder = (_ETC_VEDC_Encode_Encoder_nvenc_t*)*EncoderData;

  if (Encoder->wrd.Readed == 1) {
    return 0;
  }

  Encoder->wrd.Readed = 1;
  if (Encoder->wrd.vPacket.empty()) {
    return 0;
  }

  if (Encoder->wrd.vPacket.size() != 1) {
    __abort();
  }

  *Data = &Encoder->wrd.vPacket[0][0];
  return Encoder->wrd.vPacket[0].size();
}
