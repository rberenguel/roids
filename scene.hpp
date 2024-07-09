// Copyright 2024 Ruben Berenguel
#pragma once

#include <vector>

#include "./base.hpp"
#include "32blit.hpp"  // NOLINT(build/include_subdir)

class Scene {
 public:
  virtual void update(uint32_t tick) = 0;
  virtual void render(uint32_t tick) = 0;

  // Some things are needed for the "intro", so I might as
  // well just put them here.
  const double PLASMA_BULLET_ACCEL = 3.5;
  const double PHOTON_TORPEDO_ACCEL = 6.5;
  const int MAX_TORPEDO_ENERGY = 200;
  const struct Point shipOuter[5] = {
      {-7, 5}, {7, 0}, {-7, -5}, {-3, 0}, {-7, 5}};
  const struct Point shipMiddle[5] = {
      {-6, -4}, {6, 0}, {-6, 4}, {-2, 0}, {-6, -4}};
  const struct Point shipInner[5] = {
      {-5, -3}, {5, 0}, {-5, 3}, {-1, 0}, {-5, -3}};
  std::vector<Entity> flameList = {};
  std::vector<Entity> shotList = {};
  std::vector<Entity> torpedoList = {};

  bool availableFlame = false;
  bool availableShot = false;
  bool availableTorpedo = false;

  Entity player;
  void setupPlayer();
  void drawEntity(Entity *entity);
  void moveEntity(Entity *entity);
  void addFlame(Point center, Velocity velocity, int baseEnergy);
  void addPlasmaBullet();
  void addPhotonTorpedo();
  void purge(std::vector<Entity> *entityList, bool *availableEntity);
  void drawFlames();
  void drawShots();
};
