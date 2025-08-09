/*
  File: Decoder/Codec/va/va.h
  Fixed VAAPI decoder implementation for the video decoding framework
*/

#pragma once

// Include VAAPI headers first
#include <va/va.h>
#include <va/va_drm.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <vector>

// VAAPI decoder structure
struct _ETC_VEDC_Decoder_Codec_va_Decoder_t {
    VADisplay display;
    VAContextID context;
    VAConfigID config;
    
    // DRM device
    int drmFd;
    
    // Decoder state
    VAProfile profile;
    VAEntrypoint entrypoint;
    std::vector<VASurfaceID> surfaces;
    VASurfaceID currentOutputSurface;
    int numSurfaces;
    
    // Video properties
    int pictureWidth;
    int pictureHeight;
    bool initialized;
    
    struct {
        uint32_t SizeX;
        uint32_t SizeY;
        bool Readable;
        VASurfaceID readySurface;
    } wrd;
};

// Helper function to find DRM device
static int FindDRMDevice() {
    const char* drmPaths[] = {
        "/dev/dri/renderD128",
        "/dev/dri/renderD129", 
        "/dev/dri/card0",
        "/dev/dri/card1",
        nullptr
    };
    
    for (int i = 0; drmPaths[i]; i++) {
        int fd = open(drmPaths[i], O_RDWR);
        if (fd >= 0) {
            return fd;
        }
    }
    return -1;
}

// Check if profile is supported
static bool IsProfileSupported(VADisplay display, VAProfile profile) {
    int numProfiles = vaMaxNumProfiles(display);
    std::vector<VAProfile> profiles(numProfiles);
    
    VAStatus status = vaQueryConfigProfiles(display, profiles.data(), &numProfiles);
    if (status != VA_STATUS_SUCCESS) {
        return false;
    }
    
    for (int i = 0; i < numProfiles; i++) {
        if (profiles[i] == profile) {
            return true;
        }
    }
    return false;
}

// Create decoder context for specific dimensions
static ETC_VEDC_Decoder_Error CreateDecoderContext(
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder,
    int width, int height
) {
    VAStatus status;
    
    if (Decoder->initialized && 
        Decoder->pictureWidth == width && 
        Decoder->pictureHeight == height) {
        return ETC_VEDC_Decoder_Error_OK;
    }
    
    // Clean up existing context
    if (Decoder->context != VA_INVALID_ID) {
        vaDestroyContext(Decoder->display, Decoder->context);
        Decoder->context = VA_INVALID_ID;
    }
    
    if (Decoder->config != VA_INVALID_ID) {
        vaDestroyConfig(Decoder->display, Decoder->config);
        Decoder->config = VA_INVALID_ID;
    }
    
    if (!Decoder->surfaces.empty()) {
        vaDestroySurfaces(Decoder->display, Decoder->surfaces.data(), Decoder->surfaces.size());
        Decoder->surfaces.clear();
    }
    
    // Create config
    VAConfigAttrib attrib;
    attrib.type = VAConfigAttribRTFormat;
    attrib.value = VA_RT_FORMAT_YUV420;
    
    status = vaCreateConfig(Decoder->display, Decoder->profile, Decoder->entrypoint,
                           &attrib, 1, &Decoder->config);
    if (status != VA_STATUS_SUCCESS) {
        return ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Create surfaces
    Decoder->numSurfaces = 16;
    Decoder->surfaces.resize(Decoder->numSurfaces);
    
    status = vaCreateSurfaces(Decoder->display, VA_RT_FORMAT_YUV420,
                             width, height,
                             Decoder->surfaces.data(), Decoder->numSurfaces,
                             nullptr, 0);
    if (status != VA_STATUS_SUCCESS) {
        return ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Create context
    status = vaCreateContext(Decoder->display, Decoder->config,
                            width, height, VA_PROGRESSIVE,
                            Decoder->surfaces.data(), Decoder->numSurfaces,
                            &Decoder->context);
    if (status != VA_STATUS_SUCCESS) {
        return ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    Decoder->pictureWidth = width;
    Decoder->pictureHeight = height;
    Decoder->initialized = true;
    
    return ETC_VEDC_Decoder_Error_OK;
}

void _ETC_VEDC_Decoder_Codec_va_Close(
  void** DecoderData
);

ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_va_Open(
    void** DecoderData,
    void* DecoderUnique
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder;
    VAStatus status;
    int major, minor;
    
    // Allocate decoder structure
    Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)A_resize(NULL, 
               sizeof(_ETC_VEDC_Decoder_Codec_va_Decoder_t));
    if (!Decoder) {
        return ETC_VEDC_Decoder_Error_DecoderAllocateFail;
    }
    
    *DecoderData = (void*)Decoder;
    memset(Decoder, 0, sizeof(_ETC_VEDC_Decoder_Codec_va_Decoder_t));
    
    // Initialize defaults
    Decoder->drmFd = -1;
    Decoder->display = nullptr;
    Decoder->context = VA_INVALID_ID;
    Decoder->config = VA_INVALID_ID;
    Decoder->currentOutputSurface = VA_INVALID_SURFACE;
    Decoder->wrd.readySurface = VA_INVALID_SURFACE;
    
    // Open DRM device
    Decoder->drmFd = FindDRMDevice();
    if (Decoder->drmFd < 0) {
        goto cleanup_and_fail;
    }
    
    // Get VAAPI display
    Decoder->display = vaGetDisplayDRM(Decoder->drmFd);
    if (!Decoder->display) {
        goto cleanup_and_fail;
    }
    
    // Initialize VAAPI
    status = vaInitialize(Decoder->display, &major, &minor);
    if (status != VA_STATUS_SUCCESS) {
        goto cleanup_and_fail;
    }
    
    // Try to find H.264 profile - declare array at the beginning to avoid goto issues
    {
        VAProfile preferredProfiles[] = {
            VAProfileH264High,
            VAProfileH264Main,
            VAProfileH264Baseline,
            VAProfileH264ConstrainedBaseline
        };
        
        Decoder->profile = VAProfileNone;
        for (size_t i = 0; i < sizeof(preferredProfiles) / sizeof(preferredProfiles[0]); i++) {
            if (IsProfileSupported(Decoder->display, preferredProfiles[i])) {
                Decoder->profile = preferredProfiles[i];
                break;
            }
        }
        
        if (Decoder->profile == VAProfileNone) {
            goto cleanup_and_fail;
        }
    }
    
    Decoder->entrypoint = VAEntrypointVLD;
    
    return ETC_VEDC_Decoder_Error_OK;

cleanup_and_fail:
    _ETC_VEDC_Decoder_Codec_va_Close(DecoderData);
    return ETC_VEDC_Decoder_Error_DecoderNotFound;
}

void* _ETC_VEDC_Decoder_Codec_va_GetUnique(
    void** DecoderData,
    ETC_VEDC_Decoder_UniqueType UniqueType
) {
    // Return nullptr for now - no unique data needed
    return nullptr;
}

void _ETC_VEDC_Decoder_Codec_va_Close(
    void** DecoderData
) {
    if (!DecoderData || !*DecoderData) {
        return;
    }
    
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    
    // Clean up VAAPI resources
    if (Decoder->display) {
        if (Decoder->context != VA_INVALID_ID) {
            vaDestroyContext(Decoder->display, Decoder->context);
        }
        
        if (Decoder->config != VA_INVALID_ID) {
            vaDestroyConfig(Decoder->display, Decoder->config);
        }
        
        if (!Decoder->surfaces.empty()) {
            vaDestroySurfaces(Decoder->display, Decoder->surfaces.data(), Decoder->surfaces.size());
        }
        
        vaTerminate(Decoder->display);
    }
    
    // Close DRM device
    if (Decoder->drmFd >= 0) {
        close(Decoder->drmFd);
    }
    
    // Free decoder memory
    A_resize(Decoder, 0);
    *DecoderData = nullptr;
}

sintptr_t _ETC_VEDC_Decoder_Codec_va_Write(
    void** DecoderData,
    uint8_t* Data,
    uintptr_t Size
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    VAStatus status;
    
    if (!Decoder || !Decoder->display) {
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Initialize decoder context if needed (use default dimensions)
    if (!Decoder->initialized) {
        ETC_VEDC_Decoder_Error result = CreateDecoderContext(Decoder, 1920, 1080);
        if (result != ETC_VEDC_Decoder_Error_OK) {
            return -result;
        }
    }
    
    // Create buffer for input data
    VABufferID dataBuffer;
    status = vaCreateBuffer(Decoder->display, Decoder->context,
                           VASliceDataBufferType, Size, 1,
                           Data, &dataBuffer);
    if (status != VA_STATUS_SUCCESS) {
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Select output surface
    static int surfaceIndex = 0;
    VASurfaceID outputSurface = Decoder->surfaces[surfaceIndex % Decoder->numSurfaces];
    surfaceIndex++;
    
    // Begin picture
    status = vaBeginPicture(Decoder->display, Decoder->context, outputSurface);
    if (status != VA_STATUS_SUCCESS) {
        vaDestroyBuffer(Decoder->display, dataBuffer);
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Render picture
    status = vaRenderPicture(Decoder->display, Decoder->context, &dataBuffer, 1);
    if (status != VA_STATUS_SUCCESS) {
        vaEndPicture(Decoder->display, Decoder->context);
        vaDestroyBuffer(Decoder->display, dataBuffer);
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // End picture
    status = vaEndPicture(Decoder->display, Decoder->context);
    if (status != VA_STATUS_SUCCESS) {
        vaDestroyBuffer(Decoder->display, dataBuffer);
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Sync surface
    status = vaSyncSurface(Decoder->display, outputSurface);
    if (status != VA_STATUS_SUCCESS) {
        vaDestroyBuffer(Decoder->display, dataBuffer);
        return -ETC_VEDC_Decoder_Error_DecoderError;
    }
    
    // Clean up buffer
    vaDestroyBuffer(Decoder->display, dataBuffer);
    
    // Store result
    Decoder->currentOutputSurface = outputSurface;
    Decoder->wrd.readySurface = outputSurface;
    Decoder->wrd.SizeX = Decoder->pictureWidth;
    Decoder->wrd.SizeY = Decoder->pictureHeight;
    Decoder->wrd.Readable = true;
    
    return Size;
}

bool _ETC_VEDC_Decoder_Codec_va_IsReadable(
    void** DecoderData
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    
    return Decoder && Decoder->wrd.Readable && 
           Decoder->wrd.readySurface != VA_INVALID_SURFACE;
}

bool _ETC_VEDC_Decoder_Codec_va_IsReadType(
    void** DecoderData,
    ETC_VEDC_Decoder_ReadType ReadType
) {
    switch (ReadType) {
        case ETC_VEDC_Decoder_ReadType_Frame:
            return true;
        default:
            return false;
    }
}

void _ETC_VEDC_Decoder_Codec_va_GetReadImageProperties(
    void** DecoderData,
    ETC_VEDC_Decoder_ReadType ReadType,
    ETC_VEDC_Decoder_ImageProperties_t* ImageProperties
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    
    if (ReadType == ETC_VEDC_Decoder_ReadType_Frame && Decoder && Decoder->wrd.Readable) {
        ImageProperties->PixelFormat = PIXF_YUVNV12;
        ImageProperties->SizeX = Decoder->wrd.SizeX;
        ImageProperties->SizeY = Decoder->wrd.SizeY;
        ImageProperties->Stride[0] = Decoder->wrd.SizeX;
        ImageProperties->Stride[1] = Decoder->wrd.SizeX;
        ImageProperties->Stride[2] = 0;
        ImageProperties->Stride[3] = 0;
    }
}

ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_va_Read(
    void** DecoderData,
    ETC_VEDC_Decoder_ReadType ReadType,
    void* ReadData
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    
    if (!Decoder->wrd.Readable) {
        return ETC_VEDC_Decoder_Error_NotReadable;
    }
    
    if (ReadType == ETC_VEDC_Decoder_ReadType_Frame) {
        ETC_VEDC_Decoder_Frame_t* Frame = (ETC_VEDC_Decoder_Frame_t*)ReadData;
        
        // Store VAAPI surface ID in Data[0]
        Frame->Data[0] = (void*)(uintptr_t)Decoder->wrd.readySurface;
        Frame->Data[1] = (void*)Decoder->display;
        Frame->Data[2] = nullptr;
        Frame->Data[3] = nullptr;
        
        // Set properties
        Frame->Properties.PixelFormat = PIXF_YUVNV12;
        Frame->Properties.SizeX = Decoder->wrd.SizeX;
        Frame->Properties.SizeY = Decoder->wrd.SizeY;
        Frame->Properties.Stride[0] = Decoder->wrd.SizeX;
        Frame->Properties.Stride[1] = Decoder->wrd.SizeX;
        Frame->Properties.Stride[2] = 0;
        Frame->Properties.Stride[3] = 0;
        
        Decoder->wrd.Readable = false;
        
        return ETC_VEDC_Decoder_Error_OK;
    }
    
    return ETC_VEDC_Decoder_Error_InvalidReadType;
}

void _ETC_VEDC_Decoder_Codec_va_ReadClear(
    void** DecoderData,
    ETC_VEDC_Decoder_ReadType ReadType,
    void* ReadData
) {
    _ETC_VEDC_Decoder_Codec_va_Decoder_t* Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t*)*DecoderData;
    
    if (ReadType == ETC_VEDC_Decoder_ReadType_Frame) {
        Decoder->wrd.readySurface = VA_INVALID_SURFACE;
    }
}