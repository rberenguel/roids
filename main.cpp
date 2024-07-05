// Copyright 2024 Ruben Berenguel
#include "./main.hpp"

#include "./base.hpp"
#include "./menus.hpp"
#include "./scene.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

GameState state = Welcome;
Scene *scene;  // TODO(me) can I do this with a unique_ptr instead? So far I
               // haven't found a way

void init() {
  blit::set_screen_mode(blit::ScreenMode::hires);
  scene = new WelcomeScreen(0);
}

void update(uint32_t tick) { scene->update(tick); }

void render(uint32_t tick) {
  blit::screen.pen = DARKGREY;
  blit::screen.h_span(blit::Point{0, 0}, blit::screen.bounds.w);
  blit::screen.v_span(blit::Point{blit::screen.bounds.w - 1, 0},
                      blit::screen.bounds.h);
  blit::screen.h_span(blit::Point{0, blit::screen.bounds.h - 1},
                      blit::screen.bounds.w);
  blit::screen.v_span(blit::Point{0, 0}, blit::screen.bounds.h);
  scene->render(tick);
}
