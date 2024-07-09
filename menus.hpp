// Copyright 2024 Ruben Berenguel

#pragma once
#include <array>
#include <list>
#include <string>

#include "./main.hpp"
#include "./scene.hpp"
#include "32blit.hpp"  // NOLINT(build/include_subdir)

class WelcomeScreen : public Scene {
 public:
  explicit WelcomeScreen(uint32_t tick);
  uint32_t startTime = 0;
  void update(uint32_t tick);
  void render(uint32_t tick);
};

class MenuScreen : public Scene {
 public:
  int selected = 0;
  uint32_t startTime = 0;
  explicit MenuScreen(uint32_t initialStartTime)
      : startTime(initialStartTime) {}
  std::string menuOptions[2] = {"Play", "Help"};
  void update(uint32_t tick);
  void render(uint32_t tick);
};

class HelpScreen : public Scene {
 public:
  uint32_t startTime = 0;
  explicit HelpScreen(uint32_t initialStartTime)
      : startTime(initialStartTime) {}
  std::string helpText[12] = {"- Inspired by Hyperoid,",
                              "   by Edward Hutchins",
                              "- Move with the DPAD,",
                              "   shoot everything with A",
                              "- Photon torpedo with X",
                              "- If it hangs, sorry:",
                              "   getting used to C++",
                              "- Extra every...",
                              "   500 pt: life, 100 pt: torpedo",
                              "- Pause with Y",
                              "- Shield with B",
                              "- Thanks rn for the help"};
  void update(uint32_t tick);
  void render(uint32_t tick);
};

class HighScoreScreen : public Scene {
 public:
  uint32_t startTime = 0;
  explicit HighScoreScreen(uint32_t initialStartTime)
      : startTime(initialStartTime) {}
  void update(uint32_t tick);
  void render(uint32_t tick);
};
