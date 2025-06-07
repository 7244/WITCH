#include _WITCH_PATH(MEM/MEM.h)
#include <wayland-client.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Wayland protocol headers - generated from .xml files
// https://raw.githubusercontent.com/swaywm/wlr-protocols/master/unstable/wlr-screencopy-unstable-v1.xml
// xdg-output-unstable-v1.xml Is usually found from system headers /usr/share/wayland-protocols/unstable/xdg-output/
// Generating headers and source files:
// 
// wayland-scanner client-header wlr-screencopy-unstable-v1.xml wlr-screencopy-unstable-v1-client-protocol.h
// wayland-scanner private-code wlr-screencopy-unstable-v1.xml wlr-screencopy-unstable-v1-protocol.c
// 
// wayland-scanner client-header xdg-output-unstable-v1.xml xdg-output-unstable-v1-client-protocol.h
// wayland-scanner private-code xdg-output-unstable-v1.xml xdg-output-unstable-v1-protocol.c
// 
// Link: -lwayland-client including the .c files
#include "wlr-screencopy-unstable-v1-client-protocol.h"
#include "xdg-output-unstable-v1-client-protocol.h"

typedef struct {
  struct wl_display* display;
  struct wl_registry* registry;
  struct wl_compositor* compositor;
  struct wl_shm* shm;
  struct zwlr_screencopy_manager_v1* screencopy_manager;
  struct zxdg_output_manager_v1* output_manager;
  struct wl_output* output;
  struct zxdg_output_v1* xdg_output;

  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint32_t format;

  int shm_fd;
  uint8_t* shm_data;
  size_t shm_size;
  struct wl_buffer* buffer;
  struct zwlr_screencopy_frame_v1* frame;

  int done;
  int failed;
  int geometry_received;
} wayland_screencap_t;

static void registry_global(void* data, struct wl_registry* registry,
  uint32_t id, const char* interface, uint32_t version) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;

  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    ctx->compositor = (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 4);
  }
  else if (strcmp(interface, wl_shm_interface.name) == 0) {
    ctx->shm = (struct wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
  }
  else if (strcmp(interface, wl_output_interface.name) == 0) {
    ctx->output = (struct wl_output*)wl_registry_bind(registry, id, &wl_output_interface, 3);
  }
  else if (strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
    ctx->screencopy_manager = (struct zwlr_screencopy_manager_v1*)wl_registry_bind(registry, id,
      &zwlr_screencopy_manager_v1_interface, 3);
  }
  else if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
    ctx->output_manager = (struct zxdg_output_manager_v1*)wl_registry_bind(registry, id,
      &zxdg_output_manager_v1_interface, 2);
  }
}

static void registry_global_remove(void* data, struct wl_registry* registry, uint32_t id) {

}
static const struct wl_registry_listener registry_listener = {
  .global = registry_global,
  .global_remove = registry_global_remove,
};

static void xdg_output_logical_position(void* data, struct zxdg_output_v1* output,
  int32_t x, int32_t y) {

}
static void xdg_output_logical_size(void* data, struct zxdg_output_v1* output,
  int32_t width, int32_t height) {
   wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->width = width;
  ctx->height = height;
  ctx->geometry_received = 1;
}
static void xdg_output_done(void* data, struct zxdg_output_v1* output) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->geometry_received = 1;
}
static void xdg_output_name(void* data, struct zxdg_output_v1* output, const char* name) {

}
static void xdg_output_description(void* data, struct zxdg_output_v1* output, const char* desc) {

}
static const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_logical_position,
    .logical_size = xdg_output_logical_size,
    .done = xdg_output_done,
    .name = xdg_output_name,
    .description = xdg_output_description,
};

sint32_t MD_SCR_Get_Resolution(MD_SCR_Resolution_t* Resolution) {
  struct wl_display* display;
  struct wl_registry* registry;
  wayland_screencap_t temp_ctx = { 0 };

  display = wl_display_connect(NULL);
  if (!display) {
    return 1;
  }

  temp_ctx.display = display;
  registry = wl_display_get_registry(display);
  if (!registry) {
    wl_display_disconnect(display);
    return 1;
  }

  wl_registry_add_listener(registry, &registry_listener, &temp_ctx);
  wl_display_roundtrip(display);

  if (!temp_ctx.output) {
    wl_registry_destroy(registry);
    wl_display_disconnect(display);
    return 1;
  }

  if (temp_ctx.output_manager) {
    temp_ctx.xdg_output = zxdg_output_manager_v1_get_xdg_output(temp_ctx.output_manager, temp_ctx.output);
    if (temp_ctx.xdg_output) {
      zxdg_output_v1_add_listener(temp_ctx.xdg_output, &xdg_output_listener, &temp_ctx);

      while (!temp_ctx.geometry_received) {
        if (wl_display_dispatch(display) < 0) {
          break;
        }
      }

      zxdg_output_v1_destroy(temp_ctx.xdg_output);
    }
  }

  if (temp_ctx.output_manager) {
    zxdg_output_manager_v1_destroy(temp_ctx.output_manager);
  }
  if (temp_ctx.output) {
    wl_output_destroy(temp_ctx.output);
  }
  wl_registry_destroy(registry);
  wl_display_disconnect(display);

  if (temp_ctx.width > 0 && temp_ctx.height > 0) {
    Resolution->x = temp_ctx.width;
    Resolution->y = temp_ctx.height;
  }
  else {
    Resolution->x = 0;
    Resolution->y = 0;
  }

  return 0;
}

static int create_shm_file(size_t size) {
  int fd = memfd_create("wayland-screencap", MFD_CLOEXEC);
  if (fd < 0) {
    return -1;
  }

  if (ftruncate(fd, size) < 0) {
    close(fd);
    return -1;
  }

  return fd;
}

static void frame_buffer(void* data, struct zwlr_screencopy_frame_v1* frame,
  uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;

  ctx->width = width;
  ctx->height = height;
  ctx->stride = stride;
  ctx->format = format;
  ctx->shm_size = stride * height;

  ctx->shm_fd = create_shm_file(ctx->shm_size);
  if (ctx->shm_fd < 0) {
    ctx->failed = 1;
    return;
  }

  ctx->shm_data = (uint8_t*)mmap(NULL, ctx->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->shm_fd, 0);
  if (ctx->shm_data == MAP_FAILED) {
    close(ctx->shm_fd);
    ctx->failed = 1;
    return;
  }

  struct wl_shm_pool* pool = wl_shm_create_pool(ctx->shm, ctx->shm_fd, ctx->shm_size);
  ctx->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);
  wl_shm_pool_destroy(pool);

  zwlr_screencopy_frame_v1_copy(frame, ctx->buffer);
}

static void frame_flags(void* data, struct zwlr_screencopy_frame_v1* frame, uint32_t flags) {

}
static void frame_ready(void* data, struct zwlr_screencopy_frame_v1* frame,
  uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->done = 1;
}
static void frame_failed(void* data, struct zwlr_screencopy_frame_v1* frame) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->failed = 1;
}
static void frame_damage(void* data, struct zwlr_screencopy_frame_v1* frame,
  uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

}
static void frame_linux_dmabuf(void* data, struct zwlr_screencopy_frame_v1* frame,
  uint32_t format, uint32_t modifier_hi, uint32_t modifier_lo) {

}
static void frame_buffer_done(void* data, struct zwlr_screencopy_frame_v1* frame) {

}

static const struct zwlr_screencopy_frame_v1_listener frame_listener = {
  .buffer = frame_buffer,
  .flags = frame_flags,
  .ready = frame_ready,
  .failed = frame_failed,
  .damage = frame_damage,
  .linux_dmabuf = frame_linux_dmabuf,
  .buffer_done = frame_buffer_done,
};


typedef struct {
  wayland_screencap_t* _ctx;
  MD_SCR_Geometry_t Geometry;
} MD_SCR_t;

void MD_SCR_close(MD_SCR_t* scr);

sint32_t MD_SCR_open(MD_SCR_t* scr) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)malloc(sizeof(wayland_screencap_t));
  if (!ctx) {
    return 5;
  }

  __builtin_memset(ctx, 0, sizeof(*ctx));
  ctx->shm_fd = -1;
  scr->_ctx = ctx;

  ctx->display = wl_display_connect(NULL);
  if (!ctx->display) {
    free(ctx);
    scr->_ctx = NULL;
    return 2;
  }

  ctx->registry = wl_display_get_registry(ctx->display);
  if (!ctx->registry) {
    wl_display_disconnect(ctx->display);
    free(ctx);
    scr->_ctx = NULL;
    return 3;
  }

  wl_registry_add_listener(ctx->registry, &registry_listener, ctx);
  wl_display_roundtrip(ctx->display);

  if (!ctx->screencopy_manager || !ctx->output || !ctx->shm) {
    MD_SCR_close(scr);
    return 4;
  }

  if (ctx->output_manager) {
    ctx->xdg_output = zxdg_output_manager_v1_get_xdg_output(ctx->output_manager, ctx->output);
    if (ctx->xdg_output) {
      zxdg_output_v1_add_listener(ctx->xdg_output, &xdg_output_listener, ctx);
      
      while (!ctx->geometry_received) {
        if (wl_display_dispatch(ctx->display) < 0) {
          MD_SCR_close(scr);
          return 6;
        }
      }
    }
  }

  // If we still don't have geometry, we'll get it from the first frame capture
  // Set initial geometry (will be updated when we get actual dimensions)
  scr->Geometry.Resolution.x = ctx->width > 0 ? ctx->width : 1920;
  scr->Geometry.Resolution.y = ctx->height > 0 ? ctx->height : 1080;
  scr->Geometry.LineSize = ctx->stride > 0 ? ctx->stride : (scr->Geometry.Resolution.x * 4);

  return 0;
}

void MD_SCR_close(MD_SCR_t* scr) {
  if (!scr || !scr->_ctx) {
    return;
  }

  wayland_screencap_t* ctx = scr->_ctx;

  if (ctx->frame) {
    zwlr_screencopy_frame_v1_destroy(ctx->frame);
    ctx->frame = NULL;
  }

  if (ctx->buffer) {
    wl_buffer_destroy(ctx->buffer);
    ctx->buffer = NULL;
  }

  if (ctx->shm_data && ctx->shm_data != MAP_FAILED) {
    munmap(ctx->shm_data, ctx->shm_size);
    ctx->shm_data = NULL;
  }

  if (ctx->shm_fd >= 0) {
    close(ctx->shm_fd);
    ctx->shm_fd = -1;
  }

  if (ctx->xdg_output) {
    zxdg_output_v1_destroy(ctx->xdg_output);
  }

  if (ctx->output_manager) {
    zxdg_output_manager_v1_destroy(ctx->output_manager);
  }

  if (ctx->screencopy_manager) {
    zwlr_screencopy_manager_v1_destroy(ctx->screencopy_manager);
  }

  if (ctx->output) {
    wl_output_destroy(ctx->output);
  }

  if (ctx->shm) {
    wl_shm_destroy(ctx->shm);
  }

  if (ctx->compositor) {
    wl_compositor_destroy(ctx->compositor);
  }

  if (ctx->registry) {
    wl_registry_destroy(ctx->registry);
  }

  if (ctx->display) {
    wl_display_disconnect(ctx->display);
  }

  free(ctx);
  scr->_ctx = NULL;
}

uint8_t* MD_SCR_read(MD_SCR_t* scr) {
  if (!scr || !scr->_ctx) {
    return NULL;
  }

  wayland_screencap_t* ctx = scr->_ctx;

  if (ctx->frame) {
    zwlr_screencopy_frame_v1_destroy(ctx->frame);
    ctx->frame = NULL;
  }

  if (ctx->buffer) {
    wl_buffer_destroy(ctx->buffer);
    ctx->buffer = NULL;
  }

  if (ctx->shm_data && ctx->shm_data != MAP_FAILED) {
    munmap(ctx->shm_data, ctx->shm_size);
    ctx->shm_data = NULL;
  }

  if (ctx->shm_fd >= 0) {
    close(ctx->shm_fd);
    ctx->shm_fd = -1;
  }

  // Reset capture state
  ctx->done = 0;
  ctx->failed = 0;

  // Start new screen capture
  ctx->frame = zwlr_screencopy_manager_v1_capture_output(ctx->screencopy_manager, 0, ctx->output);
  if (!ctx->frame) {
    return NULL;
  }

  zwlr_screencopy_frame_v1_add_listener(ctx->frame, &frame_listener, ctx);

  // Process events until capture is complete or fails
  while (!ctx->done && !ctx->failed) {
    if (wl_display_dispatch(ctx->display) < 0) {
      return NULL;
    }
  }

  if (ctx->failed || !ctx->shm_data) {
    return NULL;
  }

  scr->Geometry.Resolution.x = ctx->width;
  scr->Geometry.Resolution.y = ctx->height;
  scr->Geometry.LineSize = ctx->stride;

  // Ensure data is synchronized (equivalent to xcb_flush)
  wl_display_flush(ctx->display);

  return ctx->shm_data;
}
