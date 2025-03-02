#ifndef SPACEMETEORGAME_H
#define SPACEMETEORGAME_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

/// HUD
#define SMG_HUD_SAFE_AREA (SCREEN_HEIGHT * 0.2)

/// PLAYER
#define SMG_PLAYER_WIDTH 5
#define SMG_PLAYER_HEIGHT 3
#define SMG_PLAYER_X 2

/// MISSILE
#define SMG_MISSILE_MOVE_TIME 10 // 10ms
#define SMG_MISSILE_SPEED 7      // Quick boiii
#define SMG_MISSILE_WIDTH 3
#define SMG_MISSILE_HEIGHT 1

/// METEORS
#define SMG_METEOR_SPAWN_TIME 1000        // Once a second
#define SMG_METEOR_FASTEST_SPAWN_TIME 300 // 300ms
#define SMG_METEOR_MOVE_TIME 20           // 20ms
#define SMG_METEOR_MIN_SPEED 2
#define SMG_METEOR_MAX_SPEED 5
#define SMG_METEOR_MAX_AMOUNT 10
#define SMG_METEOR_MIN_WIDTH 3
#define SMG_METEOR_MAX_WIDTH 6
#define SMG_METEOR_MIN_HEIGHT 2
#define SMG_METEOR_MAX_HEIGHT 5

void smg_init();
void smg_update(MenuState *menuState);
void smg_render();

void _smg_update_game();
void _smg_update_end(MenuState *menuState);
void _smg_spawnMissile();
void _smg_spawnMeteor();
void _smg_destroyMeteor(int index);

int _smg_isColliding(GameObject o1, GameObject o2);

#endif
