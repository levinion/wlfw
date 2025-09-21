#include <print>
#include "wlfw/builder.hpp"
#include "wlfw/wlfw.hpp"
#include <linux/input-event-codes.h>
#include <wayland-client-protocol.h>

int main() {
  auto client = wlfw::ClientBuilder::init()
                  ->bind("wl_compositor")
                  ->bind("wl_shm")
                  ->bind("xdg_wm_base")
                  ->bind("wl_seat")
                  ->build();

  auto toplevel = client->create_toplevel({});
  toplevel->set_title("wlfw example");
  toplevel->set_app_id("wlfw");

  auto surface = toplevel->get_surface();

  surface->on_configure([=]() {
    surface
      ->fill(0, 0, surface->width(), surface->height(), { 255, 255, 0, 255 });
    std::println("debug: surface configured");
    surface->attach();
    surface->damage(0, 0, surface->width(), surface->height());
    surface->commit();
  });

  auto static flag = false;

  surface->on_pointer_motion([=](double sx, double sy) {
    std::println("pointer motion: {}:{}", sx, sy);
    if (!flag)
      return;
    surface->set(sx, sy, { 255, 0, 255, 255 });
    surface->attach();
    surface->damage(0, 0, surface->width(), surface->height());
    surface->commit();
  });

  surface->on_pointer_button([=](uint32_t button, uint32_t state) {
    std::println("pointer button: {}:{}", button, state);
    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
      flag = true;
    } else if (button == BTN_LEFT
               && state == WL_POINTER_BUTTON_STATE_RELEASED) {
      flag = false;
    }
  });

  surface->on_keyboard_key([=](uint32_t key, uint32_t state) {
    std::println("keyboard key: {} {}", key, state);
    if (key == KEY_Z && state == WL_KEYBOARD_KEY_STATE_PRESSED) {
      surface
        ->fill(0, 0, surface->width(), surface->height(), { 255, 255, 0, 255 });
      surface->attach();
      surface->damage(0, 0, surface->width(), surface->height());
      surface->commit();
    }
  });

  while (client->dispatch());
}
