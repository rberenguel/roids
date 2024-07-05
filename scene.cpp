// Copyright 2024 Ruben Berenguel

#include "./scene.hpp"

#include "./base.hpp"
#include "./game.hpp"

void Scene::addFlame(Point center, Velocity velocity, int baseEnergy) {
  Entity fl = Entity();
  float rf = static_cast<float>(rand()) /  // NOLINT(runtime/threadsafe_fn)
             (RAND_MAX + 1.0);
  fl.kind = EntityKind::kFlame;

  fl.center = center;
  fl.velocity = velocity;

  fl.energy = static_cast<int>(baseEnergy +
                               (rand() % 7));  // NOLINT(runtime/threadsafe_fn)
  fl.meshes = std::vector<Mesh>(1);
  Mesh mesh = Mesh{};
  mesh.points = std::vector<Point>(1);
  mesh.presentation = std::vector<Point>(1);
  mesh.points[0] = Point{0, 0};
  mesh.presentation[0] = Point{0, 0};
  const double accel = 0.3;
  fl.velocity.x += accel * cos(-player.r);
  fl.velocity.y += accel * sin(-player.r);
  rotateTo(&fl.velocity, &fl.velocity, 0.03 - 0.06 * rf);
  mesh.color = YELLOW;
  mesh.kind = MeshKind::kPoints;
  fl.meshes[0] = mesh;
  addToList(fl, &flameList, &availableFlame);
}

void Scene::addPhotonTorpedo() {
  Entity pt = Entity();
  pt.kind = EntityKind::kPhotonTorpedo;
  pt.center = player.center;
  pt.velocity = player.velocity;
  pt.velocity.x += PHOTON_TORPEDO_ACCEL * cos(player.r);
  pt.velocity.y += PHOTON_TORPEDO_ACCEL * sin(player.r);
  pt.energy = MAX_TORPEDO_ENERGY;
  addToList(pt, &torpedoList, &availableTorpedo);
  // has no mesh or presentation, just draws itself at center
}

void Scene::addPlasmaBullet() {
  Entity bu = Entity();
  float rf = static_cast<float>(rand())  // NOLINT(runtime/threadsafe_fn)
             / (RAND_MAX + 1.0);
  bu.kind = EntityKind::kPlasmaBullet;
  bu.center = player.center;
  bu.velocity = player.velocity;
  bu.energy = static_cast<int>(30 + (rand()  // NOLINT(runtime/threadsafe_fn)
                                     % 7));
  int numPoints = 4;
  Mesh mesh = Mesh{};
  bu.meshes = std::vector<Mesh>(1);
  mesh.points = std::vector<Point>(numPoints);
  mesh.presentation = std::vector<Point>(numPoints);
  mesh.points[0] = Point{1, 0};
  mesh.points[1] = Point{2, 1};
  mesh.points[2] = Point{2, -1};
  mesh.points[3] = Point{1, 0};
  mesh.presentation[0] = Point{1, 0};
  mesh.presentation[1] = Point{2, 1};
  mesh.presentation[2] = Point{2, -1};
  mesh.presentation[3] = Point{1, 0};
  for (int i = 0; i < numPoints; i++) {
    rotateTo(&mesh.points[i], &mesh.presentation[i], player.r);
  }

  bu.velocity.x += PLASMA_BULLET_ACCEL * cos(player.r);
  bu.velocity.y += PLASMA_BULLET_ACCEL * sin(player.r);
  rotateTo(&bu.velocity, &bu.velocity, 0.03 - 0.06 * rf);
  mesh.color = CYAN;
  mesh.kind = MeshKind::kPolygon;
  bu.meshes[0] = mesh;
  addToList(bu, &shotList, &availableShot);
}

void Scene::moveEntity(Entity *entity) {
  if (entity->energy == 0) {
    return;
  }
  // I should keep the general center of the object outside the meshes, it's the
  // best way to handle clipping.
  entity->center.x = entity->center.x + entity->velocity.x;
  entity->center.y = entity->center.y + entity->velocity.y;
  if (entity->center.x > blit::screen.bounds.w) {
    entity->center.x -= blit::screen.bounds.w;
  }
  if (entity->center.x < 0) {
    entity->center.x += (blit::screen.bounds.w);
  }
  if (entity->center.y > blit::screen.bounds.h) {
    entity->center.y -= blit::screen.bounds.h;
  }
  if (entity->center.y < 0) {
    entity->center.y += (blit::screen.bounds.h);
  }
  if (entity->spin != 0) {
    entity->r += entity->spin;
    if (entity->r >= M_TWOPI) {
      entity->r -= M_TWOPI;
    }
    if (entity->r < 0) {
      entity->r += M_TWOPI;
    }
  }
}

void Scene::drawEntity(Entity *entity) {
  // Since purging of no-energy shots or flames is done
  // when new shots or flames are added
  if (entity->energy == 0) {
    return;
  }
  if (entity->meshes.size() > 0) {
    if (entity->r != 0) {
      for (auto &mesh : entity->meshes) {
        for (uint8_t i = 0; i < mesh.points.size(); i++) {
          rotateTo(&mesh.points[i], &mesh.presentation[i], entity->r);
        }
      }
    }
    for (auto &mesh : entity->meshes) {
      std::vector<blit::Point> presentation_blit_points(0);
      blit::screen.pen = mesh.color;
      switch (mesh.kind) {
        case MeshKind::kPolygon:
          blit::screen.pen = mesh.fill;
          for (auto &pt : mesh.presentation) {
            presentation_blit_points.emplace_back(
                blit::Point(entity->center.x + pt.x, entity->center.y + pt.y));
          }
          blit::screen.polygon(presentation_blit_points);
          blit::screen.pen = mesh.color;
          poly(entity->center, mesh.presentation, mesh.presentation.size());
          break;
        case MeshKind::kPoints:
          for (uint8_t i = 0; i < mesh.presentation.size(); i++) {
            blit::screen.pixel(blit::Point(
                static_cast<int>(entity->center.x + mesh.presentation[0].x),
                static_cast<int>(entity->center.y + mesh.presentation[0].y)));
          }
          break;
        case MeshKind::kCircle:
          // Would be 0.86 for a hexagon
          blit::screen.circle(
              blit::Point(
                  static_cast<int>(entity->center.x + mesh.presentation[0].x),
                  static_cast<int>(entity->center.y + mesh.presentation[0].y)),
              static_cast<int>(0.8 * mesh.radius));
          break;
        default:
          break;
      }
    }
    return;
  }
}

void Scene::drawFlames() {
  for (Entity &flame : flameList) {
    moveEntity(&flame);
    drawEntity(&flame);
    if (static_cast<float>(rand())  // NOLINT(runtime/threadsafe_fn)
            / (RAND_MAX + 1.0) <
        0.8) {
      flame.energy--;
    }
    if (flame.energy < 5) {
      flame.meshes[0].color = RED;
    }
    if (flame.energy < 3) {
      flame.meshes[0].color = DARKRED;
    }
    if (flame.energy <= 0) {
      flame.energy = 0;
      availableFlame = true;
    }
  }
}

void Scene::purge(std::vector<Entity> *entityList, bool *availableEntity) {
  bool allZero = true;

  for (Entity &entity : *entityList) {
    if (entity.energy != 0) {
      allZero = false;
      break;
    }
  }

  if (allZero) {
    entityList->clear();
    *availableEntity = false;
  }
}

void Scene::drawShots() {
  for (Entity &shot : shotList) {
    moveEntity(&shot);
    drawEntity(&shot);
    shot.energy--;
    if (shot.energy < 5) {
      shot.meshes[0].color = GREEN;
    }
    if (shot.energy < 3) {
      shot.meshes[0].color = DARKCYAN;
    }
    if (shot.energy <= 0) {
      shot.energy = 0;
      availableShot = true;
    }
  }
  for (Entity &torpedo : torpedoList) {
    moveEntity(&torpedo);
    // drawEntity(&torpedo);
    float x = torpedo.center.x;
    float y = torpedo.center.y;

    torpedo.energy--;
    if (torpedo.energy <= 0) {
      torpedo.energy = 0;
      continue;
    }
    const int radius = 1;
    // smooch it closer to 1, decay slower
    auto adjusted_value =
        sqrt(sqrt(static_cast<float>(torpedo.energy) / MAX_TORPEDO_ENERGY));
    for (int i = 0; i < 2; i++) {
      float r = (radius + fastrand(static_cast<float>(2 * radius)));
      float a = fastrand(static_cast<float>(2 * M_PI));
      auto high = blit::hsv_to_rgba(fastrand(uint32_t(30)) / 360.0, 1.0,
                                    adjusted_value);
      blit::screen.pen = high;
      auto tx = static_cast<int>(x + r * cos(a));
      auto ty = static_cast<int>(y + r * sin(a));
      auto p1 = Point{static_cast<float>(tx), static_cast<float>(ty)};
      tx = static_cast<int>(x + r * cos(a - 0.92 * M_PI));
      ty = static_cast<int>(y + r * sin(a - 0.92 * M_PI));
      auto p2 = Point{static_cast<float>(tx), static_cast<float>(ty)};
      tx = static_cast<int>(x + r * cos(a + 0.92 * M_PI));
      ty = static_cast<int>(y + r * sin(a + 0.92 * M_PI));
      auto p3 = Point{static_cast<float>(tx), static_cast<float>(ty)};
      std::vector<Point> poly = {p1, p2, p3, p1};
      std::vector<blit::Point> adapted(poly.size());
      for (uint16_t i = 0; i < poly.size(); i++) {
        adapted[i].x = poly[i].x;
        adapted[i].y = poly[i].y;
      }
      blit::screen.polygon(adapted);
    }
    auto base =
        blit::hsv_to_rgba(fastrand(uint32_t(40)) / 360.0, 0.5, adjusted_value);
    blit::screen.pen = base;
    auto torpedo_radius = static_cast<int>(radius);
    blit::screen.circle(blit::Point(x, y), torpedo_radius);
  }
}

void Scene::setupPlayer() {
  player.center = Point(static_cast<float>(blit::screen.bounds.w / 2),
                        static_cast<float>(blit::screen.bounds.h / 2));
  player.velocity = Velocity{0, 0};
  Mesh meshOuter = Mesh{};
  Mesh meshMiddle = Mesh{};
  player.meshes = std::vector<Mesh>(2);
  meshOuter.points = std::vector<Point>(5);
  meshMiddle.points = std::vector<Point>(5);
  meshOuter.presentation = std::vector<Point>(5);
  meshMiddle.presentation = std::vector<Point>(5);
  meshOuter.color = WHITE;
  meshOuter.fill = DARKGREY;
  meshOuter.kind = MeshKind::kPolygon;
  meshMiddle.color = WHITE;
  meshMiddle.fill = DARKGREY;
  meshMiddle.kind = MeshKind::kPolygon;
  for (int i = 0; i < 5; i++) {
    meshOuter.points[i] = Point{shipOuter[i].x, shipOuter[i].y};
    meshOuter.presentation[i] = Point{shipOuter[i].x, shipOuter[i].y};
    meshMiddle.points[i] = Point{shipMiddle[i].x, shipMiddle[i].y};
    meshMiddle.presentation[i] = Point{shipMiddle[i].x, shipMiddle[i].y};
  }
  player.r = 0;
  player.meshes[0] = meshOuter;
  player.meshes[1] = meshMiddle;
  player.energy = 100;
}
