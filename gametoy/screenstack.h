#ifndef SCREENSTACK_H
#define SCREENSTACK_H

#include "definitions.h"

typedef struct ScreenStackNode {
  ScreenStackNode *prev;
  GameCycle *cycle;
} ScreenStackNode;

typedef struct {
  int count;
  ScreenStackNode *top;
} ScreenStack;

void screenstack_setup();

int screenstack_recount(ScreenStack *stack);
void screenstack_push(GameCycle *cycle);
void screenstack_pop();

GameCycle *screenstack_getTop();
int screenstack_getCount();

void screenstack_updateTop();
void screenstack_renderTop();

#endif
