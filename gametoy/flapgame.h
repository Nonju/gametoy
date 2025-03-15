#ifndef FLAPGAME_H
#define FLAPGAME_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

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
#define FLAP_WALL_MIN_GAP SCREEN_HEIGHT * 0.4
#define FLAP_WALL_MAX_GAP SCREEN_HEIGHT * 0.6
#define FLAP_WALL_MIN_SPEED 2
#define FLAP_WALL_MAX_SPEED 7
#define FLAP_WALL_SPAWN_PADDING SCREEN_WIDTH * 0.5

typedef struct {
  GameObject top;
  GameObject bottom;
} FlapWallPair;

void flap_init();
void flap_update(MenuState *menuState);
void flap_render();

void _flap_spawnWall();
void _flap_spawnWall(int xOverride);
int _flap_wallIsAlive(FlapWallPair *wall);
int _flap_getWallSpeed();

#endif
