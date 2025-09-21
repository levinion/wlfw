#pragma once

#include <functional>
#include <memory>
#include "wlfw/surface.hpp"
#include <xdg-shell-client-protocol.h>

namespace wlfw {

struct ToplevelOpt {
  int width = 800;
  int height = 600;
};

class Toplevel {
public:
  void set_title(std::string title);
  void set_app_id(std::string app_id);
  Surface* get_surface();
  void on_close(std::function<void()> callback);
  void on_configure(std::function<void()> callback);

private:
  friend class Client;
  static std::unique_ptr<Toplevel> init(Client* client, ToplevelOpt opt);
  void _init(Client* client, ToplevelOpt opt);

  std::unique_ptr<Surface> surface;
  xdg_surface* xdg_surface_;
  xdg_toplevel* xdg_toplevel_;
  Client* client;

  std::function<void()> on_close_ = []() {};
  std::function<void()> on_configure_ = []() {};

  int default_width;
  int default_height;
};
} // namespace wlfw
