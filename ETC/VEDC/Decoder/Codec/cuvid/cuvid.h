#include _WITCH_PATH(include/cuda.h)
#include _WITCH_PATH(include/cuvid.h)

struct _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t{
  CUcontext Context;
  CUvideodecoder Decoder;
  CUdevice Device;
  CUvideoparser Parser;
  struct{
    uint32_t SizeX;
    uint32_t SizeY;
    bool Readable;
    CUVIDPARSERDISPINFO Info;
  }wrd;
};

int _ETC_VEDC_Decoder_Codec_cuvid_GetNumDecodeSurfaces(cudaVideoCodec eCodec, unsigned int nWidth, unsigned int nHeight){
  if (eCodec == cudaVideoCodec_VP9) {
    return 12;
  }
  if (eCodec == cudaVideoCodec_H264 || eCodec == cudaVideoCodec_H264_SVC || eCodec == cudaVideoCodec_H264_MVC) {
    return 20;
  }
  if (eCodec == cudaVideoCodec_HEVC) {
    int MaxLumaPS = 35651584;
    int MaxDpbPicBuf = 6;
    int PicSizeInSamplesY = (int)(nWidth * nHeight);
    int MaxDpbSize;
    if (PicSizeInSamplesY <= (MaxLumaPS >> 2)){
      MaxDpbSize = MaxDpbPicBuf * 4;
    }
    else if (PicSizeInSamplesY <= (MaxLumaPS >> 1)){
      MaxDpbSize = MaxDpbPicBuf * 2;
    }
    else if (PicSizeInSamplesY <= ((3 * MaxLumaPS) >> 2)){
      MaxDpbSize = (MaxDpbPicBuf * 4) / 3;
    }
    else{
      MaxDpbSize = MaxDpbPicBuf;
    }
    return (MaxDpbSize < 16 ? MaxDpbSize : 16) + 4;
  }

  return 8;
}

int _ETC_VEDC_Decoder_Codec_cuvid_Sequence_cb(void *user, CUVIDEOFORMAT *fmt){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)user;

  Decoder->wrd.SizeX = fmt->coded_width;
  Decoder->wrd.SizeY = fmt->coded_height;

  int nDecodeSurface = _ETC_VEDC_Decoder_Codec_cuvid_GetNumDecodeSurfaces(fmt->codec, fmt->coded_width, fmt->coded_height);

  if(Decoder->Decoder != NULL){
    if(cuvidDestroyDecoder(Decoder->Decoder) != CUDA_SUCCESS){
      PR_abort();
    }
  }

  CUVIDDECODECREATEINFO create_info = {0};
  create_info.CodecType = fmt->codec;
  create_info.ChromaFormat = fmt->chroma_format;
  create_info.OutputFormat = fmt->bit_depth_luma_minus8 ? cudaVideoSurfaceFormat_P016 : cudaVideoSurfaceFormat_NV12;
  create_info.bitDepthMinus8 = fmt->bit_depth_luma_minus8;
  create_info.DeinterlaceMode = cudaVideoDeinterlaceMode_Weave;
  create_info.ulNumOutputSurfaces = 1;

  create_info.ulCreationFlags = cudaVideoCreate_PreferCUVID;
  create_info.ulNumDecodeSurfaces = nDecodeSurface;

  create_info.ulWidth = fmt->coded_width;
  create_info.ulHeight = fmt->coded_height;
  create_info.ulMaxWidth = create_info.ulWidth;
  create_info.ulMaxHeight = create_info.ulHeight;
  create_info.ulTargetWidth = create_info.ulWidth;
  create_info.ulTargetHeight = create_info.ulHeight;

  if(cuvidCreateDecoder(&Decoder->Decoder, &create_info) != CUDA_SUCCESS){
    PR_abort();
  }

  return nDecodeSurface;
}

int _ETC_VEDC_Decoder_Codec_cuvid_Decode_cb(void *user, CUVIDPICPARAMS *pic) {
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)user;

  if (Decoder->Decoder == NULL) {
    PR_abort();
  }

  if(cuvidDecodePicture(Decoder->Decoder, pic) != CUDA_SUCCESS){
    PR_abort();
  }

  return 1;
}

int _ETC_VEDC_Decoder_Codec_cuvid_Display_cb(void *user, CUVIDPARSERDISPINFO *info) {
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t*)user;

  Decoder->wrd.Readable = 1;
  Decoder->wrd.Info = *info;

  return 1;
}

ETC_VEDC_Decoder_Error _ETC_VEDC_Decoder_Codec_cuvid_Open(
  void **DecoderData,
  void *DecoderUnique
){
  if(__GPU_IS_CUDA_INITED == 0){
    if(cuInit(0) != CUDA_SUCCESS){
      return ETC_VEDC_Decoder_Error_CudaInit;
    }
    __GPU_IS_CUDA_INITED = 1;
  }

  {
    int device_count;
    if(cuDeviceGetCount(&device_count) != CUDA_SUCCESS){
      PR_abort();
    }
    if(device_count == 0){
      return ETC_VEDC_Decoder_Error_CudaNoDevice;
    }
  }

  ETC_VEDC_Decoder_Error Error = ETC_VEDC_Decoder_Error_Unknown;

  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder =
    (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)A_resize(NULL, sizeof(_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t));
  *DecoderData = (void *)Decoder;

  if(cuDeviceGet(&Decoder->Device, 0) != CUDA_SUCCESS){
    Error = ETC_VEDC_Decoder_Error_CudaDeviceGet;
    goto gt_err0;
  }

  if(cuCtxCreate(&Decoder->Context, 0, Decoder->Device) != CUDA_SUCCESS){
    Error = ETC_VEDC_Decoder_Error_CudaContextCreate;
    goto gt_err0;
  }

  {
    CUVIDPARSERPARAMS p = {.CodecType = cudaVideoCodec_H264};
    p.ulMaxNumDecodeSurfaces = 1;
    p.ulErrorThreshold = 0;
    p.ulMaxDisplayDelay = 0;
    p.pUserData = (void*)Decoder;
    p.pfnSequenceCallback = _ETC_VEDC_Decoder_Codec_cuvid_Sequence_cb;
    p.pfnDecodePicture = _ETC_VEDC_Decoder_Codec_cuvid_Decode_cb;
    p.pfnDisplayPicture = _ETC_VEDC_Decoder_Codec_cuvid_Display_cb;

    if(cuvidCreateVideoParser(&Decoder->Parser, &p) != CUDA_SUCCESS){
      Error = ETC_VEDC_Decoder_Error_ParserCreate;
      goto gt_err1;
    }
  }

  Decoder->Decoder = NULL;
  Decoder->wrd.Readable = 0;

  return ETC_VEDC_Decoder_Error_OK;

  gt_err1:
  cuCtxDestroy(Decoder->Context);
  gt_err0:
  A_resize(Decoder, 0);
  return Error;
}

void *_ETC_VEDC_Decoder_Codec_cuvid_GetUnique(
  void **DecoderData,
  ETC_VEDC_Decoder_UniqueType UniqueType
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  switch(UniqueType){
    case ETC_VEDC_Decoder_UniqueType_CudaContext:{
      return (void *)Decoder->Context;
    }
  }
  return NULL;
}

void _ETC_VEDC_Decoder_Codec_cuvid_Close(
  void **DecoderData
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  if(Decoder->Decoder != NULL){
    cuvidDestroyDecoder(Decoder->Decoder);
  }
  cuvidDestroyVideoParser(Decoder->Parser);
  cuCtxDestroy(Decoder->Context);
  A_resize(Decoder, 0);
}

sintptr_t _ETC_VEDC_Decoder_Codec_cuvid_Write(
  void **DecoderData,
  uint8_t *Data,
  uintptr_t Size
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  CUVIDSOURCEDATAPACKET pkt;
  pkt.flags = 0;
  pkt.payload_size = Size;
  pkt.payload = Data;
  pkt.timestamp = 0;
  CUresult r = cuvidParseVideoData(Decoder->Parser, &pkt);
  if(r != CUDA_SUCCESS){
    return -ETC_VEDC_Decoder_Error_DecoderError;
  }

  return Size;
}

bool _ETC_VEDC_Decoder_Codec_cuvid_IsReadable(
  void **DecoderData
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  return Decoder->wrd.Readable == 1;
}

bool _ETC_VEDC_Decoder_Codec_cuvid_IsReadType(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_CudaArrayFrame:{
      return 1;
    }
    default:{
      return 0;
    }
  }
}

void _ETC_VEDC_Decoder_Codec_cuvid_GetReadImageProperties(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  ETC_VEDC_Decoder_ImageProperties_t *ImageProperties
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_CudaArrayFrame:{
      ImageProperties->PixelFormat = ETC_PIXF_YUVNV12;

      ImageProperties->Stride[0] = Decoder->wrd.SizeX;
      ImageProperties->Stride[1] = Decoder->wrd.SizeY;

      ImageProperties->SizeX = Decoder->wrd.SizeX;
      ImageProperties->SizeY = Decoder->wrd.SizeY;

      return;
    }
    default:{
      return;
    }
  }
}

ETC_VEDC_Decoder_Error
_ETC_VEDC_Decoder_Codec_cuvid_Read(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;

  if(Decoder->wrd.Readable == 0){
    return ETC_VEDC_Decoder_Error_NotReadable;
  }

  switch(ReadType){
    case ETC_VEDC_Decoder_ReadType_CudaArrayFrame:{
      ETC_VEDC_Decoder_CudaArrayFrame_t *Frame = (ETC_VEDC_Decoder_CudaArrayFrame_t *)ReadData;

      CUVIDPROCPARAMS p = {0};
      p.progressive_frame = Decoder->wrd.Info.progressive_frame;
      p.second_field = Decoder->wrd.Info.repeat_first_field + 1;
      p.top_field_first = Decoder->wrd.Info.top_field_first;
      p.unpaired_field = Decoder->wrd.Info.repeat_first_field < 0;

      uintptr_t HostPointer;
      unsigned int nPitch;
      if(cuvidMapVideoFrame(
        Decoder->Decoder,
        Decoder->wrd.Info.picture_index,
        &HostPointer,
        &nPitch,
        &p
      ) != CUDA_SUCCESS){
        PR_abort();
      }

      if(cudaMemcpy2DToArray(
        Frame->Array[0],
        0,
        0,
        (void*)HostPointer,
        nPitch,
        Decoder->wrd.SizeX,
        Decoder->wrd.SizeY,
        cudaMemcpyDeviceToDevice
      ) != CUDA_SUCCESS){
        PR_abort();
      }
      if(cudaMemcpy2DToArray(
        Frame->Array[1],
        0,
        0,
        (void*)(HostPointer + nPitch * Decoder->wrd.SizeY),
        nPitch,
        Decoder->wrd.SizeX,
        Decoder->wrd.SizeY / 2,
        cudaMemcpyDeviceToDevice
      ) != CUDA_SUCCESS){
        PR_abort();
      }

      if(cuvidUnmapVideoFrame(Decoder->Decoder, HostPointer) != CUDA_SUCCESS){
        PR_abort();
      }

      Decoder->wrd.Readable = 0;

      return ETC_VEDC_Decoder_Error_OK;
    }
    default:{
      return ETC_VEDC_Decoder_Error_InvalidReadType;
    }
  }
}

void _ETC_VEDC_Decoder_Codec_cuvid_ReadClear(
  void **DecoderData,
  ETC_VEDC_Decoder_ReadType ReadType,
  void *ReadData
){
  _ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_cuvid_Decoder_t *)*DecoderData;
}
