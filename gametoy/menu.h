#ifndef MENU_H
#define MENU_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#define MENU_ITEM_Y_OFFSET 15

void _addMenuItem(String text, int x, int y, int activeItemIndex);

GameCycle *menu_getCycle();

void menu_update();
void menu_render();

#endif
