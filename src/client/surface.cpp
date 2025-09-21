#include "wlfw/surface.hpp"
#include <sys/mman.h>
#include <wayland-client-protocol.h>
#include <cassert>
#include <memory>
#include "wlfw/client.hpp"
#include <fcntl.h>

namespace wlfw {
std::unique_ptr<Surface> Surface::init(Client* client) {
  auto surface = std::make_unique<Surface>();
  surface->_init(client);
  return surface;
}

void Surface::_init(Client* client) {
  this->client = client;
  auto wl_compositor_ = client->get<wl_compositor*>();
  assert(wl_compositor_);

  this->wl_surface_ = wl_compositor_create_surface(wl_compositor_);
  wl_surface_set_user_data(this->wl_surface_, this);
}

void Surface::create_buffer(
  int32_t offset,
  int32_t width,
  int32_t height,
  int32_t channel,
  uint32_t format
) {
  this->width_ = width;
  this->height_ = height;

  this->fd = memfd_create("", MFD_CLOEXEC);
  assert(this->fd != -1);
  this->wl_shm_pool_size = height * width * channel;
  auto ret = ftruncate(this->fd, this->wl_shm_pool_size);
  assert(ret != -1);
  this->data = mmap(
    NULL,
    this->wl_shm_pool_size,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    this->fd,
    0
  );
  assert(this->data && this->data != (void*)-1);
  auto shm = client->get<wl_shm*>();
  assert(shm);
  this->wl_shm_pool_ =
    wl_shm_create_pool(shm, this->fd, this->wl_shm_pool_size);
  this->wl_buffer_ = wl_shm_pool_create_buffer(
    this->wl_shm_pool_,
    offset,
    width,
    height,
    width * channel,
    format
  );
}

int Surface::width() {
  return this->width_;
}

int Surface::height() {
  return this->height_;
}

int Surface::size() {
  return this->width_ * this->height_;
}

unsigned char* Surface::pixels() {
  return static_cast<unsigned char*>(this->data);
}

Color Surface::at(int x, int y) {
  auto offset = (y * this->width_ + x) * 4;
  auto pixels = this->pixels();
  return { pixels[offset],
           pixels[offset + 1],
           pixels[offset + 2],
           pixels[offset + 3] };
}

void Surface::set(int x, int y, Color color) {
  auto offset = (y * this->width_ + x) * 4;
  assert(offset + 3 < this->wl_shm_pool_size);
  auto pixels = this->pixels();
  pixels[offset] = color[0];
  pixels[offset + 1] = color[1];
  pixels[offset + 2] = color[2];
  pixels[offset + 3] = color[3];
}

void Surface::fill(int x, int y, int width, int height, Color color) {
  for (int i = y; i < std::min(y + height, this->height_); i++)
    for (int j = x; j < std::min(x + width, this->width_); j++)
      this->set(j, i, color);
}

Surface::~Surface() {
  wl_buffer_destroy(this->wl_buffer_);
  wl_shm_pool_destroy(this->wl_shm_pool_);
  wl_surface_destroy(this->wl_surface_);
  auto ret = munmap(this->data, this->size() * 4);
  assert(ret != -1);
  close(this->fd);
}

void Surface::commit() {
  wl_surface_commit(this->wl_surface_);
  wl_display_flush(this->client->display);
}

void Surface::attach() {
  wl_surface_attach(this->wl_surface_, this->wl_buffer_, 0, 0);
}

void Surface::on_configure(std::function<void()> callback) {
  this->on_configure_ = callback;
}

void Surface::resize(int width, int height) {
  if (width == this->width_ && height == this->height_)
    return;

  this->width_ = width;
  this->height_ = height;
  auto pool_size = width * height * 4;

  if (this->wl_shm_pool_size < pool_size) {
    wl_shm_pool_resize(this->wl_shm_pool_, pool_size);
    auto ret = munmap(this->data, this->wl_shm_pool_size);
    this->wl_shm_pool_size = pool_size;
    assert(ret != -1);
    ret = ftruncate(this->fd, this->wl_shm_pool_size);
    assert(ret != -1);
    this->data = mmap(
      NULL,
      this->wl_shm_pool_size,
      PROT_READ | PROT_WRITE,
      MAP_SHARED,
      this->fd,
      0
    );
    assert(this->data && this->data != (void*)-1);
  }

  wl_buffer_destroy(this->wl_buffer_);
  this->wl_buffer_ = wl_shm_pool_create_buffer(
    this->wl_shm_pool_,
    0,
    width,
    height,
    width * 4,
    WL_SHM_FORMAT_ARGB8888
  );
}

void Surface::damage(int x, int y, int width, int height) {
  wl_surface_damage_buffer(this->wl_surface_, x, y, width, height_);
}

void Surface::on_pointer_enter(
  std::function<void(double sx, double sy)> callback
) {
  this->on_pointer_enter_ = callback;
}

void Surface::on_pointer_leave(std::function<void()> callback) {
  this->on_pointer_leave_ = callback;
}

void Surface::on_pointer_motion(
  std::function<void(double sx, double sy)> callback
) {
  this->on_pointer_motion_ = callback;
}

void Surface::on_pointer_button(
  std::function<void(uint32_t button, uint32_t state)> callback
) {
  this->on_pointer_button_ = callback;
}

void Surface::on_pointer_axis(
  std::function<void(uint32_t axis, double value)> callback
) {
  this->on_pointer_axis_ = callback;
}

void Surface::on_keyboard_key(
  std::function<void(uint32_t key, uint32_t state)> callback
) {
  this->on_keyboard_key_ = callback;
}

} // namespace wlfw
