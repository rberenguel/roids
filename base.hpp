// Copyright 2024 Ruben Berenguel

#pragma once

#include <vector>

#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_TWOPI 2 * M_PI

extern blit::Pen YELLOW;
extern blit::Pen CYAN;
extern blit::Pen SUBTLY_CYAN;
extern blit::Pen DARKCYAN;
extern blit::Pen BRIGHTGREEN;
extern blit::Pen MEDGREEN;
extern blit::Pen GREEN;
extern blit::Pen PINK;
extern blit::Pen RED;
extern blit::Pen MEDRED;
extern blit::Pen DARKRED;
extern blit::Pen DARKERRED;
extern blit::Pen FULLWHITE;
extern blit::Pen WHITE;
extern blit::Pen GREY;
extern blit::Pen LIGHTGREY;
extern blit::Pen DARKGREY;
extern blit::Pen BLACK;

extern const blit::Font monoid;
extern const blit::Font monoid_b;
extern const blit::Font monoid_s;

struct Point {
  Point() : x(0), y(0) {}
  Point(float _x, float _y) : x(_x), y(_y) {}
  float x, y;
};

class HSV {
 public:
  HSV() : h(0.0f), s(0.0f), v(0.0f), a(255) {}
  HSV(float _h, float _s, float _v) : h(_h), s(_s), v(_v) {}
  HSV(float _h, float _s, float _v, int _a) : h(_h), s(_s), v(_v), a(_a) {}

  float h;
  float s;
  float v;
  int a = 255;
  blit::Pen ToPen() {
    auto color = blit::hsv_to_rgba(h, s, v);
    color.a = a;
    return color;
  }
};

uint32_t fastrand(uint32_t);
float fastrand(float);
double fastrand(double ratio);

struct FastRandInt {
  using result_type = uint32_t;

  static constexpr result_type min() { return 0; }

  static constexpr result_type max() { return 10000; }

  result_type operator()() { return fastrand(uint32_t(10000)); }
};

struct Star {
  float x;
  float y;
  HSV color;
};

void poly(Point center, std::vector<Point> points, int cnt);

struct Velocity {
  float x, y;
};

enum class EntityKind {
  kNone,
  kFlame,
  kPlasmaBullet,
  kPhotonTorpedo,
  kRoid,
  kPiece,
};

enum class MeshKind { kPolygon, kPoints, kCircle };
struct Mesh {
  // Point center = Point(0, 0);
  std::vector<Point> points;
  std::vector<Point> presentation;
  float radius = 0;  // Used only for circles
  MeshKind kind;
  blit::Pen color;
  blit::Pen fill;
};

class Entity {
 public:
  Point center = Point{0, 0};
  Velocity velocity = Velocity{0, 0};
  std::vector<Mesh> meshes;
  int energy = 0;
  int mass =
      1;    // Stuff will have mass to account for some conservation of momentum
  float r;  // base rotation wrt origin
  float spin = 0;  // active spinning
  EntityKind kind = EntityKind::kNone;
  bool collided = false;
};

enum GameState { Welcome, Menu, Help, HighScore, Pause, Game };

void addTo(Point p, Point *q);
float sqdist(Point *p, Point *q);

float dot(Velocity *v1, Velocity *v2);

template <typename T>
void rotateTo(T *pt, T *dst, float angle);
template <typename T>
void addToList(T entity, std::vector<T> *entityList, bool *availableInList) {
  if (availableInList) {
    for (uint8_t i = 0; i < entityList->size(); i++) {
      if ((*entityList)[i].energy == 0) {
        (*entityList).at(i) = entity;
        (*availableInList) = false;
        return;
      }
    }
  }
  entityList->push_back(entity);
}

float n(Point *p);
float n(Velocity *p);
