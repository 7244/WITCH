#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(VEC/VEC.h)

#include _WITCH_PATH(include/windows/DXGI.h)
#include _WITCH_PATH(include/windows/DXGI1_2.h)
#include _WITCH_PATH(include/windows/D3D11.h)

static sint32_t MD_SCR_Get_Resolution(MD_SCR_Resolution_t *Resolution){
  RECT desktop;
  const HWND hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);
  Resolution->x = desktop.right;
  Resolution->y = desktop.bottom;
  return 0;
}

typedef struct{
  IDXGIOutputDuplication *duplication;
  ID3D11Texture2D *texture;
  ID3D11Device *device;
  ID3D11DeviceContext *context;
  D3D11_TEXTURE2D_DESC tex_desc;
  D3D11_MAPPED_SUBRESOURCE map;
  HRESULT map_result;
  bool imDed;

  MD_SCR_Geometry_t Geometry;
}MD_SCR_t;
static sint32_t MD_SCR_open(MD_SCR_t *scr){
  scr->duplication = 0;
  scr->texture = 0;
  scr->device = 0;
  scr->context = 0;
  scr->map_result = ~S_OK;
  scr->imDed = 0;

  IDXGIFactory1 *factory = 0;

  if(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)(&factory)) != S_OK){
    return 1;
  }

  IDXGIAdapter1 *adapter = 0;

  VEC_t adapters;
  VEC_init(&adapters, sizeof(IDXGIAdapter1 *), A_resize);

  IDXGIOutput *output = 0;
  VEC_t outputs;
  VEC_init(&outputs, sizeof(IDXGIOutput *), A_resize);

  while(factory->EnumAdapters1(adapters.Current, &adapter) != DXGI_ERROR_NOT_FOUND){
    DXGI_ADAPTER_DESC1 desc;
    if(adapter->GetDesc1(&desc) != S_OK){
      continue;
    }
    VEC_handle(&adapters);
    ((IDXGIAdapter1 **)adapters.ptr)[adapters.Current] = adapter;
    adapters.Current++;
    while(adapter->EnumOutputs(outputs.Current, &output) != DXGI_ERROR_NOT_FOUND){
      DXGI_OUTPUT_DESC desc;
      if(output->GetDesc(&desc) != S_OK){
        continue;
      }
      VEC_handle(&outputs);
      ((IDXGIOutput **)outputs.ptr)[outputs.Current] = output;
      outputs.Current++;
    }
  }

  if(!outputs.Current){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }
  if(!adapters.Current){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  D3D_FEATURE_LEVEL feature_level;

  auto result = D3D11CreateDevice(*((IDXGIAdapter1 **)adapters.ptr),
    D3D_DRIVER_TYPE_UNKNOWN,
    NULL,
    NULL,
    NULL,
    0,
    D3D11_SDK_VERSION,
    &scr->device,
    &feature_level,
    &scr->context
  );

  if(result != S_OK){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  output = *((IDXGIOutput **)outputs.ptr);
  IDXGIOutput1* output1 = 0;

  if(output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1) != S_OK){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  output1->DuplicateOutput(scr->device, &scr->duplication);

  if(!scr->duplication){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  DXGI_OUTPUT_DESC output_desc;
  if(output->GetDesc(&output_desc) != S_OK){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  if(!output_desc.DesktopCoordinates.right || !output_desc.DesktopCoordinates.bottom){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  scr->tex_desc.Width = output_desc.DesktopCoordinates.right;
  scr->tex_desc.Height = output_desc.DesktopCoordinates.bottom;
  scr->tex_desc.MipLevels = 1;
  scr->tex_desc.ArraySize = 1;
  scr->tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  scr->tex_desc.SampleDesc.Count = 1;
  scr->tex_desc.SampleDesc.Quality = 0;
  scr->tex_desc.Usage = D3D11_USAGE_STAGING;
  scr->tex_desc.BindFlags = 0;
  scr->tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  scr->tex_desc.MiscFlags = 0;

  scr->Geometry.Resolution.x = scr->tex_desc.Width;
  scr->Geometry.Resolution.y = scr->tex_desc.Height;

  result = scr->device->CreateTexture2D(&scr->tex_desc, NULL, &scr->texture);

  if(result != S_OK){
    VEC_free(&adapters);
    VEC_free(&outputs);
    return 1;
  }

  for(uintptr_t i = 0; i < adapters.Current; i++){
    IDXGIAdapter1 *ca = ((IDXGIAdapter1 **)adapters.ptr)[i];
    if(!ca){
      continue;
    }
    ca->Release();
    ca = 0;
  }
  VEC_free(&adapters);

  for(uintptr_t i = 0; i < outputs.Current; i++){
    IDXGIOutput *co = ((IDXGIOutput **)outputs.ptr)[i];
    if(!co){
      continue;
    }
    co->Release();
    co = 0;
  }
  VEC_free(&outputs);

  if(output1){
    output1->Release();
    output1 = 0;
  }

  if(factory){
    factory->Release();
    factory = 0;
  }

  return 0;
}
static void MD_SCR_close(MD_SCR_t *scr){
  scr->context->Release();
  scr->device->Release();
  scr->texture->Release();
  scr->duplication->Release();
}

static uint8_t *MD_SCR_read(MD_SCR_t *scr){
  if (scr->imDed) {
    if (MD_SCR_open(scr)) {
      return 0;
    }
    scr->imDed = false;
  }

  DXGI_OUTDUPL_DESC duplication_desc;
  // weird right - maybe set from other thread
  if (!scr->duplication) {
    scr->imDed = true;
    return 0;
  }
  scr->duplication->GetDesc(&duplication_desc);

  DXGI_OUTDUPL_FRAME_INFO frame_info;
  IDXGIResource *desktop_resource = NULL;
  ID3D11Texture2D *tex = NULL;
  DXGI_MAPPED_RECT mapped_rect;

  auto result = scr->duplication->AcquireNextFrame(0, &frame_info, &desktop_resource);
  if (result != S_OK) {
    // if ctrl-alt-delete or resolution change
    if (result == 0x887a0026) {
      MD_SCR_close(scr);
      scr->imDed = true;
      return 0;
    }
    // if not timeout
    else if (result != 0x887a0027) {
      MD_SCR_close(scr);
      scr->imDed = true;
//      std::cout << "DXGI bad errors:" << std::hex << result << '\n';
      return 0;
    }
    if(scr->map_result == S_OK){
      goto gt_succ;
    }
    else{
      return 0;
    }
  }
  if(desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D), (void **)&tex) != S_OK){
    return 0;
  }

  if(scr->map_result == S_OK){
    scr->context->Unmap(scr->texture, 0);
  }

  scr->context->CopyResource(scr->texture, tex);

  scr->map_result = scr->context->Map(scr->texture, 0, D3D11_MAP_READ, 0, &scr->map);
  if(scr->map_result != S_OK){
    return 0;
  }

  if(tex){
    tex->Release();
    tex = NULL;
  }
  if(desktop_resource){
    desktop_resource->Release();
    desktop_resource = NULL;
  }
  if(scr->duplication->ReleaseFrame() != S_OK){
    return 0;
  }

  gt_succ:
  scr->Geometry.LineSize = scr->map.RowPitch;
  return (uint8_t *)scr->map.pData;
}
