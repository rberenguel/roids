// Copyright 2024 Ruben Berenguel
#pragma once
#include "./base.hpp"
#include "./scene.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

extern GameState state;
extern Scene *scene;

void init();
void update(uint32_t tick);
void render(uint32_t tick);
