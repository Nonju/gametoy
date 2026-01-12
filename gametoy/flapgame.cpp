#include "flapgame.h"
#include "definitions.h"
#include "display.h"
#include "inputs.h"
#include "screenstack.h"

extern Adafruit_SSD1306 display;

static GameCycle flap_cycle = {
    "Flap game", // Name
    flap_init,   // Init
    flap_update, // Update
    flap_render, // Render
};

Game flap_game = {1, 0, 3};
GameObject flap_player = {FLAP_PLAYER_X,
                          SCREEN_HEIGHT / 2,
                          FLAP_PLAYER_WIDTH,
                          FLAP_PLAYER_HEIGHT,
                          0,
                          1};
double flap_playerVelocity = FLAP_PLAYER_GRAVITY; // Should this be 0?
FlapWallPair flap_walls[FLAP_WALL_MAX_AMOUNT];
FlapWallPair *flap_lastWall = nullptr;

int flap_timeSinceLastGravUpdate = 0;

GameCycle *flap_getCycle() { return &flap_cycle; }

void flap_init() {
  Serial.println("FLAP - Running init");

  flap_lastWall = nullptr;
  for (int i = 0; i < FLAP_WALL_MAX_AMOUNT; i++) {
    flap_walls[i].top.alive = 0;
    flap_walls[i].bottom.alive = 0;
  }

  flap_game.health = 3;
  flap_game.score = 0;
  flap_game.started = 1;
}

void flap_update() {
  if (flap_game.started == 1) {
    _flap_update_game();
  } else {
    _flap_update_end();
  }
}

int first = 1; // FIXME - Remove this
void _flap_update_game() {
  if (first == 1) {
    flap_init();
    first = 0;
  }

  int curTime = millis();
  if (inputs_btnPressed_A()) {
    if (flap_playerVelocity < 0) {
      flap_playerVelocity = FLAP_PLAYER_JUMP_VELOCITY;
    } else {
      flap_playerVelocity += FLAP_PLAYER_JUMP_VELOCITY;
    }
    Serial.print("CLIC - Player velocity is: ");
    Serial.println(flap_playerVelocity);
  }

  // Apply gravity
  if ((curTime - flap_timeSinceLastGravUpdate) > FLAP_PLAYER_GRAVITY_TIMEOUT) {
    flap_timeSinceLastGravUpdate = curTime;
    flap_playerVelocity -= FLAP_PLAYER_GRAVITY;
    Serial.print("GRAV - Player velocity is: ");
    Serial.println(flap_playerVelocity);
  }

  // Cap player velocity
  if (flap_playerVelocity >= FLAP_PLAYER_MAX_VELOCITY) {
    flap_playerVelocity = FLAP_PLAYER_MAX_VELOCITY;
  } else if (flap_playerVelocity <= -FLAP_PLAYER_MAX_VELOCITY) {
    flap_playerVelocity = -FLAP_PLAYER_MAX_VELOCITY;
  }

  // Move player
  flap_player.y -= flap_playerVelocity;
  if (flap_player.y < FLAP_HUD_SAFE_AREA) {
    flap_player.y = FLAP_HUD_SAFE_AREA;
  } else if (flap_player.y > (SCREEN_HEIGHT - flap_player.height)) {
    flap_player.y = (SCREEN_HEIGHT - flap_player.height);
  }

  // Spawn walls
  if (flap_lastWall == nullptr ||
      flap_lastWall->top.x <= FLAP_WALL_SPAWN_PADDING) {
    _flap_spawnWall();
  }

  // Move walls
  FlapWallPair *curWall;
  int currentWallSpeed = _flap_getWallSpeed();
  for (int i = 0; i < FLAP_WALL_MAX_AMOUNT; i++) {
    curWall = &flap_walls[i];
    if (!_flap_wallIsAlive(curWall)) {
      continue;
    }
    curWall->top.x -= currentWallSpeed;
    curWall->bottom.x -= currentWallSpeed;

    if (_flap_isColliding(flap_player, curWall->top) ||
        _flap_isColliding(flap_player, curWall->bottom)) {
      curWall->top.alive = 0;
      flap_game.health = flap_game.health - 1;
    } else if ((curWall->top.x + curWall->top.width) <= 0) {
      curWall->top.alive = 0;
      flap_game.score++;
    }
  }

  // Loss check
  if (flap_game.health <= 0) {
    Serial.println("Player is DEAD");
    flap_game.started = 0;
  }

  // NOTE - DEV
  if (inputs_btnPressed_B()) {
    int x = random(SCREEN_WIDTH * 0.1, SCREEN_WIDTH);
    _flap_spawnWall(x);
  }
}

void _flap_update_end() {
  if (inputs_btnPressed_A()) {
    // REVIEW - Not sure this is required now that
    //          screenstack_push also calls init
    flap_init(); // Reset game before returning
    screenstack_pop();
  }
}

void flap_render() {
  if (flap_game.started == 1) {
    /// Player
    display.drawRect(flap_player.x, flap_player.y, flap_player.width,
                     flap_player.height, SSD1306_WHITE);

    /// Walls
    FlapWallPair *curWall;
    for (int i = 0; i < FLAP_WALL_MAX_AMOUNT; i++) {
      curWall = &flap_walls[i];
      if (!_flap_wallIsAlive(curWall)) {
        continue;
      }
      display.drawRect(curWall->top.x, curWall->top.y, curWall->top.width,
                       curWall->top.height, SSD1306_WHITE);
      display.drawRect(curWall->bottom.x, curWall->bottom.y,
                       curWall->bottom.width, curWall->bottom.height,
                       SSD1306_WHITE);
    }

    /// HUD
    // score
    display.setCursor(5, 2);
    display.print("score: ");
    display.println(flap_game.score);

    // hp
    display.setCursor(SCREEN_WIDTH * 0.65, 2);
    display.cp437(true);
    display.print("hp: ");
    for (int i = 0; i < flap_game.health; i++) {
      display.write(3); // Heart char
    }
    display.println();
  } else {
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println("Game Over!");
    display.setCursor(0, 20);
    display.print("score: ");
    display.println(flap_game.score);
  }
}

void _flap_spawnWall() { return _flap_spawnWall(-1); }
void _flap_spawnWall(int xOverride) {
  Serial.println("Attempting to spawn wall");

  // Find free wall index
  int freeIndex = -1;
  for (int i = 0; i < FLAP_WALL_MAX_AMOUNT; i++) {
    if (flap_walls[i].top.alive == 0) {
      freeIndex = i;
      break;
    }
  }
  if (freeIndex == -1) {
    return;
  }

  // Setup wall pair
  int gap = random(FLAP_WALL_MIN_GAP, FLAP_WALL_MAX_GAP);
  int x = xOverride == -1
              ? SCREEN_WIDTH - 4
              : xOverride; // Start at outside of screen just peeking in
  int hTop = random(SCREEN_HEIGHT * 0.1, SCREEN_HEIGHT * 0.6);
  int yBottom = hTop + gap;
  int hBottom = SCREEN_HEIGHT - yBottom;

  FlapWallPair *freeWallPair = &flap_walls[freeIndex];

  // Top wall
  freeWallPair->top.alive = 1;
  freeWallPair->top.x = x;
  freeWallPair->top.y = FLAP_HUD_SAFE_AREA;
  freeWallPair->top.width = FLAP_WALL_WIDTH;
  freeWallPair->top.height = hTop;

  // Bottom wall
  freeWallPair->bottom.alive = 1;
  freeWallPair->bottom.x = x;
  freeWallPair->bottom.y = yBottom;
  freeWallPair->bottom.width = FLAP_WALL_WIDTH;
  freeWallPair->bottom.height = yBottom;

  flap_lastWall = freeWallPair; // <-- Keep track of latest spawned wall
  Serial.println("Wall has been spawned!!");
}

int _flap_wallIsAlive(FlapWallPair *wall) {
  return (wall != nullptr && wall->top.alive == 1) ? 1 : 0;
}

int _flap_getWallSpeed() {
  /*return inputs_scrollGetMappedState(FLAP_WALL_MIN_SPEED,
   * FLAP_WALL_MAX_SPEED);*/
  return inputs_scrollGetMappedState(FLAP_WALL_MAX_SPEED, FLAP_WALL_MIN_SPEED);
}

// FIXME - straight copy of the function in the "spacemeteorgame"
// TODO -> Replace both with a generic collision-checker-util
// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
int _flap_isColliding(GameObject o1, GameObject o2) {
  if (o1.alive == 0 || o2.alive == 0) {
    return 0;
  }

  if (o1.x < o2.x + o2.width && o1.x + o1.width > o2.x &&
      o1.y < o2.y + o2.height && o1.y + o1.height > o2.y) {
    return 1;
  }
  return 0;
}
