/*
 * --------- TODOs ---------
 *  - [x] Split games/utils into separate files
 *  - Meteor game
 *    - [x] Fix collition checks
 *    - [] Improve collition checks even further...
 *    - [] Spawn meteors more often the longer the game goes on
 *    - [] Add a "survival score" showing how many meteors have been avoided (passed player without hitting / being shot down)
 *    - [x] Show current score in-game
 *    - [x] Show current lives in-game
 *  - Flap game
 *    - Flappy bird like game
 *    - Scroll controls approach speed of walls
 *  - Air hockey
 *      - Simple opponent "AI" ( or locally hosted game :^) )
 *  - Main menu
 *    - [x] Center
 *    - [x] Start program here
 *    - Show latest score
 *    - Highscore-page (if has storage)
 *  - [x] Add filter for analog input (try to fix jitter)
 *  - [NEW GAME] Space invaders
 *  - [NEW GAME] Missle command-like game
*/

// Set board type to one of the defined board types
#define BOARD_ESP32S3 1
#define BOARD_ESP32C3 2
#define BOARD_TYPE BOARD_ESP32C3

#if BOARD_TYPE == BOARD_ESP32S3
  #define I2C_SCL 20
  #define I2C_SDA 21
#elif BOARD_TYPE == BOARD_ESP32C3
  #define I2C_SCL 10
  #define I2C_SDA 8
#endif

// Uncomment to enable battery information
// #define HAS_BATTERY
#ifdef HAS_BATTERY
  #define BAT_PIN 3
#endif

// REVIEW - Is this pin already in use? Does it need to be changed?
#define RNDPIN 2 // set to any pin with analog input (it is used only once to initialize random number generator)

#include <Wire.h>
#include "display.h"
#include "definitions.h"
#include "inputs.h"
#include "menu.h"
#include "spacemeteorgame.h"
#include "flapgame.h"

extern Adafruit_SSD1306 display;

enum MenuState menuState = MENU;

void setup() {
  randomSeed(analogRead((RNDPIN)));

  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32-S3!");

  #ifndef BOARD_TYPE
    Serial.println("No board type specified!");
    for(;;); // Don't proceed, loop forever
  #endif

  if (!Wire.begin(I2C_SDA, I2C_SCL)) {
    Serial.println("Failed to initialize I2C");
    for(;;); // Don't proceed, loop forever
  }

  display_setup();
  inputs_setup();

  Serial.println("Setup COMPLETE!");
}

int batteryCharge = 0;
void loop() {
  delay(20); // Needed?

  /// Read
  inputs_updateBefore();

  /// "Logic" / Update
  switch (menuState) {
    case MENU:
      menu_update(&menuState);
      break;
    case HIGHSCORE:
      // TODO
      break;
    case SPACEMETEORGAME:
      smg_update(&menuState);
      break;
    case FLAPGAME:
      flap_update(&menuState);
      break;
  }
  inputs_updateAfter();


  /// Render
  display.clearDisplay();

  // Check battery
  #ifdef HAS_BATTERY
    #if 0
      //
      // Based off of calculation in this post
      // https://forum.arduino.cc/t/esp32-battery-voltage-calculation-issues/1104072
      //
      float batteryVoltage; // = 4.12; // 3.7V battery that outputs 4.12V
      float batteryLevel;
      int analogPinVolt = analogRead(BAT_PIN);
      batteryVoltage = 2*3.3/4095*analogPinVolt;
      // batteryVoltage = 2*4.12/4095*analogPinVolt;
      if (batteryVoltage<3.5){
        batteryVoltage = 3.5;
      }
      if (batteryVoltage>4.2){
        batteryVoltage = 4.2;
      }
      // batteryVoltage = 4.12;
      batteryLevel = 3.5631457616E3*pow(batteryVoltage,4)-5.4787671494E4*pow(batteryVoltage,3)+3.1541375900E5*pow(batteryVoltage,2)-8.0559293115E5*batteryVoltage+7.7008357786E5;

      batteryCharge = static_cast<int>(batteryLevel);

      Serial.print("BATTERY LEVEL: ");
      Serial.print(batteryCharge);
      Serial.print(" | pinVolt: ");
      Serial.print(analogPinVolt);
      Serial.print(" | batVolt: ");
      Serial.println(batteryVoltage);

      display.setTextSize(1);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(SCREEN_WIDTH * 0.75, SCREEN_HEIGHT*0.9);
      display.print("B:");
      display.println(batteryCharge);
      display.setCursor(SCREEN_WIDTH * 0.75, SCREEN_HEIGHT*0.75);
      display.print("V:");
      display.println(batteryVoltage);

    #else
      //
      // Based off of calculation in this post
      // https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51/power-management#measuring-battery-3010518
      //

      float measuredvbat = analogRead(BAT_PIN);
      measuredvbat *= 2;    // we divided by 2, so multiply back
      measuredvbat *= 3.7;  // Multiply by 3.3V, our reference voltage
      measuredvbat /= 4096; // convert to voltage
      Serial.print("VBat: " );
      Serial.println(measuredvbat);

      display.setTextSize(1);              // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE); // Draw white text
      display.setCursor(SCREEN_WIDTH * 0.75, SCREEN_HEIGHT*0.9);
      display.print("B:");
      display.println(measuredvbat);
    #endif


  #endif

  switch (menuState) {
    case MENU:
      menu_render();
      break;
    case HIGHSCORE:
      // TODO
      break;
    case SPACEMETEORGAME:
      smg_render();
      break;
    case FLAPGAME:
      flap_render();
      break;
  }
  display.display();

}

