#pragma once

enum{
  EV_timer_e,
  EV_event_e,
  EV_io_e,
  EV_tp_e,
  EV_idle_e,
  EV_total_e
};

const char *const EV_method_string_g[EV_total_e] = {
  "timer",
  "event",
  "io",
  "tp",
  "idle"
};
