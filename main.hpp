// Copyright 2024 Ruben Berenguel
#pragma once
#include <memory>

#include "./base.hpp"
#include "./scene.hpp"
#include "32blit.hpp"  // NOLINT(build/include_subdir)

extern GameState state;
extern std::shared_ptr<Scene> scene;

void init();
void update(uint32_t tick);
void render(uint32_t tick);
