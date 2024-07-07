// Copyright 2024 Ruben Berenguel

#include "./base.hpp"

#include <cmath>
#include <list>
#include <vector>

#include "./font_asset.hpp"
#include "/Users/ruben/code/32blit-sdk/32blit/32blit.hpp"

const blit::Font monoid(monoid_regular);
const blit::Font monoid_b(monoid_bold);
const blit::Font monoid_s(monoid_small);

blit::Pen YELLOW = blit::hsv_to_rgba(60 / 360.0, 0.9, 0.8);
blit::Pen BLUE = blit::hsv_to_rgba(220 / 360.0, 0.9, 0.7);
blit::Pen MAGENTA = blit::hsv_to_rgba(300 / 360.0, 0.9, 0.7);
blit::Pen CYAN = blit::hsv_to_rgba(180 / 360.0, 0.9, 0.8);
blit::Pen SUBTLY_CYAN = blit::hsv_to_rgba(180 / 360.0, 0.5, 0.8);
blit::Pen DARKCYAN = blit::hsv_to_rgba(180 / 360.0, 0.9, 0.3);
blit::Pen GREEN = blit::hsv_to_rgba(120 / 360.0, 0.9, 0.3);
blit::Pen MEDGREEN = blit::hsv_to_rgba(120 / 360.0, 0.9, 0.5);
blit::Pen BRIGHTGREEN = blit::hsv_to_rgba(120 / 360.0, 0.9, 0.8);
blit::Pen PINK = blit::hsv_to_rgba(330 / 360.0, 0.9, 0.9);
blit::Pen RED = blit::hsv_to_rgba(0 / 360.0, 0.9, 0.8);
blit::Pen MEDRED = blit::hsv_to_rgba(0 / 360.0, 0.9, 0.5);
blit::Pen DARKRED = blit::hsv_to_rgba(0 / 360.0, 0.9, 0.3);
blit::Pen DARKERRED = blit::hsv_to_rgba(0 / 360.0, 0.9, 0.15);
blit::Pen FULLWHITE = blit::hsv_to_rgba(0.0, 0.0, 1.0);
blit::Pen WHITE = blit::hsv_to_rgba(0.0, 0.0, 0.9);
blit::Pen LIGHTGREY = blit::hsv_to_rgba(0.0, 0.0, 0.7);
blit::Pen GREY = blit::hsv_to_rgba(0.0, 0.0, 0.5);
blit::Pen DARKGREY = blit::hsv_to_rgba(0.0, 0.0, 0.3);
blit::Pen BLACK = blit::hsv_to_rgba(0.0, 0.0, 0.0);

extern const blit::Font monoid;
extern const blit::Font monoid_b;
extern const blit::Font monoid_s;

float sqdist(Point *p, Point *q) {
  const float a = p->x - q->x;
  const float b = p->y - q->y;
  return a * a + b * b;
}

float n(Point *p) {
  const float a = p->x;
  const float b = p->y;
  return a * a + b * b;
}

float n(Velocity *p) {
  const float a = p->x;
  const float b = p->y;
  return a * a + b * b;
}

float dot(Velocity *v1, Velocity *v2) { return v1->x * v2->x + v1->y * v2->y; }

template <typename T>
void rotateTo(T *pt, T *dst, float angle) {
  int x = pt->x;
  int y = pt->y;
  float c = cos(angle);
  float s = sin(angle);
  dst->x = c * x - s * y;
  dst->y = s * x + c * y;
}

template void rotateTo<Point>(Point *pt, Point *dst, float angle);
template void rotateTo<Velocity>(Velocity *pt, Velocity *dst, float angle);

void poly(Point center, std::vector<Point> points, int cnt) {
  for (int i = 1; i < cnt; i++) {
    blit::screen.line(blit::Point(static_cast<int>(center.x + points[i - 1].x),
                                  static_cast<int>(center.y + points[i - 1].y)),
                      blit::Point(static_cast<int>(center.x + points[i].x),
                                  static_cast<int>(center.y + points[i].y)));
  }
}

int dwSeed;

uint32_t fastrand(uint32_t x) {
  dwSeed = dwSeed * 0x343fd + 0x269ec3;
  return ((uint32_t)(((dwSeed >> 16) & 0x7fff) * x >> 15));
}

double fastrand(double ratio) {
  return ratio * fastrand(uint32_t(200)) / 200.0;
}

float fastrand(float ratio) { return ratio * fastrand(uint32_t(200)) / 200.0; }
