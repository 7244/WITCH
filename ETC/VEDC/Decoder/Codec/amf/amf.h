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

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")  // This contains the GUID definitions
#pragma comment(lib, "mfplat.lib")  // For Media Foundation
#pragma comment(lib, "mfuuid.lib")  // For Media Foundation UUIDs

#include <d3d11.h>
#include <d3d9.h>
#include <dxva2api.h>

void IdentifyDecoderProfile(const GUID& profile, int index) {
    printf("Profile %d: ", index);
    
    // H.264 Profiles
    if (IsEqualGUID(profile, {0x1b81be68, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("H.264 VLD NoFGT\n");
    } else if (IsEqualGUID(profile, {0x1b81be69, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("H.264 VLD FGT\n");
    } else if (IsEqualGUID(profile, {0x1b81bea3, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("H.264 VLD Stereo Progressive No FGT\n");
    } else if (IsEqualGUID(profile, {0x1b81bea4, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("H.264 VLD Stereo No FGT\n");
    }
    // HEVC/H.265 Profiles
    else if (IsEqualGUID(profile, {0x5b11d51b, 0x2f4c, 0x4452, {0xbc, 0xc3, 0x09, 0xf2, 0xa1, 0x16, 0x0c, 0xc0}})) {
        printf("HEVC Main\n");
    } else if (IsEqualGUID(profile, {0x107af0e0, 0xef1a, 0x4d19, {0xab, 0xa8, 0x67, 0xa1, 0x63, 0x07, 0x3d, 0x13}})) {
        printf("HEVC Main 10\n");
    } else if (IsEqualGUID(profile, {0x463707f8, 0xa1d0, 0x4585, {0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e}})) {
        printf("HEVC Main 10 (Alt)\n");
    } else if (IsEqualGUID(profile, {0xa4c749ef, 0x6ecf, 0x48aa, {0x84, 0x48, 0x50, 0xa7, 0xa1, 0x16, 0x5f, 0xf7}})) {
        printf("HEVC Main Still Picture\n");
    }
    // MPEG-2 Profiles  
    else if (IsEqualGUID(profile, {0x86695f12, 0x340e, 0x4f04, {0x9f, 0xd3, 0x92, 0x53, 0xdd, 0x32, 0x74, 0x60}})) {
        printf("MPEG-2 VLD\n");
    } else if (IsEqualGUID(profile, {0xee27417f, 0x5e28, 0x4e65, {0xbe, 0xea, 0x1d, 0x26, 0xb5, 0x08, 0xad, 0xc9}})) {
        printf("MPEG-2 & VC-1 VLD\n");
    }
    // VC-1 Profiles
    else if (IsEqualGUID(profile, {0x6f3ec719, 0x3735, 0x42cc, {0x80, 0x63, 0x65, 0xcc, 0x3c, 0xb3, 0x66, 0x16}})) {
        printf("VC-1 Main\n");
    } else if (IsEqualGUID(profile, {0x1b81bea0, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("VC-1 PostProc\n");
    } else if (IsEqualGUID(profile, {0x1b81bea1, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("VC-1 MoComp\n");
    } else if (IsEqualGUID(profile, {0x1b81bea2, 0xa0c7, 0x11d3, {0xb9, 0x84, 0x00, 0xc0, 0x4f, 0x2e, 0x73, 0xc5}})) {
        printf("VC-1 IDCT\n");
    }
    // AV1 Profiles (Your system shows 0, but just in case)
    else if (IsEqualGUID(profile, {0xb8be4ccb, 0xcf53, 0x46ba, {0x8d, 0x59, 0xd6, 0xb8, 0xa6, 0xda, 0x5d, 0x2a}})) {
        printf("AV1 Profile 0\n");
    } else if (IsEqualGUID(profile, {0x6936ff0f, 0x45b1, 0x4163, {0x9c, 0xc1, 0x64, 0x6e, 0xf6, 0x94, 0x61, 0x08}})) {
        printf("AV1 Profile 1\n");
    }
    // Additional common profiles
    else if (IsEqualGUID(profile, {0x9947ec6f, 0x689b, 0x11dc, {0xa3, 0x20, 0x00, 0x19, 0xdb, 0xbc, 0x41, 0x84}})) {
        printf("H.264 SVC VLD\n");
    } else if (IsEqualGUID(profile, {0x32fcfe3f, 0xde46, 0x4a49, {0x86, 0x1b, 0xac, 0x71, 0x11, 0x06, 0x49, 0xd5}})) {
        printf("JPEG VLD\n");
    } else if (IsEqualGUID(profile, {0x33fcfe41, 0xde46, 0x4a49, {0x86, 0x1b, 0xac, 0x71, 0x11, 0x06, 0x49, 0xd5}})) {
        printf("Motion JPEG VLD\n");
    }
    // Unknown profile
    else {
        wchar_t guidStr[40];
        StringFromGUID2(profile, guidStr, 40);
        printf("Unknown GUID: %ls\n", guidStr);
    }
}

void EnumerateWindowsHardwareDecoders() {
    ID3D11Device* device = nullptr;
    ID3D11VideoDevice* videoDevice = nullptr;
    
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_VIDEO_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        &device, nullptr, nullptr);
    
    if (SUCCEEDED(hr)) {
        hr = device->QueryInterface(&videoDevice);
        if (SUCCEEDED(hr)) {
            UINT profileCount = videoDevice->GetVideoDecoderProfileCount();
            printf("Found %u decoder profiles:\n", profileCount);
            
            // Summary counters
            int h264Count = 0, hevcCount = 0, av1Count = 0, mpeg2Count = 0, vc1Count = 0, otherCount = 0;
            
            for (UINT i = 0; i < profileCount; i++) {
                GUID profile;
                hr = videoDevice->GetVideoDecoderProfile(i, &profile);
                if (SUCCEEDED(hr)) {
                    IdentifyDecoderProfile(profile, i);
                    
                    // Count codec types
                    wchar_t guidStr[40];
                    StringFromGUID2(profile, guidStr, 40);
                    std::wstring guidString(guidStr);
                    
                    if (guidString.find(L"1B81BE6") != std::wstring::npos || 
                        guidString.find(L"1B81BEA") != std::wstring::npos) {
                        h264Count++;
                    } else if (guidString.find(L"5B11D51B") != std::wstring::npos || 
                               guidString.find(L"107AF0E0") != std::wstring::npos ||
                               guidString.find(L"463707F8") != std::wstring::npos ||
                               guidString.find(L"A4C749EF") != std::wstring::npos) {
                        hevcCount++;
                    } else if (guidString.find(L"B8BE4CCB") != std::wstring::npos ||
                               guidString.find(L"6936FF0F") != std::wstring::npos) {
                        av1Count++;
                    } else if (guidString.find(L"86695F12") != std::wstring::npos) {
                        mpeg2Count++;
                    } else if (guidString.find(L"1B81BEA0") != std::wstring::npos ||
                               guidString.find(L"1B81BEA1") != std::wstring::npos ||
                               guidString.find(L"1B81BEA2") != std::wstring::npos) {
                        vc1Count++;
                    } else {
                        otherCount++;
                    }
                }
            }
            
            // Print summary
            printf("\n=== DECODER SUMMARY ===\n");
            printf("H.264 profiles: %d\n", h264Count);
            printf("HEVC/H.265 profiles: %d\n", hevcCount);
            printf("AV1 profiles: %d\n", av1Count);
            printf("MPEG-2 profiles: %d\n", mpeg2Count);
            printf("VC-1 profiles: %d\n", vc1Count);
            printf("Other/Unknown profiles: %d\n", otherCount);
            printf("Total profiles: %d\n", profileCount);
            
            videoDevice->Release();
        } else {
            printf("Failed to get video device interface\n");
        }
        device->Release();
    } else {
        printf("Failed to create D3D11 device with video support\n");
    }
}
// Add this function to your decoder code
void EnumerateAllAMFComponents(amf::AMFFactory* factory, amf::AMFContextPtr context) {
    printf("=== Enumerating ALL AMF Components ===\n");
    
    // Use the SAME naming convention as your encoder
    const wchar_t* allPossibleComponents[] = {
        // Try encoder-style naming for decoders
        L"AMFVideoDecoderVCE_AVC",        // Match your encoder style
        L"AMFVideoDecoderVCE_H264",       
        L"AMFVideoDecoderVCN_AVC",        // VCN variants
        L"AMFVideoDecoderVCN_H264",
        
        // Your current decoder attempts
        L"AMFVideoDecoderHW_H264",
        L"AMFVideoDecoderUVD_H264", 
        L"AMFVideoDecoder_H264",
        
        // Generic variants
        L"AMFVideoDecoder",
        L"AMFVideoDecoderAVC",
        
        // AV1 (which works)
        L"AMFVideoDecoderHW_AV1",
        L"AMFVideoDecoderVCN_AV1",
        
        // For comparison - try some encoder names to see what's available
        L"AMFVideoEncoderVCE_AVC",        // Your working encoder
        L"AMFVideoEncoderHW_H264",
        L"AMFVideoEncoderVCN_H264",
        
        nullptr
    };
    
    for (int i = 0; allPossibleComponents[i] != nullptr; i++) {
        amf::AMFComponent* testComponent = nullptr;
        AMF_RESULT res = factory->CreateComponent(context, allPossibleComponents[i], &testComponent);
        
        if (res == AMF_OK && testComponent != nullptr) {
            printf("  ✓ %ls - AVAILABLE\n", allPossibleComponents[i]);
            testComponent->Terminate();
        } else {
            printf("  ✗ %ls - NOT AVAILABLE (error: %d)\n", allPossibleComponents[i], res);
        }
    }
    printf("=== End Component Enumeration ===\n");
}

// Improved decoder creation with fallback
AMF_RESULT CreateBestAvailableDecoder(amf::AMFFactory* factory,
  amf::AMFContextPtr context,
  const wchar_t* preferredCodec,
  amf::AMFComponent** outComponent) {
  EnumerateWindowsHardwareDecoders();
  // First try the preferred decoder types for the requested codec
  const wchar_t** decodersToTry = nullptr;

  if (wcsstr(preferredCodec, L"H264") || wcsstr(preferredCodec, L"h264")) {
    static const wchar_t* h264Decoders[] = {
        // Try VCE style first (matches your encoder)
        L"AMFVideoDecoderVCE_AVC",
        L"AMFVideoDecoderVCE_H264",
        
        // Try VCN variants (newer AMD GPUs)
        L"AMFVideoDecoderVCN_AVC", 
        L"AMFVideoDecoderVCN_H264",
        
        // Your original attempts
        L"AMFVideoDecoderHW_H264",
        L"AMFVideoDecoderUVD_H264",
        L"AMFVideoDecoder_H264",
        
        // Generic fallbacks
        L"AMFVideoDecoder",
        L"AMFVideoDecoderAVC",
        
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