#include "wlfw/seat.hpp"
#include "wlfw/surface.hpp"
#include "wlfw/client.hpp"
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <memory>

namespace wlfw {
std::unique_ptr<Seat> Seat::init(Client* client) {
  auto seat = std::make_unique<Seat>();
  seat->_init(client);
  return seat;
}

void Seat::_init(Client* client) {
  this->client = client;
  this->wl_seat_ = client->get<wl_seat*>();
  assert(this->wl_seat_);
  this->wl_pointer_ = wl_seat_get_pointer(this->wl_seat_);
  static auto wl_pointer_listener_ = wl_pointer_listener {
    .enter =
      [](
        void* data,
        wl_pointer* wl_pointer,
        uint32_t serial,
        wl_surface* surface,
        wl_fixed_t sx,
        wl_fixed_t sy
      ) {
        double x = wl_fixed_to_double(sx);
        double y = wl_fixed_to_double(sy);
        auto surface_ =
          static_cast<Surface*>(wl_surface_get_user_data(surface));
        auto seat = static_cast<Seat*>(data);
        seat->pointer_focused = surface_;
        surface_->on_pointer_enter_(x, y);
      },
    .leave =
      [](
        void* data,
        wl_pointer* wl_pointer,
        uint32_t serial,
        wl_surface* surface
      ) {
        auto surface_ =
          static_cast<Surface*>(wl_surface_get_user_data(surface));
        auto seat = static_cast<Seat*>(data);
        if (seat->pointer_focused == surface_) {
          seat->pointer_focused = nullptr;
          surface_->on_pointer_leave_();
        } else {
          // TODO: warn: pointer leave sended to an not focused surface
        }
      },
    .motion =
      [](
        void* data,
        wl_pointer* wl_pointer,
        uint32_t time,
        wl_fixed_t sx,
        wl_fixed_t sy
      ) {
        double x = wl_fixed_to_double(sx);
        double y = wl_fixed_to_double(sy);
        auto seat = static_cast<Seat*>(data);
        auto surface = seat->pointer_focused;
        if (!surface)
          return;
        surface->on_pointer_motion_(x, y);
      },
    .button =
      [](
        void* data,
        wl_pointer* wl_pointer,
        uint32_t serial,
        uint32_t time,
        uint32_t button,
        uint32_t state
      ) {
        auto seat = static_cast<Seat*>(data);
        auto surface = seat->pointer_focused;
        if (!surface)
          return;
        surface->on_pointer_button_(button, state);
      },
    .axis =
      [](
        void* data,
        wl_pointer* wl_pointer,
        uint32_t time,
        uint32_t axis,
        wl_fixed_t value
      ) {
        auto seat = static_cast<Seat*>(data);
        auto surface = seat->pointer_focused;
        if (!surface)
          return;
        surface->on_pointer_axis_(axis, wl_fixed_to_double(value));
      },
    .frame = [](void* data, wl_pointer* wl_pointer) {},
    .axis_source =
      [](void* data, wl_pointer* wl_pointer, uint32_t axis_source) {},
    .axis_stop =
      [](void* data, wl_pointer* wl_pointer, uint32_t time, uint32_t axis) {},
    .axis_discrete =
      [](void* data, wl_pointer* wl_pointer, uint32_t axis, int32_t discrete) {
      },
    .axis_value120 =
      [](void* data, wl_pointer* wl_pointer, uint32_t axis, int32_t value120) {
      },
    .axis_relative_direction = [](
                                 void* data,
                                 wl_pointer* wl_pointer,
                                 uint32_t axis,
                                 uint32_t direction
                               ) {},
  };
  wl_pointer_add_listener(this->wl_pointer_, &wl_pointer_listener_, this);

  this->wl_keyboard_ = wl_seat_get_keyboard(this->wl_seat_);
  static auto wl_keyboard_listener_ = wl_keyboard_listener {
    .keymap = [](
                void* data,
                wl_keyboard* wl_keyboard,
                uint32_t format,
                int32_t fd,
                uint32_t size
              ) {},
    .enter =
      [](
        void* data,
        wl_keyboard* wl_keyboard,
        uint32_t serial,
        wl_surface* surface,
        wl_array* keys
      ) {
        auto surface_ =
          static_cast<Surface*>(wl_surface_get_user_data(surface));
        auto seat = static_cast<Seat*>(data);
        seat->keyboard_focused = surface_;
        surface_->on_keyboard_enter_();
      },
    .leave =
      [](
        void* data,
        wl_keyboard* wl_keyboard,
        uint32_t serial,
        wl_surface* surface
      ) {
        auto surface_ =
          static_cast<Surface*>(wl_surface_get_user_data(surface));
        auto seat = static_cast<Seat*>(data);
        if (seat->keyboard_focused == surface_) {
          seat->keyboard_focused = nullptr;
          surface_->on_keyboard_leave_();
        } else {
          // TODO: warn: keyboard leave sended to an not focused surface
        }
      },
    .key =
      [](
        void* data,
        wl_keyboard* wl_keyboard,
        uint32_t serial,
        uint32_t time,
        uint32_t key,
        uint32_t state
      ) {
        auto seat = static_cast<Seat*>(data);
        auto surface_ = seat->keyboard_focused;
        if (!surface_)
          return;
        surface_->on_keyboard_key_(key, state);
      },
    .modifiers =
      [](
        void* data,
        wl_keyboard* wl_keyboard,
        uint32_t serial,
        uint32_t mods_depressed,
        uint32_t mods_latched,
        uint32_t mods_locked,
        uint32_t group
      ) {
        // TODO: use xkbcommon to handle modifiers
      },
    .repeat_info =
      [](void* data, wl_keyboard* wl_keyboard, int32_t rate, int32_t delay) {}
  };
  wl_keyboard_add_listener(this->wl_keyboard_, &wl_keyboard_listener_, this);
}
} // namespace wlfw
