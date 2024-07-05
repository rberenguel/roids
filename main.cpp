// Copyright 2024 Ruben Berenguel
#include "./main.hpp"

#include "./base.hpp"
#include "./menus.hpp"
#include "./scene.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

GameState state = Welcome;
std::shared_ptr<Scene> scene =
    nullptr;  // I can get by with a shared pointer and not a unique one because
              // in the scene update methods I destroy "this". With the shared
              // pointer "this" will live long enough to become "that" and be
              // destroyed later. Thanks rn for explaining (better and more
              // right than this comment)

void init() {
  blit::set_screen_mode(blit::ScreenMode::hires);
  scene = std::make_shared<WelcomeScreen>(WelcomeScreen(0));
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
