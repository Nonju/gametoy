#ifndef DEFINITIONS_H
#define DEFINITIONS_H

enum MenuState { MENU, HIGHSCORE, SPACEMETEORGAME, FLAPGAME };

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

/*typedef struct {*/
/*  int last;*/
/*  int current;*/
/*} ScrollState;*/

#endif
