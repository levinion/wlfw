#pragma once

#include <wayland-client-protocol.h>
#include <memory>

namespace wlfw {
class Client;
class Surface;

class Seat {
public:
private:
  friend class Client;
  friend class ClientBuilder;
  static std::unique_ptr<Seat> init(Client* client);
  void _init(Client* client);
  wl_seat* wl_seat_;
  wl_pointer* wl_pointer_;
  wl_keyboard* wl_keyboard_;
  Client* client;
  Surface* pointer_focused;
  Surface* keyboard_focused;
};
} // namespace wlfw
