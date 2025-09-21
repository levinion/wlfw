#pragma once

#include <wayland-client-protocol.h>
#include <array>
#include <functional>
#include <memory>
#include "xdg-shell-client-protocol.h"

namespace wlfw {

class Client;

using Color = std::array<uint8_t, 4>;

class Surface {
public:
  int width();
  int height();
  int size();
  unsigned char* pixels();
  Color at(int x, int y);
  void set(int x, int y, Color color);
  void fill(int x, int y, int width, int height, Color color);
  void commit();
  void attach();
  void resize(int width, int height);
  void damage(int x, int y, int width, int height);
  void on_configure(std::function<void()> callback);
  void on_pointer_enter(std::function<void(double sx, double sy)> callback);
  void on_pointer_leave(std::function<void()> callback);
  void on_pointer_motion(std::function<void(double sx, double sy)> callback);
  void on_pointer_button(
    std::function<void(uint32_t button, uint32_t state)> callback
  );
  void
  on_pointer_axis(std::function<void(uint32_t axis, double value)> callback);
  void on_keyboard_enter(std::function<void()> callback);
  void on_keyboard_leave(std::function<void()> callback);
  void
  on_keyboard_key(std::function<void(uint32_t key, uint32_t state)> callback);

  ~Surface();

private:
  friend class Toplevel;
  friend class LayerShell;
  friend class Seat;

  static std::unique_ptr<Surface> init(Client* client);

  void _init(Client* client);

  void create_buffer(
    int32_t offset,
    int32_t width,
    int32_t height,
    int32_t channel,
    uint32_t format
  );

  wl_surface* wl_surface_;
  void* data;
  int fd;
  wl_shm_pool* wl_shm_pool_;
  int wl_shm_pool_size;
  wl_buffer* wl_buffer_;
  int width_;
  int height_;
  Client* client;
  std::function<void()> on_configure_ = []() {};
  std::function<void(double, double)> on_pointer_enter_ =
    [](double sx, double sy) {};
  std::function<void()> on_pointer_leave_ = []() {};
  std::function<void(double, double)> on_pointer_motion_ =
    [](double sx, double sy) {};
  std::function<void(uint32_t, uint32_t)> on_pointer_button_ =
    [](uint32_t button, uint32_t state) {};
  std::function<void(uint32_t, double)> on_pointer_axis_ =
    [](uint32_t axis, double value) {};
  std::function<void()> on_keyboard_enter_ = []() {};
  std::function<void()> on_keyboard_leave_ = []() {};
  std::function<void(uint32_t, uint32_t)> on_keyboard_key_ =
    [](uint32_t key, uint32_t state) {};
};

} // namespace wlfw
