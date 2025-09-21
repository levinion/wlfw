#include "wlfw/layer_shell.hpp"
#include <wayland-client-protocol.h>
#include "wlfw/client.hpp"
#include <cassert>
#include <memory>
#include "wlr-layer-shell-unstable-v1-protocol.h"

namespace wlfw {

std::unique_ptr<LayerShell> LayerShell::init(Client* client) {
  auto layer_shell = std::make_unique<LayerShell>();
  layer_shell->_init(client);
  return layer_shell;
}

void LayerShell::_init(Client* client) {
  this->client = client;
  this->zwlr_layer_shell_ = client->get<zwlr_layer_shell_v1*>();
  assert(this->zwlr_layer_shell_);
  this->surface = Surface::init(client);
  this->zwlr_layer_surface_ = zwlr_layer_shell_v1_get_layer_surface(
    this->zwlr_layer_shell_,
    this->surface->wl_surface_,
    nullptr,
    ZWLR_LAYER_SHELL_V1_LAYER_TOP,
    ""
  );
  assert(this->zwlr_layer_surface_);

  static auto listener = zwlr_layer_surface_v1_listener {
    .configure =
      [](
        void* data,
        zwlr_layer_surface_v1* surface,
        uint32_t serial,
        uint32_t width,
        uint32_t height
      ) {
        assert(width != 0 && height != 0);
        zwlr_layer_surface_v1_ack_configure(surface, serial);
        auto layer_shell = static_cast<LayerShell*>(data);
        if (!layer_shell->surface->wl_buffer_) {
          layer_shell->surface
            ->create_buffer(0, width, height, 4, WL_SHM_FORMAT_ARGB8888);
        } else {
          layer_shell->surface->resize(width, height);
        }
        layer_shell->on_configure_();
        layer_shell->surface->on_configure_();
      },
    .closed =
      [](void* data, zwlr_layer_surface_v1* surface) {
        zwlr_layer_surface_v1_destroy(surface);
      },
  };

  zwlr_layer_surface_v1_add_listener(
    this->zwlr_layer_surface_,
    &listener,
    this
  );

  this->surface->commit();
}

void LayerShell::on_close(std::function<void()> callback) {
  this->on_close_ = callback;
}

void LayerShell::on_configure(std::function<void()> callback) {
  this->on_configure_ = callback;
}

void LayerShell::set_keyboard_interactivity(bool flag) {
  zwlr_layer_surface_v1_set_keyboard_interactivity(
    this->zwlr_layer_surface_,
    flag
  );
}

void LayerShell::set_size(uint32_t width, uint32_t height) {
  zwlr_layer_surface_v1_set_size(this->zwlr_layer_surface_, width, height);
}

void LayerShell::set_anchor(uint32_t anchor) {
  zwlr_layer_surface_v1_set_anchor(this->zwlr_layer_surface_, anchor);
}

void LayerShell::set_layer(uint32_t layer) {
  zwlr_layer_surface_v1_set_layer(this->zwlr_layer_surface_, layer);
}

Surface* LayerShell::get_surface() {
  return this->surface.get();
}

void LayerShell::set_exclusive_zone(int32_t zone) {
  zwlr_layer_surface_v1_set_exclusive_zone(this->zwlr_layer_surface_, zone);
}

LayerShell::~LayerShell() {
  zwlr_layer_surface_v1_destroy(this->zwlr_layer_surface_);
  wl_surface_destroy(this->surface->wl_surface_);
}

} // namespace wlfw
