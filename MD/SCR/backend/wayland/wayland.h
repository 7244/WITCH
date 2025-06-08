#include _WITCH_PATH(MEM/MEM.h)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#define MD_SCR_DEBUG_PRINTS

//-----------------------------------------wlroots-----------------------------------------

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
#include <wayland-client.h>
#include "wlr-screencopy-unstable-v1-client-protocol.h"
#include "xdg-output-unstable-v1-client-protocol.h"
//-----------------------------------------wlroots-----------------------------------------

//-----------------------------------------PipeWire-----------------------------------------

//Link: -ldbus-1 -lpipewire-0.3 -lspa-0.2
#include <dbus/dbus.h>

#include <pipewire/pipewire.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/type-info.h>
#include <spa/debug/types.h>

//-----------------------------------------PipeWire-----------------------------------------

typedef struct {
  struct pw_main_loop* loop;
  struct pw_core* core;
  struct pw_stream* stream;
  struct spa_hook stream_listener;

  uint8_t* frame_data;
  size_t frame_size;
  uint32_t width;
  uint32_t height;
  uint32_t stride;

  int frame_ready;
  int capture_failed;
} pipewire_ctx_t;

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

typedef enum {
  MD_SCR_METHOD_AUTO = 0,
  MD_SCR_METHOD_PORTAL_PIPEWIRE,
  MD_SCR_METHOD_WLR_SCREENCOPY,
} MD_SCR_Method_t;

typedef struct {
  MD_SCR_Method_t method;
  MD_SCR_Geometry_t Geometry;

  pipewire_ctx_t* pw_ctx;
  wayland_screencap_t* wl_ctx;

  char* portal_session_handle;
  uint32_t portal_source_id;
} MD_SCR_t;

//-----------------------------------------wlroots-----------------------------------------
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

static int wayland_init_connection(wayland_screencap_t* ctx) {
  ctx->display = wl_display_connect(NULL);
  if (!ctx->display) {
    return -1;
  }

  ctx->registry = wl_display_get_registry(ctx->display);
  if (!ctx->registry) {
    wl_display_disconnect(ctx->display);
    return -1;
  }

  wl_registry_add_listener(ctx->registry, &registry_listener, ctx);
  wl_display_roundtrip(ctx->display);

  if (!ctx->screencopy_manager || !ctx->output || !ctx->shm) {
    return -1;
  }

  if (ctx->output_manager) {
    ctx->xdg_output = zxdg_output_manager_v1_get_xdg_output(ctx->output_manager, ctx->output);
    if (ctx->xdg_output) {
      zxdg_output_v1_add_listener(ctx->xdg_output, &xdg_output_listener, ctx);

      while (!ctx->geometry_received) {
        if (wl_display_dispatch(ctx->display) < 0) {
          return -1;
        }
      }
    }
  }

  return 0;
}

static uint8_t* wayland_capture_frame(wayland_screencap_t* ctx) {
  if (!ctx) return NULL;

  // Clean up previous capture
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

  ctx->done = 0;
  ctx->failed = 0;

  ctx->frame = zwlr_screencopy_manager_v1_capture_output(ctx->screencopy_manager, 0, ctx->output);
  if (!ctx->frame) {
    return NULL;
  }

  zwlr_screencopy_frame_v1_add_listener(ctx->frame, &frame_listener, ctx);

  while (!ctx->done && !ctx->failed) {
    if (wl_display_dispatch(ctx->display) < 0) {
      return NULL;
    }
  }

  if (ctx->failed || !ctx->shm_data) {
    return NULL;
  }

  wl_display_flush(ctx->display);

  return ctx->shm_data;
}

static void wayland_cleanup(wayland_screencap_t* ctx) {
  if (!ctx) return;

  if (ctx->frame) {
    zwlr_screencopy_frame_v1_destroy(ctx->frame);
  }
  if (ctx->buffer) {
    wl_buffer_destroy(ctx->buffer);
  }
  if (ctx->shm_data && ctx->shm_data != MAP_FAILED) {
    munmap(ctx->shm_data, ctx->shm_size);
  }
  if (ctx->shm_fd >= 0) {
    close(ctx->shm_fd);
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
}

//-----------------------------------------wlroots-----------------------------------------


//-----------------------------------------PipeWire-----------------------------------------

static int portal_request_screenshare(MD_SCR_t* scr) {
  DBusConnection* conn;
  DBusMessage* msg, * reply;
  DBusError error;
  const char* session_handle;

  dbus_error_init(&error);

  conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (dbus_error_is_set(&error)) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("D-Bus connection error: %s\n", error.message);
#endif
    dbus_error_free(&error);
    return -1;
  }

  msg = dbus_message_new_method_call(
    "org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "CreateSession"
  );

  DBusMessageIter args, options;
  dbus_message_iter_init_append(msg, &args);

  dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options);
  dbus_message_iter_close_container(&args, &options);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, 5000, &error);
  dbus_message_unref(msg);

  if (dbus_error_is_set(&error)) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("CreateSession error: %s\n", error.message);
#endif
    dbus_error_free(&error);
    dbus_connection_unref(conn);
    return -1;
  }

  DBusMessageIter reply_iter;
  dbus_message_iter_init(reply, &reply_iter);

  if (dbus_message_iter_get_arg_type(&reply_iter) == DBUS_TYPE_OBJECT_PATH) {
    dbus_message_iter_get_basic(&reply_iter, &session_handle);
    scr->portal_session_handle = strdup(session_handle);
  }

  dbus_message_unref(reply);

  // Select sources (desktop)
  msg = dbus_message_new_method_call(
    "org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "SelectSources"
  );

  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &scr->portal_session_handle);

  dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options);

  // Set source types (monitor = 1)
  DBusMessageIter option, variant;
  const char* key = "types";
  uint32_t source_type = 1; // Monitor

  dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option);
  dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "u", &variant);
  dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT32, &source_type);
  dbus_message_iter_close_container(&option, &variant);
  dbus_message_iter_close_container(&options, &option);

  dbus_message_iter_close_container(&args, &options);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, 5000, &error);
  dbus_message_unref(msg);

  if (dbus_error_is_set(&error)) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("SelectSources error: %s\n", error.message);
#endif
    dbus_error_free(&error);
    dbus_connection_unref(conn);
    return -1;
  }

  dbus_message_unref(reply);

  msg = dbus_message_new_method_call(
    "org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "Start"
  );

  dbus_message_iter_init_append(msg, &args);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &scr->portal_session_handle);
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &"");

  dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options);
  dbus_message_iter_close_container(&args, &options);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, 10000, &error);
  dbus_message_unref(msg);

  if (dbus_error_is_set(&error)) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("D-Bus start error: %s\n", error.message);
#endif
    dbus_error_free(&error);
    dbus_connection_unref(conn);
    return -1;
  }

  dbus_message_iter_init(reply, &reply_iter);

  if (dbus_message_iter_get_arg_type(&reply_iter) == DBUS_TYPE_UINT32) {
    dbus_message_iter_next(&reply_iter);
  }

  if (dbus_message_iter_get_arg_type(&reply_iter) == DBUS_TYPE_ARRAY) {
    DBusMessageIter results_iter, dict_entry, variant_iter, streams_iter;

    dbus_message_iter_recurse(&reply_iter, &results_iter);

    while (dbus_message_iter_get_arg_type(&results_iter) == DBUS_TYPE_DICT_ENTRY) {
      dbus_message_iter_recurse(&results_iter, &dict_entry);

      const char* result_key;
      dbus_message_iter_get_basic(&dict_entry, &result_key);
      dbus_message_iter_next(&dict_entry);

      if (strcmp(result_key, "streams") == 0) {
        dbus_message_iter_recurse(&dict_entry, &variant_iter);
        dbus_message_iter_recurse(&variant_iter, &streams_iter);

        if (dbus_message_iter_get_arg_type(&streams_iter) == DBUS_TYPE_STRUCT) {
          DBusMessageIter stream_iter;
          dbus_message_iter_recurse(&streams_iter, &stream_iter);

          if (dbus_message_iter_get_arg_type(&stream_iter) == DBUS_TYPE_UINT32) {
            dbus_message_iter_get_basic(&stream_iter, &scr->portal_source_id);
          }
        }
        break;
      }

      dbus_message_iter_next(&results_iter);
    }
  }

  dbus_message_unref(reply);
  dbus_connection_unref(conn);

  return scr->portal_source_id > 0 ? 0 : -1;
}

static void on_stream_param_changed(void* data, uint32_t id, const struct spa_pod* param) {
  pipewire_ctx_t* ctx = (pipewire_ctx_t*)data;

  if (param == NULL || id != SPA_PARAM_Format)
    return;

  struct spa_video_info format;
  if (spa_format_parse(param, &format.media_type, &format.media_subtype) < 0)
    return;

  if (format.media_type != SPA_MEDIA_TYPE_video ||
    format.media_subtype != SPA_MEDIA_SUBTYPE_raw)
    return;

  if (spa_format_video_raw_parse(param, &format.info.raw) < 0)
    return;

  ctx->width = format.info.raw.size.width;
  ctx->height = format.info.raw.size.height;
  ctx->stride = SPA_ROUND_UP_N(ctx->width * 4, 4); // Assume BGRA
  ctx->frame_size = ctx->height * ctx->stride;

#if defined(MD_SCR_DEBUG_PRINTS)
  printf("PipeWire format: %dx%d, stride: %d\n", ctx->width, ctx->height, ctx->stride);
#endif
}

static void on_stream_process(void* data) {
  pipewire_ctx_t* ctx = (pipewire_ctx_t*)data;
  struct pw_buffer* b;
  struct spa_buffer* buf;

  if ((b = pw_stream_dequeue_buffer(ctx->stream)) == NULL) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("No buffer to dequeue\n");
#endif
    return;
  }

  buf = b->buffer;

  if (buf->datas[0].data == NULL) {
#if defined(MD_SCR_DEBUG_PRINTS)
    printf("No data in buffer\n");
#endif
    pw_stream_queue_buffer(ctx->stream, b);
    return;
  }

  if (ctx->frame_data) {
    free(ctx->frame_data);
  }

  ctx->frame_data = (uint8_t*)malloc(ctx->frame_size);
  if (ctx->frame_data) {
    memcpy(ctx->frame_data, buf->datas[0].data,
      SPA_MIN(ctx->frame_size, buf->datas[0].chunk->size));
    ctx->frame_ready = 1;
  }

  pw_stream_queue_buffer(ctx->stream, b);
}

static const struct pw_stream_events stream_events = {
  PW_VERSION_STREAM_EVENTS,
  .param_changed = on_stream_param_changed,
  .process = on_stream_process,
};

static int pipewire_connect(MD_SCR_t* scr) {
  pipewire_ctx_t* ctx = (pipewire_ctx_t*)calloc(1, sizeof(pipewire_ctx_t));
  if (!ctx) return -1;

  scr->pw_ctx = ctx;

  pw_init(NULL, NULL);

  ctx->loop = pw_main_loop_new(NULL);
  if (!ctx->loop) {
    free(ctx);
    return -1;
  }

  ctx->core = pw_context_connect_fd(pw_context_new(pw_main_loop_get_loop(ctx->loop), NULL, 0), -1, NULL, 0);
  if (!ctx->core) {
    pw_main_loop_destroy(ctx->loop);
    free(ctx);
    return -1;
  }

  char source_id_str[32];
  snprintf(source_id_str, sizeof(source_id_str), "%u", scr->portal_source_id);
  struct pw_properties* props = pw_properties_new(
    PW_KEY_MEDIA_TYPE, "Video",
    PW_KEY_MEDIA_CATEGORY, "Capture",
    PW_KEY_MEDIA_ROLE, "Screen",
    PW_KEY_TARGET_OBJECT, source_id_str,
    NULL
  );

  ctx->stream = pw_stream_new(ctx->core, "screen-capture", props);
  if (!ctx->stream) {
    pw_core_disconnect(ctx->core);
    pw_main_loop_destroy(ctx->loop);
    free(ctx);
    return -1;
  }

  pw_stream_add_listener(ctx->stream, &ctx->stream_listener, &stream_events, ctx);

  uint8_t buffer[1024];
  struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

  const struct spa_pod* params[1];
  params[0] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
    SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
    SPA_FORMAT_mediaType, SPA_POD_Id(SPA_MEDIA_TYPE_video),
    SPA_FORMAT_mediaSubtype, SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
    SPA_FORMAT_VIDEO_format, SPA_POD_CHOICE_ENUM_Id(5,
      SPA_VIDEO_FORMAT_BGRx,
      SPA_VIDEO_FORMAT_BGRA,
      SPA_VIDEO_FORMAT_RGBx,
      SPA_VIDEO_FORMAT_RGBA,
      SPA_VIDEO_FORMAT_RGB)
  );

  if (pw_stream_connect(ctx->stream,
    PW_DIRECTION_INPUT,
    PW_ID_ANY,
    (enum pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT |
    PW_STREAM_FLAG_MAP_BUFFERS),
    params, 1) < 0) {
    pw_stream_destroy(ctx->stream);
    pw_core_disconnect(ctx->core);
    pw_main_loop_destroy(ctx->loop);
    free(ctx);
    return -1;
  }

  return 0;
}

static MD_SCR_Method_t detect_best_method(void) {
  if (getenv("WAYLAND_DISPLAY") && getenv("XDG_CURRENT_DESKTOP")) {
    DBusConnection* conn;
    DBusError error;

    dbus_error_init(&error);
    conn = dbus_bus_get(DBUS_BUS_SESSION, &error);

    if (!dbus_error_is_set(&error) && conn) {
      dbus_connection_unref(conn);
      return MD_SCR_METHOD_PORTAL_PIPEWIRE;
    }
    dbus_error_free(&error);

    // Fallback to wlr-screencopy if available
    return MD_SCR_METHOD_WLR_SCREENCOPY;
  }

  return MD_SCR_METHOD_PORTAL_PIPEWIRE;
}

//-----------------------------------------PipeWire-----------------------------------------

sint32_t MD_SCR_open(MD_SCR_t* scr) {
  memset(scr, 0, sizeof(*scr));

  if (scr->method == MD_SCR_METHOD_AUTO) {
    scr->method = detect_best_method();
  }

#if defined(MD_SCR_DEBUG_PRINTS)
  printf("Using capture method: %d\n", scr->method);
#endif

  switch (scr->method) {
  case MD_SCR_METHOD_PORTAL_PIPEWIRE:
    if (portal_request_screenshare(scr) != 0) {
#if defined(MD_SCR_DEBUG_PRINTS)
      printf("Portal request failed, attempting wlroots\n");
#endif
      scr->method = MD_SCR_METHOD_WLR_SCREENCOPY;
      goto try_wlroots;
    }

    if (pipewire_connect(scr) != 0) {
      printf("PipeWire connection failed\n");
      return -1;
    }
    break;

  case MD_SCR_METHOD_WLR_SCREENCOPY:
  try_wlroots:
    scr->wl_ctx = (wayland_screencap_t*)calloc(1, sizeof(wayland_screencap_t));
    if (!scr->wl_ctx) return -1;

    scr->wl_ctx->shm_fd = -1;

    if (wayland_init_connection(scr->wl_ctx) != 0) {
      wayland_cleanup(scr->wl_ctx);
      free(scr->wl_ctx);
      scr->wl_ctx = NULL;
      return -1;
    }

    scr->Geometry.Resolution.x = scr->wl_ctx->width > 0 ? scr->wl_ctx->width : 1920;
    scr->Geometry.Resolution.y = scr->wl_ctx->height > 0 ? scr->wl_ctx->height : 1080;
    scr->Geometry.LineSize = scr->wl_ctx->stride > 0 ? scr->wl_ctx->stride : (scr->Geometry.Resolution.x * 4);

    break;

  default:
    return -1;
  }

  return 0;
}


void MD_SCR_close(MD_SCR_t* scr) {
  if (!scr) return;

  switch (scr->method) {
  case MD_SCR_METHOD_PORTAL_PIPEWIRE: {
    pipewire_ctx_t* ctx = scr->pw_ctx;
    if (ctx) {
      if (ctx->frame_data) {
        free(ctx->frame_data);
      }
      if (ctx->stream) {
        pw_stream_destroy(ctx->stream);
      }
      if (ctx->core) {
        pw_core_disconnect(ctx->core);
      }
      if (ctx->loop) {
        pw_main_loop_destroy(ctx->loop);
      }
      free(ctx);
    }

    if (scr->portal_session_handle) {
      free(scr->portal_session_handle);
    }

    pw_deinit();
    break;
  }

  case MD_SCR_METHOD_WLR_SCREENCOPY:
    if (scr->wl_ctx) {
      wayland_cleanup(scr->wl_ctx);
      free(scr->wl_ctx);
      scr->wl_ctx = NULL;
    }
    break;

  default:
    break;
  }

  memset(scr, 0, sizeof(*scr));
}

uint8_t* MD_SCR_read(MD_SCR_t* scr) {
  if (!scr) return NULL;

  switch (scr->method) {
  case MD_SCR_METHOD_PORTAL_PIPEWIRE: {
    pipewire_ctx_t* ctx = scr->pw_ctx;
    if (!ctx) return NULL;

    ctx->frame_ready = 0;

    struct timespec timeout = { 1, 0 }; // 1 second frame timeout
    while (!ctx->frame_ready && !ctx->capture_failed) {
      struct pw_loop *loop = pw_main_loop_get_loop(ctx->loop);
      if (pw_loop_iterate(loop, 100) < 0) {
        break;
      }
    }

    if (ctx->frame_ready && ctx->frame_data) {
      scr->Geometry.Resolution.x = ctx->width;
      scr->Geometry.Resolution.y = ctx->height;
      scr->Geometry.LineSize = ctx->stride;
      return ctx->frame_data;
    }
    break;
  }

  case MD_SCR_METHOD_WLR_SCREENCOPY: {
    wayland_screencap_t* ctx = scr->wl_ctx;
    if (!ctx) return NULL;

    uint8_t* frame_data = wayland_capture_frame(ctx);
    if (frame_data) {
      scr->Geometry.Resolution.x = ctx->width;
      scr->Geometry.Resolution.y = ctx->height;
      scr->Geometry.LineSize = ctx->stride;
    }
    return frame_data;
  }

  default:
    break;
  }

  return NULL;
}
sint32_t MD_SCR_Get_Resolution(MD_SCR_Resolution_t* Resolution) {
  MD_SCR_t temp_scr;
  memset(&temp_scr, 0, sizeof(temp_scr));
  temp_scr.method = MD_SCR_METHOD_AUTO;

  if (MD_SCR_open(&temp_scr) == 0) {
    Resolution->x = temp_scr.Geometry.Resolution.x;
    Resolution->y = temp_scr.Geometry.Resolution.y;
    MD_SCR_close(&temp_scr);
    return 0;
  }

  Resolution->x = 0;
  Resolution->y = 0;
  return -1;
}
