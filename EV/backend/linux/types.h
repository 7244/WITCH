typedef struct EV_t EV_t;

struct EV_t{
  int epoll_fd;

  bool Stop;
};
