#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <iostream>
//#include <gio/gio.h>
#define MD_SCR_DEBUG_PRINTS

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

//Link: -ldbus-1 -lpipewire-0.3 -lspa-0.2
#include <dbus/dbus.h>
#include <pipewire/pipewire.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/type-info.h>
#include <spa/debug/types.h>

typedef struct {
  struct pw_loop* loop;
  struct pw_context* context;
  struct pw_core* core;
  struct pw_stream* stream;
  struct spa_hook stream_listener;
  uint8_t* frame_data;
  size_t frame_size;
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint32_t format;
  int frame_ready;
  int capture_failed;
  int connected;
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

// Portal state tracking
typedef struct {
  char* session_handle;
  uint32_t source_id;
  int create_session_done;
  int select_sources_done;
  int start_done;
  int failed;
  char* request_path;
} portal_state_t;

static portal_state_t portal_state = { 0 };

// Wayland screencopy implementation (keeping your existing code)
static void registry_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
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
    ctx->screencopy_manager = (struct zwlr_screencopy_manager_v1*)wl_registry_bind(
      registry, id, &zwlr_screencopy_manager_v1_interface, 3);
  }
  else if (strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
    ctx->output_manager = (struct zxdg_output_manager_v1*)wl_registry_bind(
      registry, id, &zxdg_output_manager_v1_interface, 2);
  }
}

static void registry_global_remove(void* data, struct wl_registry* registry, uint32_t id) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

static void xdg_output_logical_position(void* data, struct zxdg_output_v1* output, int32_t x, int32_t y) {}

static void xdg_output_logical_size(void* data, struct zxdg_output_v1* output, int32_t width, int32_t height) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->width = width;
  ctx->height = height;
  ctx->geometry_received = 1;
}

static void xdg_output_done(void* data, struct zxdg_output_v1* output) {
  wayland_screencap_t* ctx = (wayland_screencap_t*)data;
  ctx->geometry_received = 1;
}

static void xdg_output_name(void* data, struct zxdg_output_v1* output, const char* name) {}
static void xdg_output_description(void* data, struct zxdg_output_v1* output, const char* desc) {}

static const struct zxdg_output_v1_listener xdg_output_listener = {
    .logical_position = xdg_output_logical_position,
    .logical_size = xdg_output_logical_size,
    .done = xdg_output_done,
    .name = xdg_output_name,
    .description = xdg_output_description,
};

// Portal signal handler - fixed to handle all three stages
static DBusHandlerResult portal_signal_handler(DBusConnection* connection, DBusMessage* message, void* user_data) {
  if (!dbus_message_is_signal(message, "org.freedesktop.portal.Request", "Response")) {
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  const char* path = dbus_message_get_path(message);
  if (!path) {
    return DBUS_HANDLER_RESULT_HANDLED;
  }

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Received Response signal from path: %s\n", path);
#endif

  DBusMessageIter args, results_iter, dict_entry, variant_iter;
  uint32_t response_code;

  if (!dbus_message_iter_init(message, &args)) {
    printf("Failed to initialize message iterator\n");
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_UINT32) {
    dbus_message_iter_get_basic(&args, &response_code);
    dbus_message_iter_next(&args);
  }

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Response code: %u\n", response_code);
#endif

  if (response_code != 0) {
    printf("Portal request failed with code: %u\n", response_code);
    portal_state.failed = 1;
    return DBUS_HANDLER_RESULT_HANDLED;
  }

  // Check if this is a CreateSession response
  if (strstr(path, "create_") && !portal_state.create_session_done) {
    if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_ARRAY) {
      dbus_message_iter_recurse(&args, &results_iter);
      while (dbus_message_iter_get_arg_type(&results_iter) == DBUS_TYPE_DICT_ENTRY) {
        dbus_message_iter_recurse(&results_iter, &dict_entry);
        const char* key = NULL;
        if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_STRING) {
          dbus_message_iter_get_basic(&dict_entry, &key);
          dbus_message_iter_next(&dict_entry);
          if (key && strcmp(key, "session_handle") == 0) {
            if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_VARIANT) {
              dbus_message_iter_recurse(&dict_entry, &variant_iter);
              if (dbus_message_iter_get_arg_type(&variant_iter) == DBUS_TYPE_OBJECT_PATH ||
                dbus_message_iter_get_arg_type(&variant_iter) == DBUS_TYPE_STRING) {
                const char* session_handle;
                dbus_message_iter_get_basic(&variant_iter, &session_handle);
                if (portal_state.session_handle) {
                  free(portal_state.session_handle);
                }
                portal_state.session_handle = strdup(session_handle);
                portal_state.create_session_done = 1;
#ifdef MD_SCR_DEBUG_PRINTS
                printf("Received session handle: %s\n", portal_state.session_handle);
#endif
              }
            }
          }
        }
        dbus_message_iter_next(&results_iter);
      }
    }
  }
  // Check if this is a SelectSources response  
  else if (strstr(path, "select_") && !portal_state.select_sources_done) {
    portal_state.select_sources_done = 1;
#ifdef MD_SCR_DEBUG_PRINTS
    printf("SelectSources completed\n");
#endif
  }
  // Check if this is a Start response
  else if (strstr(path, "start_") && !portal_state.start_done) {
    if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_ARRAY) {
      dbus_message_iter_recurse(&args, &results_iter);
      while (dbus_message_iter_get_arg_type(&results_iter) == DBUS_TYPE_DICT_ENTRY) {
        dbus_message_iter_recurse(&results_iter, &dict_entry);
        const char* key = NULL;
        if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_STRING) {
          dbus_message_iter_get_basic(&dict_entry, &key);
          dbus_message_iter_next(&dict_entry);
          if (key && strcmp(key, "streams") == 0) {
            if (dbus_message_iter_get_arg_type(&dict_entry) == DBUS_TYPE_VARIANT) {
              dbus_message_iter_recurse(&dict_entry, &variant_iter);
              if (dbus_message_iter_get_arg_type(&variant_iter) == DBUS_TYPE_ARRAY) {
                DBusMessageIter streams_iter, stream_iter;
                dbus_message_iter_recurse(&variant_iter, &streams_iter);
                if (dbus_message_iter_get_arg_type(&streams_iter) == DBUS_TYPE_STRUCT) {
                  dbus_message_iter_recurse(&streams_iter, &stream_iter);
                  if (dbus_message_iter_get_arg_type(&stream_iter) == DBUS_TYPE_UINT32) {
                    dbus_message_iter_get_basic(&stream_iter, &portal_state.source_id);
                    portal_state.start_done = 1;
#ifdef MD_SCR_DEBUG_PRINTS
                    printf("Portal source ID: %u\n", portal_state.source_id);
#endif
                  }
                }
              }
            }
          }
        }
        dbus_message_iter_next(&results_iter);
      }
    }
  }

  return DBUS_HANDLER_RESULT_HANDLED;
}

static void cleanup_resources(DBusMessage* reply, DBusConnection* conn, DBusMessage* msg, DBusError* error) {
  if (reply) dbus_message_unref(reply);
  if (msg) dbus_message_unref(msg);
  if (dbus_error_is_set(error)) dbus_error_free(error);
  if (conn) {
    dbus_connection_remove_filter(conn, portal_signal_handler, NULL);
    dbus_connection_unref(conn);
  }
  if (portal_state.session_handle) {
    free(portal_state.session_handle);
    portal_state.session_handle = NULL;
  }
  if (portal_state.request_path) {
    free(portal_state.request_path);
    portal_state.request_path = NULL;
  }
}

static int portal_request_screenshare(MD_SCR_t* scr) {
  DBusConnection* conn = NULL;
  DBusMessage* msg = NULL;
  DBusMessage* reply = NULL;
  DBusError error;
  int result = -1;
  char session_token[256];
  char start_token[256];
  char create_token[256];
  char select_token[256];

  memset(&portal_state, 0, sizeof(portal_state));

  long timestamp = time(NULL);
  pid_t pid = getpid();
  snprintf(session_token, sizeof(session_token), "session_%d_%ld", pid, timestamp);
  snprintf(start_token, sizeof(start_token), "start_%d_%ld", pid, timestamp + 1);
  snprintf(create_token, sizeof(create_token), "create_%d_%ld", pid, timestamp + 2);
  snprintf(select_token, sizeof(select_token), "select_%d_%ld", pid, timestamp + 3);

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Using tokens: session=%s, create=%s, select=%s, start=%s\n",
    session_token, create_token, select_token, start_token);
#endif

  dbus_error_init(&error);
  conn = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (dbus_error_is_set(&error)) {
    printf("D-Bus connection error: %s\n", error.message);
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  char match_rule[512];
  snprintf(match_rule, sizeof(match_rule),
    "type='signal',"
    "sender='org.freedesktop.portal.Desktop',"
    "interface='org.freedesktop.portal.Request',"
    "member='Response'");

  dbus_bus_add_match(conn, match_rule, &error);
  if (dbus_error_is_set(&error)) {
    printf("Failed to add match rule: %s\n", error.message);
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  dbus_connection_add_filter(conn, portal_signal_handler, NULL, NULL);
  dbus_connection_flush(conn);

  // Step 1: Create Session
#ifdef MD_SCR_DEBUG_PRINTS
  printf("Step 1: Creating session...\n");
#endif

  msg = dbus_message_new_method_call("org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "CreateSession");
  if (!msg) {
    printf("Failed to create CreateSession message\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  DBusMessageIter args, options, option, variant;
  dbus_message_iter_init_append(msg, &args);

  if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* session_token_key = "session_handle_token";
  const char* session_token_ptr = session_token;
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &session_token_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "s", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &session_token_ptr) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* handle_token_key = "handle_token";
  const char* create_token_ptr = create_token;
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &handle_token_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "s", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &create_token_ptr) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (!dbus_message_iter_close_container(&args, &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  reply = dbus_connection_send_with_reply_and_block(conn, msg, 10000, &error);
  if (dbus_error_is_set(&error)) {
    printf("CreateSession error: %s\n", error.message);
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (reply) {
    dbus_message_unref(reply);
    reply = NULL;
  }
  dbus_message_unref(msg);
  msg = NULL;

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Waiting for CreateSession response...\n");
#endif

  int timeout = 15000;
  while (!portal_state.create_session_done && !portal_state.failed && timeout > 0) {
    dbus_connection_read_write_dispatch(conn, 100);
    timeout -= 100;
    usleep(100000);
  }

  if (!portal_state.create_session_done || portal_state.failed) {
    printf("CreateSession failed or timed out\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  // Step 2: Select Sources
#ifdef MD_SCR_DEBUG_PRINTS
  printf("Step 2: Selecting sources...\n");
#endif

  msg = dbus_message_new_method_call("org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "SelectSources");
  if (!msg) {
    printf("Failed to create SelectSources message\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  dbus_message_iter_init_append(msg, &args);

  const char* session_handle_ptr = portal_state.session_handle;
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &session_handle_ptr)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* select_token_ptr = select_token;
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &handle_token_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "s", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &select_token_ptr) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* types_key = "types";
  uint32_t source_type = 1; // Monitor
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &types_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "u", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT32, &source_type) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* multiple_key = "multiple";
  dbus_bool_t multiple_val = FALSE;
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &multiple_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "b", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_BOOLEAN, &multiple_val) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (!dbus_message_iter_close_container(&args, &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  // Send SelectSources without blocking - this triggers the user dialog
  if (!dbus_connection_send(conn, msg, NULL)) {
    printf("Failed to send SelectSources message\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }
  dbus_connection_flush(conn);

  dbus_message_unref(msg);
  msg = NULL;

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Waiting for user to select sources (monitor selection dialog)...\n");
#endif

  // Wait for the user to complete source selection
  timeout = 60000; // 60 seconds for user interaction
  while (!portal_state.select_sources_done && !portal_state.failed && timeout > 0) {
    dbus_connection_read_write_dispatch(conn, 100);
    timeout -= 100;
    usleep(100000);
  }

  if (!portal_state.select_sources_done || portal_state.failed) {
    printf("SelectSources failed or timed out\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  // Step 3: Start screen capture
#ifdef MD_SCR_DEBUG_PRINTS
  printf("Step 3: Starting screen capture...\n");
#endif

  msg = dbus_message_new_method_call("org.freedesktop.portal.Desktop",
    "/org/freedesktop/portal/desktop",
    "org.freedesktop.portal.ScreenCast",
    "Start");
  if (!msg) {
    printf("Failed to create Start message\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  dbus_message_iter_init_append(msg, &args);

  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &session_handle_ptr)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* parent_window = "";
  if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &parent_window)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (!dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  const char* start_token_ptr = start_token;
  if (!dbus_message_iter_open_container(&options, DBUS_TYPE_DICT_ENTRY, NULL, &option) ||
    !dbus_message_iter_append_basic(&option, DBUS_TYPE_STRING, &handle_token_key) ||
    !dbus_message_iter_open_container(&option, DBUS_TYPE_VARIANT, "s", &variant) ||
    !dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &start_token_ptr) ||
    !dbus_message_iter_close_container(&option, &variant) ||
    !dbus_message_iter_close_container(&options, &option)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (!dbus_message_iter_close_container(&args, &options)) {
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &error);
  if (dbus_error_is_set(&error)) {
    printf("Start error: %s\n", error.message);
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (reply) {
    dbus_message_unref(reply);
    reply = NULL;
  }
  dbus_message_unref(msg);
  msg = NULL;

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Waiting for Start response...\n");
#endif

  timeout = 15000;
  while (!portal_state.start_done && !portal_state.failed && timeout > 0) {
    dbus_connection_read_write_dispatch(conn, 100);
    timeout -= 100;
    usleep(100000);
  }

  if (!portal_state.start_done || portal_state.failed) {
    printf("Start failed or timed out\n");
    cleanup_resources(reply, conn, msg, &error);
    return -1;
  }

  if (portal_state.session_handle) {
    scr->portal_session_handle = strdup(portal_state.session_handle);
  }
  scr->portal_source_id = portal_state.source_id;
  result = 0;

#ifdef MD_SCR_DEBUG_PRINTS
  printf("Portal setup completed successfully. Source ID: %u\n", scr->portal_source_id);
#endif

  cleanup_resources(reply, conn, msg, &error);
  return result;
}

// PipeWire implementation
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
  ctx->stride = SPA_ROUND_UP_N(format.info.raw.size.width * 4, 4);
  ctx->frame_size = ctx->stride * ctx->height;
  ctx->format = format.info.raw.format;

#ifdef MD_SCR_DEBUG_PRINTS
  const char* format_name = "Unknown";
  switch (format.info.raw.format) {
  case SPA_VIDEO_FORMAT_BGRA:
    format_name = "BGRA";
    break;
  case SPA_VIDEO_FORMAT_BGRx:
    format_name = "BGRx";
    break;
  case SPA_VIDEO_FORMAT_RGBA:
    format_name = "RGBA";
    break;
  case SPA_VIDEO_FORMAT_RGBx:
    format_name = "RGBx";
    break;
  case SPA_VIDEO_FORMAT_ARGB:
    format_name = "ARGB";
    break;
  case SPA_VIDEO_FORMAT_xRGB:
    format_name = "xRGB";
    break;
  }
  printf("PipeWire format: %dx%d, stride: %d, format: %s (%d)\n",
    ctx->width, ctx->height, ctx->stride, format_name, format.info.raw.format);
#endif

  if (ctx->frame_data) {
    free(ctx->frame_data);
  }
  ctx->frame_data = (uint8_t*)malloc(ctx->frame_size);
  if (!ctx->frame_data) {
    ctx->capture_failed = 1;
    return;
  }
}

static void on_stream_process(void* data) {
  pipewire_ctx_t* ctx = (pipewire_ctx_t*)data;
  struct pw_buffer* b;
  struct spa_buffer* buf;

  if ((b = pw_stream_dequeue_buffer(ctx->stream)) == NULL) {
    return;
  }

  buf = b->buffer;
  if (buf->datas[0].data == NULL) {
    pw_stream_queue_buffer(ctx->stream, b);
    return;
  }

  size_t copy_size = buf->datas[0].chunk->size;
  if (copy_size > ctx->frame_size) {
    copy_size = ctx->frame_size;
  }

  if (ctx->frame_data) {
    memcpy(ctx->frame_data, buf->datas[0].data, copy_size);
    ctx->frame_ready = 1;
  }

  pw_stream_queue_buffer(ctx->stream, b);
}

static void on_stream_state_changed(void* data, enum pw_stream_state old,
  enum pw_stream_state state, const char* error) {
  pipewire_ctx_t* ctx = (pipewire_ctx_t*)data;
#ifdef MD_SCR_DEBUG_PRINTS
  printf("PipeWire stream state: %s\n", pw_stream_state_as_string(state));
  if (state == PW_STREAM_STATE_ERROR && error) {
    printf("PipeWire stream error: %s\n", error);
  }
#endif
  switch (state) {
  case PW_STREAM_STATE_ERROR:
  case PW_STREAM_STATE_UNCONNECTED:
    ctx->capture_failed = 1;
    break;
  case PW_STREAM_STATE_STREAMING:
    ctx->connected = 1;
    break;
  default:
    break;
  }
}

static const struct pw_stream_events stream_events = {
  PW_VERSION_STREAM_EVENTS,
  .param_changed = on_stream_param_changed,
  .process = on_stream_process,
  .state_changed = on_stream_state_changed,
};

static int pipewire_init_stream(pipewire_ctx_t* ctx, uint32_t node_id) {
  if (!ctx) return -1;

  pw_init(NULL, NULL);

  ctx->loop = pw_loop_new(NULL);
  if (!ctx->loop) {
    printf("Failed to create PipeWire loop\n");
    return -1;
  }

  ctx->context = pw_context_new(ctx->loop, NULL, 0);
  if (!ctx->context) {
    printf("Failed to create PipeWire context\n");
    pw_loop_destroy(ctx->loop);
    return -1;
  }

  ctx->core = pw_context_connect(ctx->context, NULL, 0);
  if (!ctx->core) {
    printf("Failed to connect to PipeWire\n");
    pw_context_destroy(ctx->context);
    pw_loop_destroy(ctx->loop);
    return -1;
  }

  // Create stream
  ctx->stream = pw_stream_new_simple(
    ctx->loop,
    "screen-capture",
    pw_properties_new(
      PW_KEY_MEDIA_TYPE, "Video",
      PW_KEY_MEDIA_CATEGORY, "Capture",
      PW_KEY_MEDIA_ROLE, "Screen",
      PW_KEY_NODE_LATENCY, "512/48000",     // Add low latency
      PW_KEY_NODE_MAX_LATENCY, "1024/48000", // Add max latency
      NULL),
    &stream_events,
    ctx);

  if (!ctx->stream) {
    printf("Failed to create PipeWire stream\n");
    pw_core_disconnect(ctx->core);
    pw_context_destroy(ctx->context);
    pw_loop_destroy(ctx->loop);
    return -1;
  }

  // Connect to the specific node (monitor source)
  char target[64];
  snprintf(target, sizeof(target), "%u", node_id);

  const struct spa_pod* params[1];
  uint8_t buffer[1024];
  struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));

  struct spa_rectangle def_size = SPA_RECTANGLE(320, 240);
  struct spa_rectangle min_size = SPA_RECTANGLE(1, 1);
  struct spa_rectangle max_size = SPA_RECTANGLE(8192, 8192);

  struct spa_fraction def_fps = SPA_FRACTION(60, 1);
  struct spa_fraction min_fps = SPA_FRACTION(0, 1);
  struct spa_fraction max_fps = SPA_FRACTION(144, 1);

  params[0] = (const struct spa_pod*)spa_pod_builder_add_object(&b,
    SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat,
    SPA_FORMAT_mediaType, SPA_POD_Id(SPA_MEDIA_TYPE_video),
    SPA_FORMAT_mediaSubtype, SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
    SPA_FORMAT_VIDEO_format, SPA_POD_CHOICE_ENUM_Id(4,
      SPA_VIDEO_FORMAT_BGRx,
      SPA_VIDEO_FORMAT_BGRA,
      SPA_VIDEO_FORMAT_BGRx,
      SPA_VIDEO_FORMAT_RGBx),
    SPA_FORMAT_VIDEO_size, SPA_POD_CHOICE_RANGE_Rectangle(&def_size, &min_size, &max_size),
    SPA_FORMAT_VIDEO_framerate, SPA_POD_CHOICE_RANGE_Fraction(&def_fps, &min_fps, &max_fps));


  if (pw_stream_connect(ctx->stream,
    PW_DIRECTION_INPUT,
    node_id,
    (pw_stream_flags)(PW_STREAM_FLAG_AUTOCONNECT |
      PW_STREAM_FLAG_MAP_BUFFERS),
    params, 1) < 0) {
    printf("Failed to connect PipeWire stream to node %u\n", node_id);
    pw_stream_destroy(ctx->stream);
    pw_core_disconnect(ctx->core);
    pw_context_destroy(ctx->context);
    pw_loop_destroy(ctx->loop);
    return -1;
  }

  return 0;
}

static void pipewire_cleanup(pipewire_ctx_t* ctx) {
  if (!ctx) return;

  if (ctx->frame_data) {
    free(ctx->frame_data);
    ctx->frame_data = NULL;
  }

  if (ctx->stream) {
    pw_stream_destroy(ctx->stream);
    ctx->stream = NULL;
  }

  if (ctx->core) {
    pw_core_disconnect(ctx->core);
    ctx->core = NULL;
  }

  if (ctx->context) {
    pw_context_destroy(ctx->context);
    ctx->context = NULL;
  }

  if (ctx->loop) {
    pw_loop_destroy(ctx->loop);
    ctx->loop = NULL;
  }

  pw_deinit();
}

sint32_t MD_SCR_Get_Resolution(MD_SCR_Resolution_t* Resolution) {
  if (!Resolution) return -1;

  // This is a fallback - actual resolution will be set during capture setup
  Resolution->x = 0;
  Resolution->y = 0;

  return 0;
}

sint32_t MD_SCR_open(MD_SCR_t* scr) {
  if (!scr) return -1;
printf("AAA\n");
  memset(scr, 0, sizeof(MD_SCR_t));

  // Determine the best method based on environment
  const char* wayland_display = getenv("WAYLAND_DISPLAY");
  const char* xdg_session_type = getenv("XDG_SESSION_TYPE");

  if (wayland_display || (xdg_session_type && strcmp(xdg_session_type, "wayland") == 0)) {
    // Try portal first (works on GNOME, KDE, and should work on COSMIC)
    scr->method = MD_SCR_METHOD_PORTAL_PIPEWIRE;

    // Allocate PipeWire context
    scr->pw_ctx = (pipewire_ctx_t*)calloc(1, sizeof(pipewire_ctx_t));
    if (!scr->pw_ctx) {
      return -1;
    }

#ifdef MD_SCR_DEBUG_PRINTS
    printf("Using Portal + PipeWire method\n");
#endif

    // Request screen share through portal
    if (portal_request_screenshare(scr) != 0) {
      printf("Portal screen share request failed\n");
      free(scr->pw_ctx);
      scr->pw_ctx = NULL;

      // Fallback to WLR screencopy if available
      const char* compositor = getenv("XDG_CURRENT_DESKTOP");
      if (compositor && (strcasecmp(compositor, "sway") == 0 ||
        strcasecmp(compositor, "hyprland") == 0 ||
        strstr(compositor, "wlroots"))) {
        scr->method = MD_SCR_METHOD_WLR_SCREENCOPY;
        scr->wl_ctx = (wayland_screencap_t*)calloc(1, sizeof(wayland_screencap_t));
        if (!scr->wl_ctx) {
          return -1;
        }
#ifdef MD_SCR_DEBUG_PRINTS
        printf("Falling back to WLR screencopy method\n");
#endif
      }
      else {
        return -1;
      }
    }

    // Initialize PipeWire stream if using portal method
    if (scr->method == MD_SCR_METHOD_PORTAL_PIPEWIRE) {
      if (pipewire_init_stream(scr->pw_ctx, scr->portal_source_id) != 0) {
        printf("Failed to initialize PipeWire stream\n");
        if (scr->portal_session_handle) {
          free(scr->portal_session_handle);
        }
        free(scr->pw_ctx);
        return -1;
      }

      // Wait for stream to be ready
      int timeout = 5000;
      while (!scr->pw_ctx->connected && !scr->pw_ctx->capture_failed && timeout > 0) {
        pw_loop_iterate(scr->pw_ctx->loop, 100);
        timeout -= 100;
      }

      if (scr->pw_ctx->capture_failed || !scr->pw_ctx->connected) {
        printf("PipeWire stream failed to connect\n");
        pipewire_cleanup(scr->pw_ctx);
        free(scr->pw_ctx);
        if (scr->portal_session_handle) {
          free(scr->portal_session_handle);
        }
        return -1;
      }

      // Set geometry from PipeWire stream
      scr->Geometry.Resolution.x = scr->pw_ctx->width;
      scr->Geometry.Resolution.y = scr->pw_ctx->height;
      scr->Geometry.LineSize = scr->pw_ctx->stride;
    }
  }
  else {
    return -1;
  }

  return 0;
}

void MD_SCR_close(MD_SCR_t* scr) {
  if (!scr) return;

  if (scr->method == MD_SCR_METHOD_PORTAL_PIPEWIRE && scr->pw_ctx) {
    pipewire_cleanup(scr->pw_ctx);
    free(scr->pw_ctx);
    scr->pw_ctx = NULL;
  }

  if (scr->method == MD_SCR_METHOD_WLR_SCREENCOPY && scr->wl_ctx) {
    // Cleanup Wayland resources
    if (scr->wl_ctx->buffer) {
      wl_buffer_destroy(scr->wl_ctx->buffer);
    }
    if (scr->wl_ctx->shm_data && scr->wl_ctx->shm_data != MAP_FAILED) {
      munmap(scr->wl_ctx->shm_data, scr->wl_ctx->shm_size);
    }
    if (scr->wl_ctx->shm_fd >= 0) {
      close(scr->wl_ctx->shm_fd);
    }
    if (scr->wl_ctx->frame) {
      zwlr_screencopy_frame_v1_destroy(scr->wl_ctx->frame);
    }
    if (scr->wl_ctx->xdg_output) {
      zxdg_output_v1_destroy(scr->wl_ctx->xdg_output);
    }
    if (scr->wl_ctx->output_manager) {
      zxdg_output_manager_v1_destroy(scr->wl_ctx->output_manager);
    }
    if (scr->wl_ctx->screencopy_manager) {
      zwlr_screencopy_manager_v1_destroy(scr->wl_ctx->screencopy_manager);
    }
    if (scr->wl_ctx->shm) {
      wl_shm_destroy(scr->wl_ctx->shm);
    }
    if (scr->wl_ctx->compositor) {
      wl_compositor_destroy(scr->wl_ctx->compositor);
    }
    if (scr->wl_ctx->registry) {
      wl_registry_destroy(scr->wl_ctx->registry);
    }
    if (scr->wl_ctx->display) {
      wl_display_disconnect(scr->wl_ctx->display);
    }
    free(scr->wl_ctx);
    scr->wl_ctx = NULL;
  }

  if (scr->portal_session_handle) {
    free(scr->portal_session_handle);
    scr->portal_session_handle = NULL;
  }

  memset(scr, 0, sizeof(MD_SCR_t));
}

uint8_t* MD_SCR_read(MD_SCR_t* scr) {
  if (!scr) return NULL;

  if (scr->method == MD_SCR_METHOD_PORTAL_PIPEWIRE && scr->pw_ctx) {
    // Process PipeWire events
    scr->pw_ctx->frame_ready = 0;

    int timeout = 100; // 1 second timeout
    while (!scr->pw_ctx->frame_ready && !scr->pw_ctx->capture_failed && timeout > 0) {
      pw_loop_iterate(scr->pw_ctx->loop, 5);
      timeout -= 5;
    }
    if (scr->pw_ctx->capture_failed) {
      printf("PipeWire capture failed\n");
      return NULL;
    }

    if (!scr->pw_ctx->frame_ready) {
#ifdef MD_SCR_DEBUG_PRINTS
      printf("No frame ready (timeout)\n");
#endif
      return NULL;
    }
    return scr->pw_ctx->frame_data;
  }

  if (scr->method == MD_SCR_METHOD_WLR_SCREENCOPY && scr->wl_ctx) {
    printf("WLR screencopy read not fully implemented, implement can be found from older wayland.h\n");
    return NULL;
  }

  return NULL;
}

static const char* detect_desktop_environment() {
  const char* desktop = getenv("XDG_CURRENT_DESKTOP");
  if (desktop) {
    if (strcasestr(desktop, "gnome")) return "GNOME";
    if (strcasestr(desktop, "kde")) return "KDE";
    if (strcasestr(desktop, "cosmic")) return "COSMIC";
    if (strcasestr(desktop, "sway")) return "Sway";
    if (strcasestr(desktop, "hyprland")) return "Hyprland";
  }

  const char* gdm_session = getenv("GDMSESSION");
  if (gdm_session) {
    if (strcasestr(gdm_session, "gnome")) return "GNOME";
    if (strcasestr(gdm_session, "cosmic")) return "COSMIC";
  }

  return "Unknown";
}