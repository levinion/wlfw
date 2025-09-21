#include <print>
#include "wlfw/builder.hpp"
#include "wlfw/wlfw.hpp"
#include "wlr-layer-shell-unstable-v1-protocol.h"
#include <linux/input-event-codes.h>
#include <wayland-client-protocol.h>

int main() {
  auto client = wlfw::ClientBuilder::init()
                  ->bind("wl_compositor")
                  ->bind("wl_shm")
                  ->bind("xdg_wm_base")
                  ->bind("wl_seat")
                  ->bind("zwlr_layer_shell_v1")
                  ->build();

  auto layer_shell = client->create_layer_shell({});
  layer_shell->set_keyboard_interactivity(false);
  layer_shell->set_size(1920, 1080);
  layer_shell->set_layer(ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND);
  layer_shell->set_exclusive_zone(-1);

  auto surface = layer_shell->get_surface();

  static int width = 0, height = 0;

  surface->on_configure([=]() {
    if (surface->width() == width && surface->height() == height)
      return;
    std::println("surface: {}:{}", surface->width(), surface->height());
    surface
      ->fill(0, 0, surface->width(), surface->height(), { 255, 0, 0, 255 });
    std::println("debug: surface configured");
    surface->attach();
    surface->damage(0, 0, surface->width(), surface->height());
    surface->commit();
    width = surface->width();
    height = surface->height();
  });

  while (client->dispatch());
}
