typedef void *CUvideodecoder;
typedef void *CUvideoparser;

typedef long long CUvideotimestamp;

typedef struct _CUVIDPARSERDISPINFO{
  int picture_index;
  int progressive_frame;
  int top_field_first;
  int repeat_first_field;

  CUvideotimestamp timestamp;
}CUVIDPARSERDISPINFO;

typedef enum cudaVideoCodec_enum{
  cudaVideoCodec_MPEG1 = 0,
  cudaVideoCodec_MPEG2,
  cudaVideoCodec_MPEG4,
  cudaVideoCodec_VC1,
  cudaVideoCodec_H264,
  cudaVideoCodec_JPEG,
  cudaVideoCodec_H264_SVC,
  cudaVideoCodec_H264_MVC,
  cudaVideoCodec_HEVC,
  cudaVideoCodec_VP8,
  cudaVideoCodec_VP9,
  cudaVideoCodec_AV1,
  cudaVideoCodec_NumCodecs,

  cudaVideoCodec_YUV420 = (('I'<<24)|('Y'<<16)|('U'<<8)|('V')),
  cudaVideoCodec_YV12   = (('Y'<<24)|('V'<<16)|('1'<<8)|('2')),
  cudaVideoCodec_NV12   = (('N'<<24)|('V'<<16)|('1'<<8)|('2')),
  cudaVideoCodec_YUYV   = (('Y'<<24)|('U'<<16)|('Y'<<8)|('V')),
  cudaVideoCodec_UYVY   = (('U'<<24)|('Y'<<16)|('V'<<8)|('Y'))
}cudaVideoCodec;

typedef enum cudaVideoChromaFormat_enum{
  cudaVideoChromaFormat_Monochrome = 0,
  cudaVideoChromaFormat_420,
  cudaVideoChromaFormat_422,
  cudaVideoChromaFormat_444
}cudaVideoChromaFormat;

typedef enum cudaVideoSurfaceFormat_enum{
  cudaVideoSurfaceFormat_NV12 = 0,
  cudaVideoSurfaceFormat_P016 = 1,
  cudaVideoSurfaceFormat_YUV444 = 2,
  cudaVideoSurfaceFormat_YUV444_16Bit = 3,
}cudaVideoSurfaceFormat;

typedef enum cudaVideoDeinterlaceMode_enum{
  cudaVideoDeinterlaceMode_Weave = 0,
  cudaVideoDeinterlaceMode_Bob,
  cudaVideoDeinterlaceMode_Adaptive
}cudaVideoDeinterlaceMode;

typedef struct _CUcontextlock_st *CUvideoctxlock;

typedef enum cudaVideoCreateFlags_enum{
  cudaVideoCreate_Default     = 0x00,
  cudaVideoCreate_PreferCUDA  = 0x01,
  cudaVideoCreate_PreferDXVA  = 0x02,
  cudaVideoCreate_PreferCUVID = 0x04
}cudaVideoCreateFlags;

typedef struct _CUVIDDECODECREATEINFO{
  unsigned long ulWidth;
  unsigned long ulHeight;
  unsigned long ulNumDecodeSurfaces;
  cudaVideoCodec CodecType;
  cudaVideoChromaFormat ChromaFormat;
  unsigned long ulCreationFlags;
  unsigned long bitDepthMinus8;
  unsigned long ulIntraDecodeOnly;

  unsigned long ulMaxWidth;
  unsigned long ulMaxHeight;
  unsigned long Reserved1;

  struct{
    short left;
    short top;
    short right;
    short bottom;
  }display_area;

  cudaVideoSurfaceFormat OutputFormat;
  cudaVideoDeinterlaceMode DeinterlaceMode;
  unsigned long ulTargetWidth;
  unsigned long ulTargetHeight;
  unsigned long ulNumOutputSurfaces;
  CUvideoctxlock vidLock;
  struct{
    short left;
    short top;
    short right;
    short bottom;
  }target_rect;

  unsigned long enableHistogram;
  unsigned long Reserved2[4];
}CUVIDDECODECREATEINFO;

CUresult cuvidDestroyDecoder(CUvideodecoder hDecoder){
  return CUDA_SUCCESS;
}

CUresult cuvidCreateDecoder(CUvideodecoder *phDecoder, CUVIDDECODECREATEINFO *pdci){
  return CUDA_SUCCESS;
}

typedef struct _CUVIDH264DPBENTRY{
  int PicIdx;
  int FrameIdx;
  int is_long_term;
  int not_existing;
  int used_for_reference;
  int FieldOrderCnt[2];
}CUVIDH264DPBENTRY;

typedef struct _CUVIDH264MVCEXT{
  int num_views_minus1;
  int view_id;
  unsigned char inter_view_flag;
  unsigned char num_inter_view_refs_l0;
  unsigned char num_inter_view_refs_l1;
  unsigned char MVCReserved8Bits;
  int InterViewRefsL0[16];
  int InterViewRefsL1[16];
}CUVIDH264MVCEXT;
typedef struct _CUVIDH264SVCEXT{
  unsigned char profile_idc;
  unsigned char level_idc;
  unsigned char DQId;
  unsigned char DQIdMax;
  unsigned char disable_inter_layer_deblocking_filter_idc;
  unsigned char ref_layer_chroma_phase_y_plus1;
  signed char   inter_layer_slice_alpha_c0_offset_div2;
  signed char   inter_layer_slice_beta_offset_div2;

  unsigned short DPBEntryValidFlag;
  unsigned char inter_layer_deblocking_filter_control_present_flag;
  unsigned char extended_spatial_scalability_idc;
  unsigned char adaptive_tcoeff_level_prediction_flag;
  unsigned char slice_header_restriction_flag;
  unsigned char chroma_phase_x_plus1_flag;
  unsigned char chroma_phase_y_plus1;

  unsigned char tcoeff_level_prediction_flag;
  unsigned char constrained_intra_resampling_flag;
  unsigned char ref_layer_chroma_phase_x_plus1_flag;
  unsigned char store_ref_base_pic_flag;
  unsigned char Reserved8BitsA;
  unsigned char Reserved8BitsB;

  short scaled_ref_layer_left_offset;
  short scaled_ref_layer_top_offset;
  short scaled_ref_layer_right_offset;
  short scaled_ref_layer_bottom_offset;
  unsigned short Reserved16Bits;
  struct _CUVIDPICPARAMS *pNextLayer;
  int bRefBaseLayer;
} CUVIDH264SVCEXT;

typedef struct _CUVIDH264PICPARAMS{
  int log2_max_frame_num_minus4;
  int pic_order_cnt_type;
  int log2_max_pic_order_cnt_lsb_minus4;
  int delta_pic_order_always_zero_flag;
  int frame_mbs_only_flag;
  int direct_8x8_inference_flag;
  int num_ref_frames;
  unsigned char residual_colour_transform_flag;
  unsigned char bit_depth_luma_minus8;
  unsigned char bit_depth_chroma_minus8;
  unsigned char qpprime_y_zero_transform_bypass_flag;
  int entropy_coding_mode_flag;
  int pic_order_present_flag;
  int num_ref_idx_l0_active_minus1;
  int num_ref_idx_l1_active_minus1;
  int weighted_pred_flag;
  int weighted_bipred_idc;
  int pic_init_qp_minus26;
  int deblocking_filter_control_present_flag;
  int redundant_pic_cnt_present_flag;
  int transform_8x8_mode_flag;
  int MbaffFrameFlag;
  int constrained_intra_pred_flag;
  int chroma_qp_index_offset;
  int second_chroma_qp_index_offset;
  int ref_pic_flag;
  int frame_num;
  int CurrFieldOrderCnt[2];
  CUVIDH264DPBENTRY dpb[16];
  unsigned char WeightScale4x4[6][16];
  unsigned char WeightScale8x8[2][64];
  unsigned char fmo_aso_enable;
  unsigned char num_slice_groups_minus1;
  unsigned char slice_group_map_type;
  signed char pic_init_qs_minus26;
  unsigned int slice_group_change_rate_minus1;
  union{
    unsigned long long slice_group_map_addr;
    const unsigned char *pMb2SliceGroupMap;
  }fmo;
  unsigned int  Reserved[12];
  union{
    CUVIDH264MVCEXT mvcext;
    CUVIDH264SVCEXT svcext;
  };
}CUVIDH264PICPARAMS;

typedef struct _CUVIDPICPARAMS{
  int PicWidthInMbs;
  int FrameHeightInMbs;
  int CurrPicIdx;
  int field_pic_flag;
  int bottom_field_flag;
  int second_field;

  unsigned int nBitstreamDataLen;
  const unsigned char *pBitstreamData;
  unsigned int nNumSlices;
  const unsigned int *pSliceDataOffsets;
  int ref_pic_flag;
  int intra_pic_flag;
  unsigned int Reserved[30];

  union{
    // CUVIDMPEG2PICPARAMS mpeg2; TODO
    CUVIDH264PICPARAMS  h264;
    // CUVIDVC1PICPARAMS   vc1; TODO
    // CUVIDMPEG4PICPARAMS mpeg4; TODO
    // CUVIDJPEGPICPARAMS  jpeg; TODO
    // CUVIDHEVCPICPARAMS  hevc; TODO
    // CUVIDVP8PICPARAMS   vp8; TODO
    // CUVIDVP9PICPARAMS   vp9; TODO
    // CUVIDAV1PICPARAMS   av1; TODO
    unsigned int CodecReserved[1024];
  }CodecSpecific;
}CUVIDPICPARAMS;

CUresult cuvidDecodePicture(CUvideodecoder hDecoder, CUVIDPICPARAMS *pPicParams){
  return CUDA_SUCCESS;
}

typedef struct{
  cudaVideoCodec codec;
  struct{
    unsigned int numerator;
    unsigned int denominator;
  }frame_rate;
  unsigned char progressive_sequence;
  unsigned char bit_depth_luma_minus8;
  unsigned char bit_depth_chroma_minus8;
  unsigned char min_num_decode_surfaces;
  unsigned int coded_width;
  unsigned int coded_height;
  struct{
    int left;
    int top;
    int right;
    int bottom;
  }display_area;
  cudaVideoChromaFormat chroma_format;
  unsigned int bitrate;
  struct{
    int x;
    int y;
  }display_aspect_ratio;
  struct{
    unsigned char video_format          : 3;
    unsigned char video_full_range_flag : 1;
    unsigned char reserved_zero_bits    : 4;
    unsigned char color_primaries;
    unsigned char transfer_characteristics;
    unsigned char matrix_coefficients;
  }video_signal_description;
  unsigned int seqhdr_data_length;
}CUVIDEOFORMAT;

typedef struct{
  cudaVideoCodec codec;
  union{
    struct{
      unsigned char  operating_points_cnt;
      unsigned char  reserved24_bits[3];
      unsigned short operating_points_idc[32];
    }av1;
    unsigned char CodecReserved[1024];
  };
}CUVIDOPERATINGPOINTINFO;

typedef struct _CUSEIMESSAGE{
  unsigned char sei_message_type;
  unsigned char reserved[3];
  unsigned int sei_message_size;
}CUSEIMESSAGE;

typedef struct _CUVIDSEIMESSAGEINFO{
  void *pSEIData;
  CUSEIMESSAGE *pSEIMessage;
  unsigned int sei_message_count;
  unsigned int picIdx;
} CUVIDSEIMESSAGEINFO;

typedef int (*PFNVIDSEQUENCECALLBACK)(void *, CUVIDEOFORMAT *);
typedef int (*PFNVIDDECODECALLBACK)(void *, CUVIDPICPARAMS *);
typedef int (*PFNVIDDISPLAYCALLBACK)(void *, CUVIDPARSERDISPINFO *);
typedef int (*PFNVIDOPPOINTCALLBACK)(void *, CUVIDOPERATINGPOINTINFO *);
typedef int (*PFNVIDSEIMSGCALLBACK)(void *, CUVIDSEIMESSAGEINFO *);

typedef struct{
  unsigned int max_width;
  unsigned int max_height;
  unsigned char reserved[1016];
}CUVIDAV1SEQHDR;

typedef struct{
  CUVIDEOFORMAT format;
  union{
    CUVIDAV1SEQHDR av1;
    unsigned char raw_seqhdr_data[1024];
  };
}CUVIDEOFORMATEX;

typedef struct _CUVIDPARSERPARAMS{
  cudaVideoCodec CodecType;
  unsigned int ulMaxNumDecodeSurfaces;
  unsigned int ulClockRate;
  unsigned int ulErrorThreshold;
  unsigned int ulMaxDisplayDelay;
  unsigned int bAnnexb : 1;
  unsigned int uReserved : 31;
  unsigned int uReserved1[4];
  void *pUserData;
  PFNVIDSEQUENCECALLBACK pfnSequenceCallback;
  PFNVIDDECODECALLBACK pfnDecodePicture;
  PFNVIDDISPLAYCALLBACK pfnDisplayPicture;
  PFNVIDOPPOINTCALLBACK pfnGetOperatingPoint;
  PFNVIDSEIMSGCALLBACK pfnGetSEIMsg;
  void *pvReserved2[5];
  CUVIDEOFORMATEX *pExtVideoInfo;
}CUVIDPARSERPARAMS;

CUresult cuvidCreateVideoParser(CUvideoparser *pObj, CUVIDPARSERPARAMS *pParams){
  return CUDA_SUCCESS;
}

CUresult cuvidDestroyVideoParser(CUvideoparser obj){
  return CUDA_SUCCESS;
}

typedef struct _CUVIDSOURCEDATAPACKET{
  unsigned long flags;
  unsigned long payload_size;
  const unsigned char *payload;
  CUvideotimestamp timestamp;
}CUVIDSOURCEDATAPACKET;

CUresult cuvidParseVideoData(CUvideoparser obj, CUVIDSOURCEDATAPACKET *pPacket){
  return CUDA_SUCCESS;
}

typedef struct _CUVIDPROCPARAMS{
  int progressive_frame;
  int second_field;
  int top_field_first;
  int unpaired_field;

  unsigned int reserved_flags;
  unsigned int reserved_zero;
  unsigned long long raw_input_dptr;
  unsigned int raw_input_pitch;
  unsigned int raw_input_format;
  unsigned long long raw_output_dptr;
  unsigned int raw_output_pitch;
  unsigned int Reserved1;
  CUstream output_stream;
  unsigned int Reserved[46];
  unsigned long long *histogram_dptr;
  void *Reserved2[1];
}CUVIDPROCPARAMS;

CUresult cuvidMapVideoFrame(
  CUvideodecoder hDecoder,
  int nPicIdx,
  uintptr_t *pDevPtr,
  unsigned int *pPitch,
  CUVIDPROCPARAMS *pVPP
){
  return CUDA_SUCCESS;
}
CUresult cuvidUnmapVideoFrame(
  CUvideodecoder hDecoder,
  uintptr_t DevPtr
){
  return CUDA_SUCCESS;
}
