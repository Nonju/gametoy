#include <stdlib.h>

#include "definitions.h"
#include "menu.h"
#include "screenstack.h"

ScreenStack screenStack = {0, nullptr};

void screenstack_setup() {
  while (screenStack.top != nullptr) {
    screenstack_pop();
  }
  screenstack_push(menu_getCycle());
}

int screenstack_recount(ScreenStack *stack) {
  int count = 0;
  ScreenStackNode *cur = stack->top;
  while (cur != nullptr) {
    count++;
    cur = cur->prev;
  }
  return count;
}

void screenstack_push(GameCycle *cycle) {
  if (cycle == nullptr)
    return;

  ScreenStackNode *newNode = (ScreenStackNode *)malloc(sizeof(ScreenStackNode));

  ScreenStackNode *prevTop = screenStack.top;
  if (prevTop != nullptr) {
    newNode->prev = prevTop;
  }
  newNode->cycle = cycle;

  screenStack.top = newNode;
  screenStack.count++;

  if (cycle->init != nullptr) {
    cycle->init();
  }
}

void screenstack_pop() {
  ScreenStackNode *top = screenStack.top;
  if (top == nullptr) {
    screenStack.count = 0; // Ensure count is reset
    return;
  }

  if (top->prev != nullptr) {
    screenStack.top = top->prev;
    screenStack.count--;
  }

  free(top);
}

GameCycle *screenstack_getTop() {
  return screenStack.top == nullptr ? nullptr : screenStack.top->cycle;
}
int screenstack_getCount() { return screenStack.count; }

void screenstack_updateTop() {
  GameCycle *topCycle = screenstack_getTop();
  if (topCycle == nullptr)
    return;
  topCycle->update();
}
void screenstack_renderTop() {
  GameCycle *topCycle = screenstack_getTop();
  if (topCycle == nullptr)
    return;
  topCycle->render();
}
