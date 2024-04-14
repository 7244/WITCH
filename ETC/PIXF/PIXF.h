#pragma once

typedef enum{
  ETC_PIXF_YUV420p,
  ETC_PIXF_YUVNV12,
  ETC_PIXF_BGRX,
  ETC_PIXF_BGRA,
  ETC_PIXF_RGB24
}ETC_PIXF;

uint8_t ETC_PIXF_GetImageCount(ETC_PIXF pixf){
  switch(pixf){
    case ETC_PIXF_YUV420p:
    {
      return 3;
    }
    case ETC_PIXF_YUVNV12:
    {
      return 2;
    }
    case ETC_PIXF_BGRX:
    case ETC_PIXF_BGRA:
    case ETC_PIXF_RGB24:
    {
      return 1;
    }
  }
  PR_abort();
  __unreachable();
}

void ETC_PIXF_GetImageSize(ETC_PIXF pixf, uint8_t ImageIndex, uint32_t *x, uint32_t *y){
  switch(pixf){
    case ETC_PIXF_YUV420p:
    {
      switch(ImageIndex){
        case 0:
        {
          return;
        }
        case 1:
        case 2:
        {
          *x /= 2;
          *y /= 2;
          return;
        }
        default:{
          PR_abort();
          __unreachable();
        }
      }
    }
    case ETC_PIXF_YUVNV12:
    {
      switch(ImageIndex){
        case 0:
        {
          return;
        }
        case 1:{
          *y /= 2;
          return;
        }
        default:{
          PR_abort();
          __unreachable();
        }
      }
    }
    case ETC_PIXF_BGRX:
    case ETC_PIXF_BGRA:
    {
      if(ImageIndex != 0){
        PR_abort();
        __unreachable();
      }
      *x *= 4;
      return;
    }
    case ETC_PIXF_RGB24:
    {
      if(ImageIndex != 0){
        PR_abort();
        __unreachable();
      }
      *x *= 3;
      return;
    }
    default:{
      PR_abort();
      __unreachable();
    }
  }
}
