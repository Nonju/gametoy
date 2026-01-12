#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// No longer needed
// enum MenuState { MENU, HIGHSCORE, SPACEMETEORGAME, FLAPGAME };

typedef struct {
  int started;
  int score;
  int health;
} Game;

typedef struct {
  int x;
  int y;
  int width;
  int height;
  int speed;
  int alive;
} GameObject;

// TODO - Rename this into "ScreenCycle" or smth like that
typedef void (*GameCycleInit)();
typedef void (*GameCycleUpdate)();
typedef void (*GameCycleRender)();
typedef struct {
  const char *name;
  GameCycleInit init;
  GameCycleUpdate update;
  GameCycleRender render;
} GameCycle;

#endif
