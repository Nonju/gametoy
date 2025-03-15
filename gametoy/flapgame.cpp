#include "flapgame.h"
#include "display.h"
#include "inputs.h"

extern Adafruit_SSD1306 display;

Game flap_game = {1, 0, 1};
GameObject flap_player = {FLAP_PLAYER_X,
                          SCREEN_HEIGHT / 2,
                          FLAP_PLAYER_WIDTH,
                          FLAP_PLAYER_HEIGHT,
                          0,
                          1};
int flap_playerVelocity = FLAP_PLAYER_GRAVITY; // Should this be 0?
FlapWallPair flap_walls[FLAP_WALL_MAX_AMOUNT];
FlapWallPair *flap_lastWall = nullptr;

int flap_timeSinceLastGravUpdate = 0;

void flap_init() {}

void flap_update(MenuState *menuState) {
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
  if (flap_player.y < 0) {
    flap_player.y = 0;
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

    if ((curWall->top.x + curWall->top.width) <= 0) {
      curWall->top.alive = 0;
    }
  }

}

void flap_render() {

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
  freeWallPair->top.y = 0;
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
