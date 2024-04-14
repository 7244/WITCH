#include <X11/Xlib.h>

typedef struct{
  Display *dpy;
  VADisplay vadpy;
}_ETC_VEDC_Decoder_Codec_va_Decoder_t;

sint32_t _ETC_VEDC_Decoder_Codec_va_Open(
  void **DecoderData,
  void *DecoderUnique
){
  _ETC_VEDC_Decoder_Codec_va_Decoder_t *Decoder =
    (_ETC_VEDC_Decoder_Codec_va_Decoder_t *)A_resize(0, sizeof(_ETC_VEDC_Decoder_Codec_va_Decoder_t));
  *DecoderData = (void *)Decoder;

  Decoder->dpy = XOpenDisplay(NULL);
  if(Decoder->dpy == NULL){
    goto gt_err0;
  }

  Decoder->vadpy = vaGetDisplay(Decoder->dpy);
  if(Decoder->vadpy == NULL){
    goto gt_err1;
  }
  int major;
  int minor;
  if(vaInitialize(Decoder->vadpy, &major, &minor) != VA_STATUS_SUCCESS){
    goto gt_err2;
  }

  return 0;

  gt_err2:
  vaTerminate(Decoder->vadpy);
  gt_err1:
  XCloseDisplay(Decoder->dpy);
  gt_err0:
  A_resize(Decoder, 0);
  return 1;
}

void _ETC_VEDC_Decoder_Codec_va_Close(
  void **DecoderData
){
  _ETC_VEDC_Decoder_Codec_va_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t *)*DecoderData;

  vaTerminate(Decoder->vadpy);
  XCloseDisplay(Decoder->dpy);
  A_resize(Decoder, 0);
}

sint32_t _ETC_VEDC_Decoder_Codec_va_Write(
  void **DecoderData,
  uint8_t *Data,
  uint32_t Size
){
  _ETC_VEDC_Decoder_Codec_va_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t *)*DecoderData;

  PR_abort();

  return 0;
}

bool _ETC_VEDC_Decoder_Codec_va_Read(
  void **DecoderData,
  ETC_VEDC_Decoder_Frame_t *Frame
){
  _ETC_VEDC_Decoder_Codec_va_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t *)*DecoderData;

  PR_abort();

  return 1;
}

void _ETC_VEDC_Decoder_Codec_va_ReadClear(
  void **DecoderData,
  ETC_VEDC_Decoder_Frame_t *Frame
){
  _ETC_VEDC_Decoder_Codec_va_Decoder_t *Decoder = (_ETC_VEDC_Decoder_Codec_va_Decoder_t *)*DecoderData;

  PR_abort();
}
