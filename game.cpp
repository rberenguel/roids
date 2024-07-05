// Copyright 2024 Ruben Berenguel

#include "./game.hpp"

#include <time.h>

#include <array>
#include <cmath>
#include <list>

#include "./base.hpp"
#include "./scene.hpp"

GameScreen::GameScreen() {
  stars_size = 250;
  stars = starfield(blit::screen.bounds.w, blit::screen.bounds.h, stars_size);

  // Setup player, set as separate function eventually

  flameList = std::vector<Entity>(0);
  shotList = std::vector<Entity>(0);
  roidList = std::vector<Asteroid>(0);
  ephemeralsList = std::vector<Entity>(0);
  setupPlayer();
}

void GameScreen::update(uint32_t tick) {
  if (paused && (blit::buttons & blit::Button::A)) {
    paused = false;
    return;
  }
  if (paused && (blit::buttons & blit::Button::B)) {
    showDebug = !showDebug;
    return;
  }
  if (blit::buttons & blit::Button::A) {
    if (tick - previousShot > FIRERATE) {
      addPlasmaBullet();
      previousShot = tick;
    }
  }
  if (lives > 0 && (blit::buttons & blit::Button::X)) {
    if (tick - previousTorpedo > TORPEDORATE) {
      if (torpedoes <= 0) {
        return;
      }
      addPhotonTorpedo();
      previousTorpedo = tick;
      torpedoes--;
    }
  }
  if (blit::buttons.pressed & blit::Button::B) {
    invulnerable = tick + 1000;
  }
  if (lives == 0 && (blit::buttons.pressed & blit::Button::X)) {
    lives = 3;
    score = 0;
    roidList.clear();
    availableRoid = false;
    stars = starfield(blit::screen.bounds.w, blit::screen.bounds.h, stars_size);
    addSomeRoids();
  }
  if (blit::buttons.pressed & blit::Button::Y) {
    paused = true;
    return;
  }
  if (blit::buttons & (blit::Button::DPAD_LEFT | blit::Button::DPAD_RIGHT)) {
    int dir = (blit::buttons & blit::Button::DPAD_RIGHT) ? +1.0 : -1.0;
    player.r += dir * RSPEED;
    if (player.r < -2 * M_PI || player.r >= 2 * M_PI) {
      player.r = 0;
    }
  }

  if (blit::buttons & (blit::Button::DPAD_DOWN | blit::Button::DPAD_UP)) {
    int dir = (blit::buttons & blit::Button::DPAD_DOWN) ? +1.0 : -1.0;
    player.velocity.x += dir * ACCEL * cos(player.r);
    player.velocity.y += dir * ACCEL * sin(player.r);
    Velocity accelerated =
        Velocity{player.velocity.x + dir * 12 * ACCEL * cos(player.r),
                 player.velocity.y + dir * 12 * ACCEL * sin(player.r)};
    Point adjustedCenter = Point{static_cast<float>(-dir * 5), 0};
    rotateTo(&adjustedCenter, &adjustedCenter, player.r);
    addFlame(Point{player.center.x + adjustedCenter.x,
                   player.center.y + adjustedCenter.y},
             accelerated, 12);
  }
  //???
}

void GameScreen::addRoid(Asteroid roid) {
  addToList<Asteroid>(roid, &roidList, &availableRoid);
}

void GameScreen::showInfo(uint32_t tick) {
  if (showDebug) {
    blit::screen.pen = RED;
    auto foo = "stars: " + std::to_string(stars_size) +
               "\nflame: " + std::to_string(flameList.size()) +
               "\nshots: " + std::to_string(shotList.size()) +
               "\nroids: " + std::to_string(roidList.size()) +
               "\ntemps: " + std::to_string(ephemeralsList.size());
    blit::screen.text(foo, blit::minimal_font,
                      blit::Point(blit::screen.bounds.w - 50, 2));
    blit::screen.pen = RED;
    blit::screen.text(std::to_string(invulnerable), blit::minimal_font,
                      blit::Point(2, blit::screen.bounds.h - 14));
    blit::screen.pen = GREEN;
    blit::screen.text(std::to_string(tick), blit::minimal_font,
                      blit::Point(2, blit::screen.bounds.h - 8));
  }
  blit::screen.pen = YELLOW;
  blit::screen.text(std::to_string(score), blit::minimal_font,
                    blit::Point(2, 2));
  blit::screen.pen = BRIGHTGREEN;
  blit::screen.text(std::to_string(lives), blit::minimal_font,
                    blit::Point(2, 8));
  blit::screen.pen = RED;
  std::string p_torpedoes(torpedoes, '.');
  blit::screen.text(p_torpedoes, blit::minimal_font, blit::Point(2, 14));
}

void GameScreen::render(uint32_t tick) {
  blit::screen.pen = BLACK;
  blit::screen.clear();
  drawStars();

  if (lives == 0) {
    showInfo(tick);
    blit::screen.text(
        "Game over\n Press X to continue", monoid,
        blit::Point(2, static_cast<int>(blit::screen.bounds.h / 6)));
    blit::screen.pen = BRIGHTGREEN;
    blit::screen.text(
        "You scored " + std::to_string(score), monoid,
        blit::Point(2, static_cast<int>(blit::screen.bounds.h / 2)));
    return;
  }

  if (score / 500 > addedLife) {
    lives++;
    addedLife++;
  }
  if (score / 100 > addedTorpedo) {
    torpedoes++;
    if (torpedoes > 5) {
      torpedoes = 5;
    }
    addedTorpedo++;
  }

  if (paused) {
    showInfo(tick);
    blit::screen.text(
        "Game paused\n Press A to continue", monoid,
        blit::Point(2, static_cast<int>(blit::screen.bounds.h / 6)));
    return;
  }
  drawRoids(tick);
  // Player on top for invulnerability
  moveEntity(&player);
  if (invulnerable >= tick) {
    player.meshes[0].color = BRIGHTGREEN;
    player.meshes[1].color = BRIGHTGREEN;
  } else {
    if (lives > 3) {
      player.meshes[0].color = FULLWHITE;
      player.meshes[1].color = FULLWHITE;
    } else {
      switch (lives) {
        case 3:
          player.meshes[0].color = WHITE;
          player.meshes[1].color = WHITE;
          break;
        case 2:
          player.meshes[0].color = LIGHTGREY;
          player.meshes[1].color = LIGHTGREY;
          break;
        case 1:
          player.meshes[0].color = GREY;
          player.meshes[1].color = GREY;
          break;
        default:
          player.meshes[0].color = PINK;
          player.meshes[1].color = PINK;
      }
    }
  }
  drawEntity(&player);

  drawFlames();
  drawEphemerals();
  drawShots();
  showInfo(tick);
  if (tick % 100 == 0) {
    flameList = hardPurge(&flameList, &availableFlame);
    shotList = hardPurge(&shotList, &availableShot);
    torpedoList = hardPurge(&torpedoList, &availableTorpedo);
    roidList = hardPurge(&roidList, &availableRoid);
    ephemeralsList = hardPurge(&ephemeralsList, &availableEphemeral);
    if (roidList.size() == 0) {
      invulnerable = tick + 2000;
      stars =
          starfield(blit::screen.bounds.w, blit::screen.bounds.h, stars_size);
      addSomeRoids();
    }
  }
}

void GameScreen::addSomeRoids() {
  for (uint16_t i = 0; i < 8 + blit::random() % 10; i++) {
    float x = blit::random() % blit::screen.bounds.w;
    float y = blit::random() % blit::screen.bounds.h;
    float vx = 0.3 * blit::random() / (RAND_MAX + 1.0);
    float vy = 0.3 * blit::random() / (RAND_MAX + 1.0);
    auto vel = Velocity{vx, vy};
    auto ctr = Point{x, y};
    auto sides = 9 + blit::random() % 8;
    float size = 5.0 + 2. * sides;
    auto spin = 0.01 - 0.05 * blit::random() / (RAND_MAX + 1.0);
    if (sqdist(&player.center, &ctr) < 1.05 * size * size) {
      i--;
      continue;
    }
    // TODO(me) I should remove the dependency on game from asteroids.
    // Currently it looks annoying, because collision detection is _in_
    // the asteroids. But nothing prevents me from moving it _out_ into game.
    auto foo = Asteroid(shared_from_this(), ctr, vel, sides, size, spin);

    addRoid(foo);
  }
  for (uint8_t i = 0; i < roidList.size(); i++) {
    Asteroid *roid = &roidList[i];
    for (uint8_t j = i + 1; j < roidList.size(); j++) {
      Asteroid *other = &roidList[j];
      float siz = roid->size + other->size;
      if (roid->energy > 0 && other->energy > 0 &&
          sqdist(&other->center, &roid->center) < 2 * siz * siz) {
        other->energy = 0;
      }
    }
  }
}

void GameScreen::explodeAt(int count, Point center, Velocity velocity,
                           Point angleFan) {
  // angleFan is a "point" to control whether explosions are fully radial
  // or can also be like rebounding from a shot against an asteroid.
  for (int i = 0; i < count; i++) {
    float randAngle = angleFan.x + 0.5 * angleFan.y -
                      angleFan.y * rand()  // NOLINT(runtime/threadsafe_fn)
                          / (RAND_MAX + 1.0);
    rotateTo(&velocity, &velocity, randAngle);
    addFlame(center, velocity, 3);
  }
}

void GameScreen::explode(Entity player) {
  // Convert the external mesh of the "player" in segments,
  // each rotating and going in slightly different direcctions.
  // I name this only explode because I use it for roids too,
  // but it was first implemented for the player.
  for (uint8_t i = 1; i < player.meshes[0].points.size(); i++) {
    const Point p = player.meshes[0].points[i - 1];
    const Point q = player.meshes[0].points[i];
    Entity piece = Entity{};
    piece.center = Point{(q.x - p.x) / 2, (q.y - p.y) / 2};
    // const auto nrm = n(&player.velocity);
    //  I think with this normal one piece will go the wrong direction
    piece.velocity = Velocity{static_cast<float>(player.velocity.x),
                              static_cast<float>(player.velocity.x)};
    piece.center.x += player.center.x;
    piece.center.y += player.center.y;
    piece.meshes = std::vector<Mesh>(1);
    Mesh mesh = Mesh{};
    mesh.points = std::vector<Point>(2);
    mesh.presentation = std::vector<Point>(2);
    mesh.points[0] = p;
    mesh.points[1] = q;
    mesh.presentation[0] = p;
    mesh.presentation[1] = q;
    mesh.kind = MeshKind::kPolygon;
    mesh.color = RED;
    piece.meshes[0] = mesh;
    piece.spin = 0.01;
    piece.r = player.r;
    piece.kind = EntityKind::kPiece;
    piece.energy = 12;
    addToList(piece, &ephemeralsList, &availableEphemeral);
  }
}

void GameScreen::drawRoids(uint32_t tick) {
  bool added = false;
  for (Asteroid &roid : roidList) {
    if (roid.energy <= 0) {
      continue;
    }
    roid.elasticCollision();
    moveEntity(&roid);
    added |= roid.shotCollisionDetection(&toAdd);
    added |= roid.playerCollision(&toAdd, tick);
    drawEntity(static_cast<Entity *>(&roid));
  }
  if (added) {
    roidList = hardPurge(&roidList, &availableRoid);
    for (Asteroid &roid : toAdd) {
      addRoid(roid);
    }
  }
  for (Asteroid &roid : roidList) {
    roid.collided = false;
  }
}

// Only one function for energy-changing stuff should be needed. But
// that one needs to define the transition points depending on the
// type of thing, so it becomes a bit annoying.
void GameScreen::drawEphemerals() {
  for (Entity &ephemeral : ephemeralsList) {
    moveEntity(&ephemeral);
    drawEntity(&ephemeral);
    if (static_cast<float>(rand())  // NOLINT(runtime/threadsafe_fn)
            / (RAND_MAX + 1.0) <
        0.7) {
      ephemeral.energy--;
    }
    if (ephemeral.energy < 12) {
      ephemeral.meshes[0].color = MEDRED;
    }
    if (ephemeral.energy < 6) {
      ephemeral.meshes[0].color = DARKRED;
    }
    if (ephemeral.energy < 3) {
      ephemeral.meshes[0].color = DARKERRED;
    }
    if (ephemeral.energy <= 0) {
      ephemeral.energy = 0;
      availableEphemeral = true;
    }
  }
}

void GameScreen::drawStars() {
  for (Star &star : stars) {
    blit::screen.pen =
        blit::hsv_to_rgba(star.color.h, star.color.s, star.color.v);

    blit::screen.pixel(
        blit::Point(static_cast<int>(star.x), static_cast<int>(star.y)));
  }
}

// Copied from Stardust, where I had cleaned up the starfield
std::vector<Star> GameScreen::starfield(int width, int height, int star_size) {
  std::vector<Point> points = std::vector<Point>(0);
  std::vector<Star> star_layer_ = std::vector<Star>(0);
  points.clear();
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      const auto n = fastrand(static_cast<uint32_t>(10000)) / 10000.0;
      const auto f = static_cast<int>(floor(n * 10000));
      if (f == 1 || f == 42 || (f > 90 && f < +star_size / 2)) {
        points.emplace_back(
            Point{static_cast<float>(i), static_cast<float>(j)});
      }
    }
  }
  std::shuffle(points.begin(), points.end(), FastRandInt());
  for (int i = 0; i < fmin(points.size(), star_size); i++) {
    auto rh = fastrand(static_cast<uint32_t>(360));
    auto rs = fastrand(static_cast<uint32_t>(20));
    auto rv = fastrand(static_cast<uint32_t>(52));
    float h = static_cast<float>(rh) / 360.0;
    float s = static_cast<float>(10 + rs) / 100.0;
    float v = static_cast<float>(50 + rv) / 100.0;
    auto x = static_cast<float>(points[i].x);
    auto y = static_cast<float>(points[i].y);
    star_layer_.emplace_back(Star{x, y, HSV{h, s, v}});
  }
  return star_layer_;
}

Asteroid::Asteroid() {}

Asteroid::Asteroid(std::shared_ptr<GameScreen> game, Point center,
                   Velocity velocity, int sides, float size, float spin) {
  this->game = game;
  this->center = center;
  this->velocity = velocity;
  kind = EntityKind::kRoid;
  energy = static_cast<int>(0.8 * sides * size);
  this->size = size;
  this->spin = spin;
  meshes = std::vector<Mesh>(1);
  Mesh mesh = Mesh{};
  int numPoints = sides + 1;
  mesh.points = std::vector<Point>(numPoints);
  mesh.presentation = std::vector<Point>(numPoints);

  const auto a = M_TWOPI / sides;
  for (int i = 0; i < numPoints; i++) {
    const float wiggled = i * a + 0.3 * a +
                          0.6 * a *
                              (rand()  // NOLINT(runtime/threadsafe_fn)
                               / (RAND_MAX + 1.0));
    const float radius = size + size * 0.1 *
                                    rand()  // NOLINT(runtime/threadsafe_fn)
                                    / (RAND_MAX + 1.0);
    mesh.points[i] = Point{radius * cos(wiggled), radius * sin(wiggled)};
    mesh.presentation[i] = Point{radius * cos(wiggled), radius * sin(wiggled)};
  }
  mesh.points[sides] = mesh.points[0];
  mesh.presentation[sides] = mesh.presentation[0];
  for (int i = 0; i < numPoints; i++) {
    rotateTo(&mesh.points[i], &mesh.presentation[i], game->player.r);
  }
  mesh.color = GREY;
  mesh.fill = DARKGREY;
  mesh.kind = MeshKind::kPolygon;
  meshes[0] = mesh;
}

bool Asteroid::shotCollisionDetection(std::list<Asteroid> *toAdd) {
  bool addedAsteroids = false;
  for (Entity &shot : game->shotList) {
    addedAsteroids |= shotCollisionDetectionLoop(&shot, toAdd);
  }
  for (Entity &torpedo : game->torpedoList) {
    addedAsteroids |= shotCollisionDetectionLoop(&torpedo, toAdd);
  }
  return addedAsteroids;
}

bool Asteroid::shotCollisionDetectionLoop(Entity *shot,
                                          std::list<Asteroid> *toAdd) {
  bool addedAsteroids = false;
  if (shot->energy == 0) {
    return addedAsteroids;
  }
  auto sct = shot->center;
  float distq = sqdist(&sct, &center);
  if (distq <= 1.04 * (size * size)) {
    auto shot_initial_energy = shot->energy;
    int scaled_explosion_energy =
        static_cast<int>(floor(sqrt(shot_initial_energy)));
    game->explodeAt(scaled_explosion_energy, shot->center, shot->velocity,
                    Point{M_PI, 0.2});
    if (shot->kind == EntityKind::kPhotonTorpedo) {
      shot->energy -= size;
    }
    if (shot->kind == EntityKind::kPlasmaBullet) {
      shot->energy = 0;
    }

    if (blit::random() / (RAND_MAX + 1.0) < 0.8) {
      // Only add the crack randomly to avoid having too many things in memory
      Mesh mesh = Mesh{};
      mesh.points = std::vector<Point>(2);
      mesh.presentation = std::vector<Point>(2);
      float dx = sct.x - center.x;
      float dy = sct.y - center.y;
      // This vector points to the center of the asteroid.
      // Adding another crack randomly could make things more interesting.
      Point p = Point{dx, dy};
      Point pp = Point{dx, dy};
      rotateTo(&p, &p, -r);
      Point q = Point{0.9f * dx, 0.9f * dy};
      rotateTo(&q, &q, -r);
      Point qp = q;
      mesh.points[0] = p;
      mesh.points[1] = q;
      mesh.presentation[0] = pp;
      mesh.presentation[1] = qp;
      mesh.kind = MeshKind::kPolygon;
      mesh.color = RED;
      meshes.push_back(mesh);
    }

    // This is "half" of a elastic collision, because the bullet or torpedo is
    // not affected by the impact
    float m1 = size;
    float m2 = 0.0;  // Shot mass
    if (shot->kind == EntityKind::kPhotonTorpedo) {
      m2 = 0.5;
    }

    if (shot->kind == EntityKind::kPlasmaBullet) {
      // Plasma bullets have 0 penetration, but they do have a teeny wheeny bit
      // of mass
      m2 = 0.05;
    }
    float total = m1 + m2;
    Velocity v1 = velocity;
    Velocity v2 = shot->velocity;
    Velocity v1mv2 = Velocity{v1.x - v2.x, v1.y - v2.y};
    Velocity p1mp2 =
        Velocity{center.x - shot->center.x, center.y - shot->center.y};
    float dot1 = dot(&v1mv2, &p1mp2);
    float r1 = 2 * m2 / total;
    Velocity newThisVelocity = Velocity{v1.x - r1 * dot1 / distq * p1mp2.x,
                                        v1.y - r1 * dot1 / distq * p1mp2.y};
    this->velocity = newThisVelocity;

    energy -= shot_initial_energy;  // This is new
    if (shot->kind == EntityKind::kPhotonTorpedo) {
      shot->velocity.x *= 0.8;
      shot->velocity.y *= 0.8;
    }

    if (shot->kind == EntityKind::kPlasmaBullet) {
      // Plasma bullets have 0 penetration, but they do have a teeny wheeny bit
      // of mass
      shot->energy = 0;
    }

    if (energy < 25) {
      meshes[0].color = RED;
      meshes[0].fill = DARKERRED;
    }
    if (energy <= 0) {
      energy = 0;
      game->score += size;
      addedAsteroids |= breakUpRoid(shot->velocity, toAdd);
    }
  }
  return addedAsteroids;
}

bool Asteroid::playerCollision(std::list<Asteroid> *toAdd, uint32_t tick) {
  // Player collision
  bool addedAsteroids = false;
  if (energy <= 0) {
    return false;
  }
  if (sqrt(sqdist(&(game->player.center), &center)) <= 1.05 * size) {
    if (tick < game->invulnerable) {
      return false;
    }
    game->lives--;
    game->explode(game->player);
    game->invulnerable = tick + 2000;
    energy = 0;
    addedAsteroids |= breakUpRoid(game->player.velocity, toAdd);
    game->player.center = Point(static_cast<float>(blit::screen.bounds.w / 2),
                                static_cast<float>(blit::screen.bounds.h / 2));
    game->player.velocity = Velocity{0, 0};
  }
  return addedAsteroids;
}

void Asteroid::elasticCollision() {
  if (collided) {
    return;
  }
  for (Asteroid &other : game->roidList) {
    if (&other == this) {
      continue;
    }
    float span = abs(size + other.size);
    if (other.energy <= 0) {
      continue;
    }
    if (other.collided) {
      continue;
    }
    float distq = sqdist(&other.center, &center);
    if (distq <= 1.1 * span * span) {
      float m1 = size;
      float m2 = other.size;
      float total = m1 + m2;
      Velocity v1 = velocity;
      Velocity v2 = other.velocity;
      Velocity v1mv2 = Velocity{v1.x - v2.x, v1.y - v2.y};
      Velocity v2mv1 = Velocity{v2.x - v1.x, v2.y - v1.y};
      Velocity p1mp2 =
          Velocity{center.x - other.center.x, center.y - other.center.y};
      Velocity p2mp1 =
          Velocity{other.center.x - center.x, other.center.y - center.y};
      float dot1 = dot(&v1mv2, &p1mp2);
      float dot2 = dot(&v2mv1, &p2mp1);
      if (abs(dot1) < 0.1) {
        dot1 = 0.5;
      }
      if (abs(dot2) < 0.1) {
        dot2 = 0.5;
      }
      float r1 = .9 * m2 / total;
      float r2 = .9 * m1 / total;
      Velocity newThisVelocity = Velocity{v1.x - r1 * dot1 / distq * p1mp2.x,
                                          v1.y - r1 * dot1 / distq * p1mp2.y};
      Velocity newOtherVelocity = Velocity{v2.x - r2 * dot2 / distq * p2mp1.x,
                                           v2.y - r2 * dot2 / distq * p2mp1.y};
      this->velocity = newThisVelocity;
      other.velocity = newOtherVelocity;
      other.collided = true;
      if (distq < 0.8 * span * span) {
        // Try to prevent clumping of asteroids once they collide
        // It still is wrong, there is some sort of numerical instability
        // somewhere.
        int jiggle = 1 + 1.0 * blit::random() / (RAND_MAX + 1.0);
        this->center.x += jiggle * newOtherVelocity.x;
        this->center.y += -jiggle * newOtherVelocity.y;
      }
    }
  }
}

bool Asteroid::breakUpRoid(Velocity breakUpDirection,
                           std::list<Asteroid> *toAdd) {
  float r1 = size * (0.2 + .8 * blit::random() / (RAND_MAX + 1.0));
  const float f = 1.5;
  game->explodeAt(5, center, Velocity{r1, 0}, Point{0, 2 * M_PI});
  game->explode(Entity{*this});
  float vx = -breakUpDirection.y / sqrt(n(&breakUpDirection));
  float vy = breakUpDirection.x / sqrt(n(&breakUpDirection));
  int newSides = static_cast<int>(meshes[0].points.size() - 1);
  int newSize = size / 2.0;
  if (newSides > 5 && newSize > 5.0) {
    Point newCenter1 =
        Point{center.x + f * size * vx, center.y + f * size * vy};
    Point newCenter2 =
        Point{center.x - f * size * vx, center.y + f * size * vy};
    (*toAdd).push_back(Asteroid(game, newCenter1, Velocity{vx, vy}, newSides,
                                newSize, this->spin));
    (*toAdd).push_back(Asteroid(game, newCenter2, Velocity{-vx, -vy}, newSides,
                                newSize, -this->spin));
    return true;
  }
  return false;
}
