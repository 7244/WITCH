/*
  AMD AMF encoder implementation for the video encoding framework
  Requires AMD Advanced Media Framework SDK v1.4.36
*/

#pragma once

#include <AMF/core/Platform.h>
#include <AMF/core/Factory.h>
#include <AMF/core/Context.h>
#include <AMF/core/Surface.h>
#include <AMF/core/Buffer.h>
#include <AMF/core/Variant.h>
#include <AMF/components/Component.h>
#include <vector>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// AMF component IDs - these should be defined in AMF headers
#ifndef AMFVideoEncoderVCE_AVC
#define AMFVideoEncoderVCE_AVC L"AMFVideoEncoderVCE_AVC"
#endif

// Make these non-static so they can be shared if needed
bool __AMF_IS_INITIALIZED = false;
amf::AMFFactory* __AMF_FACTORY = nullptr;
void* __AMF_MODULE = nullptr;

typedef struct {
  amf::AMFContextPtr context;
  amf::AMFComponentPtr encoder;
  struct {
    std::vector<std::vector<uint8_t>> vPacket;
    bool Readed;
  } wrd;
  amf::AMF_SURFACE_FORMAT surfaceFormat;
  uint32_t width;
  uint32_t height;
  uint32_t frameRateNum;
  uint32_t frameRateDen;
  uint32_t bitrate;
} _ETC_VEDC_Encode_Encoder_amf_t;

// Initialize AMF factory
static AMF_RESULT InitializeAMFFactory() {
  if (__AMF_IS_INITIALIZED) {
    return AMF_OK;
  }

#ifdef _WIN32
  __AMF_MODULE = LoadLibraryA(AMF_DLL_NAMEA);
#else
  __AMF_MODULE = dlopen(AMF_DLL_NAMEA, RTLD_LAZY);
#endif

  if (!__AMF_MODULE) {
    return AMF_FAIL;
  }

#ifdef _WIN32
  AMFInit_Fn AMFInit = (AMFInit_Fn)GetProcAddress((HMODULE)__AMF_MODULE, AMF_INIT_FUNCTION_NAME);
#else
  AMFInit_Fn AMFInit = (AMFInit_Fn)dlsym(__AMF_MODULE, AMF_INIT_FUNCTION_NAME);
#endif

  if (!AMFInit) {
    return AMF_FAIL;
  }

  AMF_RESULT res = AMFInit(AMF_FULL_VERSION, &__AMF_FACTORY);
  if (res != AMF_OK) {
    return res;
  }

  __AMF_IS_INITIALIZED = true;
  return AMF_OK;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_amf_Open(
  void** EncoderData,
  ETC_VEDC_EncoderSetting_t* EncoderSetting,
  void* EncoderUnique
) {
  // Only support H.264 for now
  if (EncoderSetting->CodecStandard != ETC_VCODECSTD_H264) {
    return ETC_VEDC_Encode_Error_EncoderDoesntSupportCodecStandard;
  }

  // Only support VBR rate control
  if (EncoderSetting->RateControl.Type != ETC_VEDC_EncoderSetting_RateControlType_VBR) {
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  // Initialize AMF if not already done
  if (InitializeAMFFactory() != AMF_OK) {
    return ETC_VEDC_Encode_Error_EncoderError;
  }

  _ETC_VEDC_Encode_Encoder_amf_t* Encoder =
    (_ETC_VEDC_Encode_Encoder_amf_t*)A_resize(0, sizeof(_ETC_VEDC_Encode_Encoder_amf_t));
  *EncoderData = (void*)Encoder;

  // Zero initialize the entire structure first
  memset(Encoder, 0, sizeof(_ETC_VEDC_Encode_Encoder_amf_t));

  // Explicitly construct the smart pointers in place
  std::construct_at(&Encoder->context);
  std::construct_at(&Encoder->encoder);

  // Create AMF context - use temporary pointer
  amf::AMFContext* pContext = nullptr;
  AMF_RESULT res = __AMF_FACTORY->CreateContext(&pContext);
  if (res != AMF_OK) {
    std::destroy_at(&Encoder->encoder);
    std::destroy_at(&Encoder->context);
    A_resize(Encoder, 0);
    return ETC_VEDC_Encode_Error_EncoderError;
  }
  Encoder->context = pContext;

  // Initialize context - try DX11 first, then OpenCL
  res = Encoder->context->InitDX11(nullptr);
  if (res != AMF_OK) {
    res = Encoder->context->InitOpenCL(nullptr);
    if (res != AMF_OK) {
      A_resize(Encoder, 0);
      return ETC_VEDC_Encode_Error_EncoderError;
    }
  }

  // Create H.264 encoder component - use temporary pointer
  amf::AMFComponent* pComponent = nullptr;
  res = __AMF_FACTORY->CreateComponent(Encoder->context, AMFVideoEncoderVCE_AVC, &pComponent);
  if (res != AMF_OK) {
    std::destroy_at(&Encoder->encoder);
    std::destroy_at(&Encoder->context);
    A_resize(Encoder, 0);
    return ETC_VEDC_Encode_Error_EncoderError;
  }
  Encoder->encoder = pComponent;

  // Store settings
  Encoder->width = EncoderSetting->FrameSizeX;
  Encoder->height = EncoderSetting->FrameSizeY;
  Encoder->surfaceFormat = amf::AMF_SURFACE_BGRA;
  Encoder->frameRateNum = (uint32_t)(EncoderSetting->InputFrameRate * 1000);
  Encoder->frameRateDen = 1000;
  Encoder->bitrate = EncoderSetting->RateControl.VBR.bps;

  // Configure encoder - using explicit AMFVariant constructors

  // Set frame size
  AMFSize frameSize = AMFConstructSize(Encoder->width, Encoder->height);
  amf::AMFVariant varFrameSize(frameSize);
  Encoder->encoder->SetProperty(L"FrameSize", varFrameSize);

  // Set frame rate  
  AMFRate frameRate = AMFConstructRate(Encoder->frameRateNum, Encoder->frameRateDen);
  amf::AMFVariant varFrameRate(frameRate);
  Encoder->encoder->SetProperty(L"FrameRate", varFrameRate);

  // Set bitrate
  amf::AMFVariant varBitrate((amf_int64)Encoder->bitrate);
  Encoder->encoder->SetProperty(L"TargetBitrate", varBitrate);
  Encoder->encoder->SetProperty(L"PeakBitrate", varBitrate);

  // Set usage for low latency
  amf::AMFVariant varUsage((amf_int64)0); // Ultra low latency usage
  Encoder->encoder->SetProperty(L"Usage", varUsage);

  // Set rate control to VBR
  amf::AMFVariant varRateControl((amf_int64)1); // VBR mode
  Encoder->encoder->SetProperty(L"RateControlMethod", varRateControl);

  // Disable B-frames for low latency
  amf::AMFVariant varBFrames((amf_int64)0);
  Encoder->encoder->SetProperty(L"BPicturesPattern", varBFrames);

  // Set GOP size (infinite)
  amf::AMFVariant varGOP((amf_int64)0);
  Encoder->encoder->SetProperty(L"GOPSize", varGOP);

  // Initialize the encoder
  res = Encoder->encoder->Init(Encoder->surfaceFormat, Encoder->width, Encoder->height);
  if (res != AMF_OK) {
    Encoder->context->Terminate();
    std::destroy_at(&Encoder->encoder);
    std::destroy_at(&Encoder->context);
    A_resize(Encoder, 0);
    return ETC_VEDC_Encode_Error_EncoderError;
  }

  // Initialize packet storage
  std::construct_at(&Encoder->wrd.vPacket);
  Encoder->wrd.Readed = true;

  return ETC_VEDC_Encode_Error_Success;
}

void*
_ETC_VEDC_Encode_Encoder_amf_GetUnique(
  void** EncoderData,
  ETC_VEDC_Encoder_UniqueType UniqueType
) {
  switch (UniqueType) {
  case ETC_VEDC_Encoder_UniqueType_CudaContext:
    // AMF doesn't use CUDA
    return nullptr;
  }
  return nullptr;
}

void
_ETC_VEDC_Encode_Encoder_amf_Close(
  void** EncoderData
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  if (Encoder->encoder) {
    Encoder->encoder->Drain();
    Encoder->encoder->Terminate();
  }

  if (Encoder->context) {
    Encoder->context->Terminate();
  }

  std::destroy_at(&Encoder->wrd.vPacket);
  std::destroy_at(&Encoder->encoder);
  std::destroy_at(&Encoder->context);
  A_resize(Encoder, 0);
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_amf_SetFrameSize(
  void** EncoderData,
  uint32_t FrameSizeX,
  uint32_t FrameSizeY
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  Encoder->width = FrameSizeX;
  Encoder->height = FrameSizeY;

  AMFSize frameSize = AMFConstructSize(FrameSizeX, FrameSizeY);
  amf::AMFVariant var(frameSize);
  Encoder->encoder->SetProperty(L"FrameSize", var);

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_amf_SetRateControl(
  void** EncoderData,
  ETC_VEDC_EncoderSetting_RateControl_t* RateControl
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  if (RateControl->Type != ETC_VEDC_EncoderSetting_RateControlType_VBR) {
    return ETC_VEDC_Encode_Error_InvalidRateControl;
  }

  Encoder->bitrate = RateControl->VBR.bps;

  amf::AMFVariant var((amf_int64)Encoder->bitrate);
  Encoder->encoder->SetProperty(L"TargetBitrate", var);
  Encoder->encoder->SetProperty(L"PeakBitrate", var);

  return ETC_VEDC_Encode_Error_Success;
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_amf_SetInputFrameRate(
  void** EncoderData,
  f32_t fps
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  Encoder->frameRateNum = (uint32_t)(fps * 1000);

  AMFRate frameRate = AMFConstructRate(Encoder->frameRateNum, Encoder->frameRateDen);
  amf::AMFVariant var(frameRate);
  Encoder->encoder->SetProperty(L"FrameRate", var);

  return ETC_VEDC_Encode_Error_Success;
}

bool
_ETC_VEDC_Encode_Encoder_amf_IsWriteType(
  void** EncoderData,
  ETC_VEDC_Encode_WriteType WriteType
) {
  switch (WriteType) {
  case ETC_VEDC_Encode_WriteType_Frame:
    return true;
  default:
    return false;
  }
}

ETC_VEDC_Encode_Error
_ETC_VEDC_Encode_Encoder_amf_Write(
  void** EncoderData,
  ETC_VEDC_Encode_WriteType WriteType,
  void* WriteData,
  uint8_t Flags
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  switch (WriteType) {
  case ETC_VEDC_Encode_WriteType_Frame: {
    ETC_VEDC_Encode_Frame_t* Frame = (ETC_VEDC_Encode_Frame_t*)WriteData;

    // Create AMF surface from input data
    amf::AMFSurfacePtr surface;
    AMF_RESULT res = Encoder->context->CreateSurfaceFromHostNative(
      Encoder->surfaceFormat,
      Encoder->width,
      Encoder->height,
      Encoder->width,
      Encoder->height,
      Frame->Data[0],
      &surface,
      nullptr
    );

    if (res != AMF_OK) {
      return ETC_VEDC_Encode_Error_EncoderError;
    }

    // Set timestamp
    surface->SetPts(Frame->TimeStamp);

    // Force IDR frame if requested
    if (Flags & ETC_VEDC_EncoderFlag_ResetIDR) {
      amf::AMFVariant var((amf_int64)1); // Force IDR
      surface->SetProperty(L"ForceIDR", var);
    }

    // Submit frame for encoding
    res = Encoder->encoder->SubmitInput(surface);
    if (res != AMF_OK) {
      return ETC_VEDC_Encode_Error_EncoderError;
    }

    // Query for output
    amf::AMFDataPtr data;
    res = Encoder->encoder->QueryOutput(&data);
    if (res == AMF_OK && data != nullptr) {
      amf::AMFBufferPtr buffer(data);
      if (buffer != nullptr) {
        size_t size = buffer->GetSize();
        uint8_t* pData = static_cast<uint8_t*>(buffer->GetNative());

        Encoder->wrd.vPacket.clear();
        Encoder->wrd.vPacket.resize(1);
        Encoder->wrd.vPacket[0].assign(pData, pData + size);
        Encoder->wrd.Readed = false;
      }
    }
    else {
      // No output yet
      Encoder->wrd.vPacket.clear();
      Encoder->wrd.Readed = true;
    }

    return ETC_VEDC_Encode_Error_Success;
  }
  default:
    return ETC_VEDC_Encode_Error_InvalidWriteType;
  }
}

bool
_ETC_VEDC_Encode_Encoder_amf_IsReadable(
  void** EncoderData
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  if (!Encoder->wrd.Readed) {
    return true;
  }

  // Check for any pending output
  amf::AMFDataPtr data;
  AMF_RESULT res = Encoder->encoder->QueryOutput(&data);
  if (res == AMF_OK && data != nullptr) {
    amf::AMFBufferPtr buffer(data);
    if (buffer != nullptr) {
      size_t size = buffer->GetSize();
      uint8_t* pData = static_cast<uint8_t*>(buffer->GetNative());

      Encoder->wrd.vPacket.clear();
      Encoder->wrd.vPacket.resize(1);
      Encoder->wrd.vPacket[0].assign(pData, pData + size);
      Encoder->wrd.Readed = false;
      return true;
    }
  }

  return false;
}

sint32_t
_ETC_VEDC_Encode_Encoder_amf_Read(
  void** EncoderData,
  ETC_VEDC_Encode_PacketInfo* PacketInfo,
  void** Data
) {
  _ETC_VEDC_Encode_Encoder_amf_t* Encoder = (_ETC_VEDC_Encode_Encoder_amf_t*)*EncoderData;

  if (Encoder->wrd.Readed) {
    return 0;
  }

  Encoder->wrd.Readed = true;

  if (Encoder->wrd.vPacket.empty()) {
    return 0;
  }

  if (Encoder->wrd.vPacket.size() != 1) {
    __abort();
  }

  *Data = &Encoder->wrd.vPacket[0][0];
  *PacketInfo = ETC_VEDC_Encode_PacketInfo_Nothing;

  return Encoder->wrd.vPacket[0].size();
}