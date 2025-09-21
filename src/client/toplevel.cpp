#include "wlfw/toplevel.hpp"
#include <functional>
#include <memory>
#include "wlfw/client.hpp"
#include "wlfw/surface.hpp"
#include <wayland-client-protocol.h>
#include <xdg-shell-client-protocol.h>

namespace wlfw {
std::unique_ptr<Toplevel> Toplevel::init(Client* client, ToplevelOpt opt) {
  auto toplevel = std::make_unique<Toplevel>();
  toplevel->_init(client, opt);
  return toplevel;
}

void Toplevel::_init(Client* client, ToplevelOpt opt) {
  this->client = client;
  this->default_width = opt.width;
  this->default_height = opt.height;
  this->surface = Surface::init(client);
  this->surface->create_buffer(
    0,
    this->default_width,
    this->default_height,
    4,
    WL_SHM_FORMAT_ARGB8888
  );

  auto xdg_wm_base_ = client->get<xdg_wm_base*>();
  assert(xdg_wm_base_);
  this->xdg_surface_ =
    xdg_wm_base_get_xdg_surface(xdg_wm_base_, surface->wl_surface_);
  assert(this->xdg_surface_);

  static auto xdg_surface_listener_ = xdg_surface_listener {
    .configure =
      [](void* data, xdg_surface* xdg_surface_, uint32_t serial) {
        auto surface = static_cast<Surface*>(data);
        xdg_surface_ack_configure(xdg_surface_, serial);
        surface->on_configure_();
      }
  };
  xdg_surface_add_listener(
    this->xdg_surface_,
    &xdg_surface_listener_,
    this->surface.get()
  );

  this->xdg_toplevel_ = xdg_surface_get_toplevel(this->xdg_surface_);
  static auto listener = xdg_toplevel_listener {
    .configure =
      [](
        void* data,
        xdg_toplevel* toplevel,
        int32_t width,
        int32_t height,
        wl_array* a
      ) {
        auto toplevel_ = static_cast<Toplevel*>(data);
        if (width == 0 || height == 0) {
          toplevel_->surface->resize(
            toplevel_->default_width,
            toplevel_->default_height
          );
        } else {
          toplevel_->surface->resize(width, height);
        }
        toplevel_->on_configure_();
      },
    .close =
      [](void* data, xdg_toplevel* toplevel) {
        auto toplevel_ = static_cast<Toplevel*>(data);
        toplevel_->on_close_();
        toplevel_->client->dispatcher->quit();
      },
    .configure_bounds =
      [](void* data, xdg_toplevel* toplevel, int32_t width, int32_t height) {},
    .wm_capabilities =
      [](void* data, xdg_toplevel* toplevel, wl_array* capabilities) {}
  };
  xdg_toplevel_add_listener(this->xdg_toplevel_, &listener, this);
  this->surface->commit();
}

void Toplevel::set_title(std::string title) {
  xdg_toplevel_set_title(this->xdg_toplevel_, title.data());
}

void Toplevel::set_app_id(std::string app_id) {
  xdg_toplevel_set_app_id(this->xdg_toplevel_, app_id.data());
}

Surface* Toplevel::get_surface() {
  return this->surface.get();
}

void Toplevel::on_close(std::function<void()> callback) {
  this->on_close_ = callback;
}

void Toplevel::on_configure(std::function<void()> callback) {
  this->on_configure_ = callback;
}

} // namespace wlfw
