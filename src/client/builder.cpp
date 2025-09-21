#include "wlfw/builder.hpp"
#include <wayland-client-core.h>
#include <wayland-util.h>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include "wlfw/client.hpp"
#include "wlfw/seat.hpp"
#include "wlr-layer-shell-unstable-v1-protocol.h"

namespace wlfw {

std::unordered_map<
  std::string,
  std::
    tuple<const wl_interface*, std::type_index, std::function<void(Client*)>>>
  ClientBuilder::interface_table = {
    {
      "wl_compositor",
      std::make_tuple(
        &wl_compositor_interface,
        std::type_index(typeid(wl_compositor*)),
        [](Client* client) {}
      ),
    },
    {
      "wl_shm",
      std::make_tuple(
        &wl_shm_interface,
        std::type_index(typeid(wl_shm*)),
        [](Client* client) {}
      ),
    },
    {
      "wl_seat",
      std::make_tuple(
        &wl_seat_interface,
        std::type_index(typeid(wl_seat*)),
        [](Client* client) {}
      ),
    },
    {
      "wl_subcompositor",
      std::make_tuple(
        &wl_subcompositor_interface,
        std::type_index(typeid(wl_subcompositor*)),
        [](Client* client) {}
      ),
    },
    {
      "xdg_wm_base",
      std::make_tuple(
        &xdg_wm_base_interface,
        std::type_index(typeid(xdg_wm_base*)),
        [](Client* client) {
          auto xdg_wm_base_ = client->get<xdg_wm_base*>();
          static auto listener = xdg_wm_base_listener {
            .ping =
              [](void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial) {
                xdg_wm_base_pong(xdg_wm_base, serial);
              }
          };
          xdg_wm_base_add_listener(xdg_wm_base_, &listener, client);
        }
      ),
    },
    {
      "zwlr_layer_shell_v1",
      std::make_tuple(
        &zwlr_layer_shell_v1_interface,
        std::type_index(typeid(zwlr_layer_shell_v1*)),
        [](Client* client) {}
      ),
    }
  };

ClientBuilder* ClientBuilder::bind(std::string&& interface) {
  assert(interface_table.contains(interface));
  this->interfaces.insert(interface);
  return this;
}

std::unique_ptr<ClientBuilder> ClientBuilder::init() {
  return std::make_unique<ClientBuilder>();
}

std::unique_ptr<Client> ClientBuilder::build() {
  auto client = Client::init();
  client->interfaces = std::move(this->interfaces);
  wl_display_roundtrip(client->display);
  client->seat = Seat::init(client.get());
  return client;
}
} // namespace wlfw
