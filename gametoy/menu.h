#ifndef MENU_H
#define MENU_H

#include "definitions.h"
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

void _addMenuItem(String text, int x, int y, int activeItemIndex);

void menu_update(MenuState *menuState);
void menu_render();

#endif
