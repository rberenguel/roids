// Copyright 2024 Ruben Berenguel

#include "./menus.hpp"

#include "./base.hpp"
#include "./game.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

WelcomeScreen::WelcomeScreen(uint32_t tick) {
  startTime = tick;
  flameList = std::vector<Entity>(0);
  shotList = std::vector<Entity>(0);
  setupPlayer();
  player.r = -M_PI / 4;
}

void WelcomeScreen::update(uint32_t tick) {
  if (blit::buttons || tick - startTime >= 10000) {
    state = Menu;
    scene = std::make_shared<MenuScreen>(MenuScreen(tick));
  }
}

void WelcomeScreen::render(uint32_t tick) {
  blit::screen.pen = YELLOW;
  blit::Rect clipRect(
      blit::screen.bounds.w / 3, static_cast<int>(blit::screen.bounds.h / 6),
      static_cast<int>(3.0 * blit::screen.bounds.w / 4), blit::screen.bounds.h);
  blit::screen.text("'Roids", monoid_b, clipRect,
                    blit::TextAlign::center_center);
  blit::screen.text(
      "rb'24", monoid_s,
      blit::Point(blit::screen.bounds.w - 32, blit::screen.bounds.h - 14));
  drawEntity(&player);
  if (tick % 1 == 0) {
    flameList = hardPurge(&flameList, &availableFlame);
    shotList = hardPurge(&shotList, &availableShot);
    Point adjustedCenter = Point{-5, 0};
    float p = 1.8 + 1.2 * blit::random() / (RAND_MAX + 1.0);
    float q = -0.2 + 0.4 * blit::random() / (RAND_MAX + 1.0);
    float j = 0.9 + 0.2 * blit::random() / (RAND_MAX + 1.0);
    int e = 2 + (blit::random() % 3);
    Velocity v = Velocity{p, q};
    rotateTo(&adjustedCenter, &adjustedCenter, j * player.r);
    rotateTo(&v, &v, -3.0 * M_PI / 2.0 + j * M_PI / 4.0);
    addFlame(Point{player.center.x + adjustedCenter.x,
                   player.center.y + adjustedCenter.y},
             v, e);
  }
  drawFlames();
  drawShots();
}

void MenuScreen::update(uint32_t tick) {
  if ((tick > startTime + 200) && (blit::buttons & blit::Button::A)) {
    switch (selected) {
      case 0:
        state = Game;
        scene = std::make_shared<GameScreen>(GameScreen());
        return;
      case 1:
        state = Help;
        scene = std::make_shared<HelpScreen>(HelpScreen(tick));
        return;
      default:
        break;
    }
  }
  if ((tick > startTime + 200) && (blit::buttons & blit::Button::DPAD_UP)) {
    selected = (selected - 1) % 3;
    startTime = tick;
    return;
  }
  if ((tick > startTime + 200) && (blit::buttons & blit::Button::DPAD_DOWN)) {
    selected = (selected + 1) % 3;
    startTime = tick;
    return;
  }
}
void MenuScreen::render(uint32_t tick) {
  blit::screen.pen = BLACK;
  blit::screen.clear();
  int i = 0;
  for (std::string &option : menuOptions) {
    // This should be a function of font size eventually
    int third = static_cast<int>(blit::screen.bounds.h / 3);
    blit::screen.pen = RED;
    if (i == selected) {
      blit::screen.pen = YELLOW;
    }
    blit::Rect clipRect(blit::screen.bounds.w / 4, third + 20 * i,
                        blit::screen.bounds.w, third + 8);
    blit::screen.text(option, monoid_b, clipRect,
                      blit::TextAlign::center_center);
    i++;
  }
}
void HelpScreen::update(uint32_t tick) {
  if ((tick > startTime + 200) && blit::buttons) {
    state = Menu;
    scene = std::make_shared<MenuScreen>(MenuScreen(tick));
  }
}
void HelpScreen::render(uint32_t tick) {
  int i = 0;
  blit::screen.pen = BLACK;
  blit::screen.clear();
  blit::screen.pen = RED;
  for (std::string &txt : helpText) {
    // This should be a function of font size eventually
    int up = static_cast<int>(blit::screen.bounds.h / 8);
    blit::Rect clipRect(3, up + 16 * i, blit::screen.bounds.w, up + 8);
    blit::screen.text(txt, monoid, clipRect, blit::TextAlign::center_h);
    i++;
  }
}

void HighScoreScreen::update(uint32_t tick) {
  // TODO(me) need to figure out saving properly
  state = Menu;
  scene = std::make_shared<MenuScreen>(MenuScreen(tick));
}
void HighScoreScreen::render(uint32_t tick) {
  // TODO(me) need to figure out saving properly
  state = Menu;
  scene = std::make_shared<MenuScreen>(MenuScreen(tick));
}
