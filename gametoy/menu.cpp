#include "menu.h"
#include "definitions.h"
#include "inputs.h"

extern Adafruit_SSD1306 display;
int selectedItem = 0;

void menu_update(MenuState *menuState) {
  if (inputs_scrollHasUpdated() == 1) {
    /*Serial.print("SCROLL HAS UPDATED: ");*/
    /*Serial.print(inputs_scrollGetMappedState());*/
    /*Serial.print(" | ");*/
    /*Serial.println(inputs_scrollGetState());*/
    selectedItem = inputs_scrollGetMappedState(0, 2);
  }

  if (inputs_btnPressed_A()) {
    // FIXME - Chose state corresponding to `selectedItem`
    if (selectedItem == 0) {
      *menuState = SPACEMETEORGAME;
    } else if (selectedItem == 1) {
      *menuState = FLAPGAME;
    } else {
      *menuState = MENU;
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
  _addMenuItem("Meteor game", 10, 25, 0);
  _addMenuItem("Flap game", 10, 40, 1);
  _addMenuItem("Highscores", 10, 55, 2);
}
