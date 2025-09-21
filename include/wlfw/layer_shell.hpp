#pragma once

#include <functional>
#include <memory>
#include "wlfw/surface.hpp"
#include <wlr-layer-shell-unstable-v1-protocol.h>

namespace wlfw {
class LayerShell {
public:
  Surface* get_surface();
  void set_keyboard_interactivity(bool flag);
  void set_size(uint32_t width, uint32_t height);
  void set_anchor(uint32_t anchor);
  void set_layer(uint32_t layer);
  void set_exclusive_zone(int32_t zone);

  void on_close(std::function<void()> callback);
  void on_configure(std::function<void()> callback);

  ~LayerShell();

private:
  friend class Client;
  static std::unique_ptr<LayerShell> init(Client* client);
  void _init(Client* client);

  std::unique_ptr<Surface> surface;
  zwlr_layer_shell_v1* zwlr_layer_shell_;
  zwlr_layer_surface_v1* zwlr_layer_surface_;
  Client* client;

  std::function<void()> on_close_ = []() {};
  std::function<void()> on_configure_ = []() {};
};
} // namespace wlfw
