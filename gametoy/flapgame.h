#ifndef FLAPGAME_H
#define FLAPGAME_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

/// HUD
#define FLAP_HUD_SAFE_AREA (SCREEN_HEIGHT * 0.2)

/// PLAYER
#define FLAP_PLAYER_WIDTH 4
#define FLAP_PLAYER_HEIGHT 3
#define FLAP_PLAYER_X 4
#define FLAP_PLAYER_JUMP_VELOCITY 5.5
#define FLAP_PLAYER_GRAVITY 2.5
#define FLAP_PLAYER_GRAVITY_TIMEOUT 100 // Time between gravity affects
#define FLAP_PLAYER_MAX_VELOCITY 10

/// WALL
#define FLAP_WALL_MAX_AMOUNT 5
#define FLAP_WALL_WIDTH SCREEN_WIDTH * 0.1
#define FLAP_WALL_MIN_GAP SCREEN_HEIGHT * 0.55
#define FLAP_WALL_MAX_GAP SCREEN_HEIGHT * 0.7
#define FLAP_WALL_MIN_SPEED 2
#define FLAP_WALL_MAX_SPEED 7
#define FLAP_WALL_SPAWN_PADDING SCREEN_WIDTH * 0.4

typedef struct {
  GameObject top;
  GameObject bottom;
} FlapWallPair;

GameCycle *flap_getCycle();

void flap_init();
void flap_update();
void flap_render();

void _flap_update_game();
void _flap_update_end();
void _flap_spawnWall();
void _flap_spawnWall(int xOverride);
int _flap_wallIsAlive(FlapWallPair *wall);
int _flap_getWallSpeed();

int _flap_isColliding(GameObject o1, GameObject o2);

#endif
