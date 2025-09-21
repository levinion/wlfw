#pragma once

#include <wayland-util.h>
#include <functional>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include "wlfw/client.hpp"

namespace wlfw {
class ClientBuilder {
public:
  static std::unique_ptr<ClientBuilder> init();
  ClientBuilder* bind(std::string&& interface);
  std::unique_ptr<Client> build();

private:
  friend class Client;
  std::unordered_set<std::string> interfaces;
  static std::unordered_map<
    std::string,
    std::
      tuple<const wl_interface*, std::type_index, std::function<void(Client*)>>>
    interface_table;
};
} // namespace wlfw
