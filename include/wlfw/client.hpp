#pragma once

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_set>
#include "wlfw/dispatcher.hpp"
#include <wayland-client.h>
#include "wlfw/surface.hpp"
#include "wlfw/toplevel.hpp"
#include "wlfw/layer_shell.hpp"
#include "wlfw/seat.hpp"

namespace wlfw {

class Surface;

class Client {
public:
  bool dispatch();
  std::unique_ptr<Toplevel> create_toplevel(ToplevelOpt opt);
  std::unique_ptr<LayerShell> create_layer_shell(LayerShellOpt opt);
  wl_output* current_output();
  Dispatcher<1024>* get_dispatcher();

  template<typename T>
    requires std::is_pointer_v<T>
  void set(T t) {
    this->storage[std::type_index(typeid(T))] = t;
  }

  void set(std::type_index type_index, void* t) {
    this->storage[type_index] = t;
  }

  template<typename T>
    requires std::is_pointer_v<T>
  T get() {
    auto type_index = std::type_index(typeid(T));
    if (!this->storage.contains(type_index))
      return nullptr;
    return static_cast<T>(this->storage[type_index]);
  }

private:
  friend class Toplevel;
  friend class Surface;
  friend class ClientBuilder;
  static std::unique_ptr<Client> init();
  void _init();
  std::unique_ptr<Dispatcher<1024>> dispatcher;
  wl_display* display;
  // interface
  std::unordered_set<std::string> interfaces;
  std::unordered_map<std::type_index, void*> storage;
  std::unique_ptr<Seat> seat;
};

} // namespace wlfw
