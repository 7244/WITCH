/*
  AMD AMF decoder implementation for the video decoding framework
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

// AMF decoder component IDs
#ifndef AMFVideoDecoderUVD_H264
#define AMFVideoDecoderUVD_H264 L"AMFVideoDecoderUVD_H264"
#endif

static bool __AMF_DECODER_IS_INITIALIZED = false;
static amf::AMFFactory* __AMF_DECODER_FACTORY = nullptr;
static void* __AMF_DECODER_MODULE = nullptr;

struct _ETC_VEDC_Decoder_Codec_amf_Decoder_t {
  amf::AMFContextPtr context;
  amf::AMFComponentPtr decoder;
  struct {
    uint32_t SizeX;
    uint32_t SizeY;
    bool Readable;
    amf::AMFSurfacePtr surface;
  } wrd;
  amf::AMF_SURFACE_FORMAT outputFormat;
};

// Initialize AMF factory for decoder
static AMF_RESULT InitializeAMFDecoderFactory() {
  if (__AMF_DECODER_IS_INITIALIZED) {
    return AMF_OK;
  }

#ifdef _WIN32
  __AMF_DECODER_MODULE = LoadLibraryA(AMF_DLL_NAMEA);
#else
  __AMF_DECODER_MODULE = dlopen(AMF_DLL_NAMEA, RTLD_LAZY);
#endif

  if (!__AMF_DECODER_MODULE) {
    return AMF_FAIL;
  }

#ifdef _WIN32
  AMFInit_Fn AMFInit = (AMFInit_Fn)GetProcAddress((HMODULE)__AMF_DECODER_MODULE, AMF_INIT_FUNCTION_NAME);
#else
  AMFInit_Fn AMFInit = (AMFInit_Fn)dlsym(__AMF_DECODER_MODULE, AMF_INIT_FUNCTION_NAME);
#endif

  if (!AMFInit) {
    return AMF_FAIL;
  }

  AMF_RESULT res = AMFInit(AMF_FULL_VERSION, &__AMF_DECODER_FACTORY);
  if (res != AMF_OK) {
    return res;
  }

  __AMF_DECODER_IS_INITIALIZED = true;
  return AMF_OK;
}

static const wchar_t* SupportedDecoders[] = {
  // H.264 decoders (in order of preference)
  L"AMFVideoDecoderHW_H264",    // Modern hardware decoder
  L"AMFVideoDecoderUVD_H264",   // Legacy UVD decoder
  L"AMFVideoDecoder_H264",      // Fallback generic decoder

  // H.265 decoders
  L"AMFVideoDecoderHW_H265",
  L"AMFVideoDecoderUVD_H265",
  L"AMFVideoDecoder_H265",

  // AV1 decoder (newer GPUs only)
  L"AMFVideoDecoderHW_AV1",

  nullptr // Terminator
};

// Function to test and report supported decoders
void PrintSupportedAMFDecoders(amf::AMFFactory* factory, amf::AMFContextPtr context) {
  printf("Checking AMD AMF decoder support:\n");

  for (int i = 0; SupportedDecoders[i] != nullptr; i++) {
    amf::AMFComponent* testComponent = nullptr;
    AMF_RESULT res = factory->CreateComponent(context, SupportedDecoders[i], &testComponent);

    if (res == AMF_OK && testComponent != nullptr) {
      printf("  ✓ %ls - SUPPORTED\n", SupportedDecoders[i]);
      testComponent->Terminate();
      testComponent = nullptr;
    }
    else {
      printf("  ✗ %ls - NOT SUPPORTED (error: %d)\n", SupportedDecoders[i], res);
    }
  }
}

// Improved decoder creation with fallback
AMF_RESULT CreateBestAvailableDecoder(amf::AMFFactory* factory,
  amf::AMFContextPtr context,
  const wchar_t* preferredCodec,
  amf::AMFComponent** outComponent) {

  // First try the preferred decoder types for the requested codec
  const wchar_t** decodersToTry = nullptr;

  if (wcsstr(preferredCodec, L"H264") || wcsstr(preferredCodec, L"h264")) {
    static const wchar_t* h264Decoders[] = {
        L"AMFVideoDecoderHW_H264",
        L"AMFVideoDecoderUVD_H264",
        L"AMFVideoDecoder_H264",
        nullptr
    };
    decodersToTry = h264Decoders;
  }
  else if (wcsstr(preferredCodec, L"H265") || wcsstr(preferredCodec, L"h265") ||
    wcsstr(preferredCodec, L"HEVC") || wcsstr(preferredCodec, L"hevc")) {
    static const wchar_t* h265Decoders[] = {
        L"AMFVideoDecoderHW_H265",
        L"AMFVideoDecoderUVD_H265",
        L"AMFVideoDecoder_H265",
        nullptr
    };
    decodersToTry = h265Decoders;
  }
  else if (wcsstr(preferredCodec, L"AV1") || wcsstr(preferredCodec, L"av1")) {
    static const wchar_t* av1Decoders[] = {
        L"AMFVideoDecoderHW_AV1",
        nullptr
    };
    decodersToTry = av1Decoders;
  }

  if (decodersToTry == nullptr) {
    return AMF_NOT_SUPPORTED;
  }

  // Try each decoder in order of preference
  for (int i = 0; decodersToTry[i] != nullptr; i++) {
    AMF_RESULT res = factory->CreateComponent(context, decodersToTry[i], outComponent);
    if (res == AMF_OK && *outComponent != nullptr) {
      printf("Successfully created decoder: %ls\n", decodersToTry[i]);
      return AMF_OK;
    }
    else {
      printf("Failed to create decoder %ls (error: %d)\n", decodersToTry[i], res);
    }
  }

  return AMF_NOT_SUPPORTED;
}


ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_amf_Open(
  void** DecoderData,
  void* DecoderUnique
) {
  // Declare all variables at the top to avoid goto issues
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder;
  amf::AMFContext* pContext = nullptr;
  amf::AMFComponent* pComponent = nullptr;
  AMF_RESULT res;

  // Initialize AMF if not already done
  res = InitializeAMFDecoderFactory();
  if (res != AMF_OK) {
    // AMF initialization failed - probably no AMD hardware or drivers
    return ETC_VEDC_Decoder_Error_DecoderNotFound; // Different error code
  }

  Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)A_resize(NULL, sizeof(_ETC_VEDC_Decoder_Codec_amf_Decoder_t));
  *DecoderData = (void*)Decoder;

  // Zero initialize the entire structure first
  memset(Decoder, 0, sizeof(_ETC_VEDC_Decoder_Codec_amf_Decoder_t));

  // Explicitly construct the smart pointers in place
  std::construct_at(&Decoder->context);
  std::construct_at(&Decoder->decoder);
  std::construct_at(&Decoder->wrd.surface);

  // Create AMF context
  res = __AMF_DECODER_FACTORY->CreateContext(&pContext);
  if (res != AMF_OK) {
    // Context creation failed
    goto cleanup_and_fail;
  }
  Decoder->context = pContext;

  // Initialize context - try DX11 first, then OpenCL
  res = Decoder->context->InitDX11(nullptr);
  if (res != AMF_OK) {
    res = Decoder->context->InitOpenCL(nullptr);
    if (res != AMF_OK) {
      // Both DX11 and OpenCL failed - no compatible graphics context
      goto cleanup_and_fail;
    }
  }

  PrintSupportedAMFDecoders(__AMF_DECODER_FACTORY, Decoder->context);

  // Try to create H.264 decoder using improved method
  res = CreateBestAvailableDecoder(__AMF_DECODER_FACTORY,
    Decoder->context,
    L"H264",
    &pComponent);

  if (res != AMF_OK || pComponent == nullptr) {
    printf("No H.264 decoder available on this system\n");
    goto cleanup_and_fail;
  }

  Decoder->decoder = pComponent;

  // Set minimal properties
  Decoder->outputFormat = amf::AMF_SURFACE_NV12;

  // Initialize decoder with minimal settings
  res = Decoder->decoder->Init(amf::AMF_SURFACE_UNKNOWN, 0, 0);
  if (res != AMF_OK) {
    // Decoder initialization failed - decoder doesn't support the format/settings
    goto cleanup_and_fail;
  }

  // Initialize state
  Decoder->wrd.SizeX = 0;
  Decoder->wrd.SizeY = 0;
  Decoder->wrd.Readable = false;

  return ETC_VEDC_Decoder_Error_OK;

cleanup_and_fail:
  if (Decoder && Decoder->context) {
    Decoder->context->Terminate();
  }
  if (Decoder) {
    std::destroy_at(&Decoder->wrd.surface);
    std::destroy_at(&Decoder->decoder);
    std::destroy_at(&Decoder->context);
    A_resize(Decoder, 0);
  }
  return ETC_VEDC_Decoder_Error_NotImplemented; // Better error code
}

void* _ETC_VEDC_Decoder_Codec_amf_GetUnique(
  void** DecoderData,
  ETC_VEDC_Decoder_UniqueType UniqueType
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;
  return nullptr;
}


void _ETC_VEDC_Decoder_Codec_amf_Close(
  void** DecoderData
) {
  if (!DecoderData || !*DecoderData) {
    return;
  }

  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  if (Decoder->decoder) {
    try {
      Decoder->decoder->Drain();
      Decoder->decoder->Terminate();
    }
    catch (...) {
      printf("Warning: Exception during decoder cleanup\n");
    }
  }

  if (Decoder->context) {
    try {
      Decoder->context->Terminate();
    }
    catch (...) {
      printf("Warning: Exception during context cleanup\n");
    }
  }

  try {
    std::destroy_at(&Decoder->wrd.surface);
  }
  catch (...) {
  }

  try {
    std::destroy_at(&Decoder->decoder);
  }
  catch (...) {
  }

  try {
    std::destroy_at(&Decoder->context);
  }
  catch (...) {
  }

  printf("TODO A_Resize(Decoder, 0) crash\n");
  //A_resize(Decoder, 0);

  *DecoderData = nullptr;
}

sintptr_t _ETC_VEDC_Decoder_Codec_amf_Write(
  void** DecoderData,
  uint8_t* Data,
  uintptr_t Size
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  // Check if decoder is properly initialized
  if (!Decoder->decoder || !Decoder->context) {
    return -ETC_VEDC_Decoder_Error_DecoderError;
  }

  // Create AMF buffer from input data
  amf::AMFBufferPtr buffer;
  AMF_RESULT res = Decoder->context->CreateBufferFromHostNative(Data, Size, &buffer, nullptr);
  if (res != AMF_OK) {
    return -ETC_VEDC_Decoder_Error_DecoderError;
  }

  // Submit data to decoder
  res = Decoder->decoder->SubmitInput(buffer);
  if (res != AMF_OK) {
    return -ETC_VEDC_Decoder_Error_DecoderError;
  }

  // Check for output
  amf::AMFDataPtr outputData;
  res = Decoder->decoder->QueryOutput(&outputData);
  if (res == AMF_OK && outputData != nullptr) {
    amf::AMFSurfacePtr surface(outputData);
    if (surface != nullptr) {
      // Store decoded surface
      Decoder->wrd.surface = surface;
      // Get dimensions from plane 0 (Y plane)
      amf::AMFPlanePtr plane0 = surface->GetPlaneAt(0);
      if (plane0) {
        Decoder->wrd.SizeX = plane0->GetWidth();
        Decoder->wrd.SizeY = plane0->GetHeight();
      }
      else {
        // Fallback - use format info if available
        Decoder->wrd.SizeX = 1920; // Will be updated when we get actual surface
        Decoder->wrd.SizeY = 1080;
      }
      Decoder->wrd.Readable = true;
    }
  }

  return Size;
}

bool _ETC_VEDC_Decoder_Codec_amf_IsReadable(
  void** DecoderData
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  // Safety check - ensure decoder is properly initialized
  if (!Decoder->decoder || !Decoder->context) {
    return false;
  }

  // Check if we have decoded data ready
  if (Decoder->wrd.Readable) {
    return true;
  }

  // Check for new output
  amf::AMFDataPtr outputData;
  AMF_RESULT res = Decoder->decoder->QueryOutput(&outputData);
  if (res == AMF_OK && outputData != nullptr) {
    amf::AMFSurfacePtr surface(outputData);
    if (surface != nullptr) {
      Decoder->wrd.surface = surface;
      // Get dimensions from plane 0 (Y plane)
      amf::AMFPlanePtr plane0 = surface->GetPlaneAt(0);
      if (plane0) {
        Decoder->wrd.SizeX = plane0->GetWidth();
        Decoder->wrd.SizeY = plane0->GetHeight();
      }
      Decoder->wrd.Readable = true;
      return true;
    }
  }

  return false;
}

bool _ETC_VEDC_Decoder_Codec_amf_IsReadType(
  void** DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType
) {
  switch (ReadType) {
  case ETC_VEDC_Decoder_ReadType_Frame: {
    return true; // AMF surfaces via standard Frame type
  }
  default: {
    return false;
  }
  }
}

void _ETC_VEDC_Decoder_Codec_amf_GetReadImageProperties(
  void** DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t* ImageProperties
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  switch (ReadType) {
  case ETC_VEDC_Decoder_ReadType_Frame: {
    if (Decoder->wrd.surface) {
      ImageProperties->PixelFormat = PIXF_YUVNV12;
      ImageProperties->SizeX = Decoder->wrd.SizeX;
      ImageProperties->SizeY = Decoder->wrd.SizeY;

      // Safely get stride info - check if planes exist
      amf::AMFPlanePtr planeY = Decoder->wrd.surface->GetPlaneAt(0);
      amf::AMFPlanePtr planeUV = Decoder->wrd.surface->GetPlaneAt(1);

      if (planeY && planeUV) {
        ImageProperties->Stride[0] = planeY->GetHPitch();
        ImageProperties->Stride[1] = planeUV->GetHPitch();
      }
      else {
        // Fallback stride calculation
        ImageProperties->Stride[0] = Decoder->wrd.SizeX;
        ImageProperties->Stride[1] = Decoder->wrd.SizeX;
      }
      ImageProperties->Stride[2] = 0;
      ImageProperties->Stride[3] = 0;
    }
    return;
  }
  default: {
    return;
  }
  }
}

ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_amf_Read(
  void** DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void* ReadData
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  if (!Decoder->wrd.Readable) {
    return ETC_VEDC_Decoder_Error_NotReadable;
  }

  switch (ReadType) {
  case ETC_VEDC_Decoder_ReadType_Frame: {
    // Use standard Frame type for AMF surfaces
    ETC_VEDC_Decoder_Frame_t* Frame = (ETC_VEDC_Decoder_Frame_t*)ReadData;

    if (Decoder->wrd.surface) {
      // Store AMF surface pointer in Data[0]
      Frame->Data[0] = (void*)Decoder->wrd.surface.GetPtr();
      Frame->Data[1] = nullptr; // Not used for AMF
      Frame->Data[2] = nullptr;
      Frame->Data[3] = nullptr;

      // Set properties
      Frame->Properties.PixelFormat = PIXF_YUVNV12;
      Frame->Properties.SizeX = Decoder->wrd.SizeX;
      Frame->Properties.SizeY = Decoder->wrd.SizeY;

      // Safely get stride info - check if planes exist
      amf::AMFPlanePtr planeY = Decoder->wrd.surface->GetPlaneAt(0);
      amf::AMFPlanePtr planeUV = Decoder->wrd.surface->GetPlaneAt(1);

      if (planeY && planeUV) {
        Frame->Properties.Stride[0] = planeY->GetHPitch();
        Frame->Properties.Stride[1] = planeUV->GetHPitch();
      }
      else {
        // Fallback stride calculation if planes are invalid
        Frame->Properties.Stride[0] = Decoder->wrd.SizeX;
        Frame->Properties.Stride[1] = Decoder->wrd.SizeX;
      }
      Frame->Properties.Stride[2] = 0;
      Frame->Properties.Stride[3] = 0;

      // Keep surface alive until ReadClear
      Decoder->wrd.Readable = false;

      return ETC_VEDC_Decoder_Error_OK;
    }
    return ETC_VEDC_Decoder_Error_DecoderError;
  }
  default: {
    return ETC_VEDC_Decoder_Error_InvalidReadType;
  }
  }
}

void _ETC_VEDC_Decoder_Codec_amf_ReadClear(
  void** DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void* ReadData
) {
  _ETC_VEDC_Decoder_Codec_amf_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_amf_Decoder_t*)*DecoderData;

  switch (ReadType) {
  case ETC_VEDC_Decoder_ReadType_Frame: {
    // Clear the surface reference
    Decoder->wrd.surface = nullptr;
    break;
  }
  }
}