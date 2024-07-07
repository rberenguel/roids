// Copyright 2024 Ruben Berenguel

#pragma once
#include <list>
#include <memory>
#include <vector>

#include "./base.hpp"
#include "./scene.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

class Asteroid;

class GameScreen : public Scene {
 public:
  GameScreen();
  const float RSPEED = 0.02;
  const float ACCEL = .015;
  const uint32_t FIRERATE = 100;      // Gap in milliseconds between shots
  const uint32_t TORPEDORATE = 1000;  // Gap in milliseconds between torpedoes
  bool paused = false;
  uint32_t invulnerable = 0;
  std::vector<Asteroid> roidList = {};
  bool availableRoid = false;
  uint32_t previousShot = 0;
  uint32_t previousTorpedo = 0;
  std::vector<Star> stars;
  int stars_size;

  uint16_t lives = 3;
  uint16_t score = 0;
  uint16_t level = 0;
  float multiplier = 0.9;  // Yeah I know
  uint16_t torpedoes = 3;
  uint16_t addedLife = 0;
  uint16_t addedTorpedo = 0;
  bool showDebug = false;

  void update(uint32_t tick);
  void render(uint32_t tick);

  void addRoid(Asteroid entity);
  bool availableEphemeral = false;
  std::vector<Entity> ephemeralsList = {};
  void drawEphemerals();
  void explodeAt(int count, Point center, Velocity velocity, Point angleFan);
  void explode(Entity player);
  std::vector<Star> starfield(int w, int h, int star_size);
  void drawStars();
  void addSomeRoids();
  void drawRoids(uint32_t tick);
  void showInfo(uint32_t tick);

  bool shotCollisionDetection(Asteroid *roid, std::list<Asteroid> *toAdd);
  bool shotCollisionDetectionLoop(Asteroid *roid, Entity *shot,
                                  std::list<Asteroid> *toAdd);
  bool breakUpRoid(Asteroid *roid, Velocity v, std::list<Asteroid> *toAdd);
  bool playerCollision(Asteroid *roid, std::list<Asteroid> *toAdd,
                       uint32_t tick);
  void elasticCollision(Asteroid *roid);
};

class Asteroid : public Entity {
  std::shared_ptr<GameScreen> game = nullptr;

 public:
  float size = 0;
  Asteroid();
  Asteroid(Point center, Velocity velocity, int sides, float size, float spin,
           float playerR);
};

template <typename T>
std::vector<T> hardPurge(std::vector<T> *entityList, bool *availableEntity) {
  std::vector<T> purged;

  std::copy_if(entityList->begin(), entityList->end(),
               std::back_inserter(purged),
               [](const T &entity) { return entity.energy > 0; });

  (*entityList).clear();
  *availableEntity = false;
  return purged;
}

// template <> void addToList<Asteroid>(Asteroid entity, std::vector<Asteroid>
// *entityList, bool *availableInList);

// void addToList(Asteroid entity, std::vector<Asteroid> *entityList,
//                bool *availableInList);
