#include "wlfw/client.hpp"
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include "wlfw/builder.hpp"
#include "wlfw/seat.hpp"
#include "wlfw/toplevel.hpp"
#include "wlfw/wlfw.hpp"
#include <sys/mman.h>
#include <functional>
#include "wlfw/seat.hpp"

namespace wlfw {

std::unique_ptr<Client> Client::init() {
  auto client = std::make_unique<Client>();
  client->_init();
  return client;
}

void Client::_init() {
  this->display = wl_display_connect(nullptr);
  this->dispatcher = Dispatcher<1024>::init();
  this->dispatcher->add_task(wl_display_get_fd(this->display), [this]() {
    return wl_display_dispatch(this->display);
  });
  this->set(wl_display_get_registry(this->display));
  static auto listener = wl_registry_listener {
    .global =
      [](
        void* data,
        wl_registry* registry,
        uint32_t name,
        const char* interface,
        uint32_t version
      ) {
        auto client = static_cast<Client*>(data);
        if (client->interfaces.contains(interface)) {
          auto& [interface_, type_index, callback] =
            ClientBuilder::interface_table.at(interface);
          auto instance = wl_registry_bind(registry, name, interface_, version);
          client->set(type_index, instance);
          callback(client);
        }
      },
    .global_remove = [](void* data, wl_registry* registry, uint32_t name) {}
  };
  auto registry = this->get<wl_registry*>();
  wl_registry_add_listener(registry, &listener, this);
}

bool Client::dispatch() {
  return this->dispatcher->dispatch();
}

std::unique_ptr<Toplevel> Client::create_toplevel(ToplevelOpt opt) {
  return Toplevel::init(this, opt);
}

std::unique_ptr<LayerShell> Client::create_layer_shell(LayerShellOpt opt) {
  return LayerShell::init(this, opt);
}

} // namespace wlfw
