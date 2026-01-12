#include "menu.h"
#include "definitions.h"
#include "flapgame.h"
#include "inputs.h"
#include "screenstack.h"
#include "spacemeteorgame.h"

extern Adafruit_SSD1306 display;

static GameCycle menu_cycle = {
    "Menu",      // Name
    nullptr,     // Init
    menu_update, // Update
    menu_render, // Render
};

int selectedItem = 0;

GameCycle *menu_getCycle() { return &menu_cycle; }

void menu_update() {
  if (inputs_scrollHasUpdated() == 1) {
    /*Serial.print("SCROLL HAS UPDATED: ");*/
    /*Serial.print(inputs_scrollGetMappedState());*/
    /*Serial.print(" | ");*/
    /*Serial.println(inputs_scrollGetState());*/
    selectedItem = inputs_scrollGetMappedState(0, 2);
  }

  if (inputs_btnPressed_A()) {

    if (selectedItem == 0) {
      screenstack_push(smg_getCycle());
    } else if (selectedItem == 1) {
      screenstack_push(flap_getCycle());
    }
  }
}

void _addMenuItem(String text, int x, int y, int activeItemIndex) {
  String prefix = activeItemIndex == selectedItem ? "> " : "";
  display.setCursor(x, y);
  display.println(prefix + text);
}
void menu_render() {
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  _addMenuItem("-Select game-", 25, 5, -1);
  // _addMenuItem("Meteor game", 10, 25, 0);
  // _addMenuItem("Flap game", 10, 40, 1);
  _addMenuItem(smg_getCycle()->name, 10, 25, 0);
  _addMenuItem(flap_getCycle()->name, 10, 40, 1);
  _addMenuItem("Highscores", 10, 55, 2);
}
