#include "flapgame.h"
#include "display.h"
#include "inputs.h"

extern Adafruit_SSD1306 display;

Game flap_game = {1, 0, 1};
GameObject flap_player = {FLAP_PLAYER_X,
                          SCREEN_HEIGHT / 2,
                          FLAP_PLAYER_WIDTH,
                          FLAP_PLAYER_HEIGHT,
                          0,
                          1};
int flap_playerVelocity = FLAP_PLAYER_GRAVITY; // Should this be 0?
FlapWallPair flap_walls[5];

int flap_timeSinceLastGravUpdate = 0;

void flap_init() {}

void flap_update(MenuState *menuState) {
  int curTime = millis();

  if (inputs_btnPressed()) {
    if (flap_playerVelocity < 0) {
      flap_playerVelocity = FLAP_PLAYER_JUMP_VELOCITY;
    } else {
      flap_playerVelocity += FLAP_PLAYER_JUMP_VELOCITY;
    }
    Serial.print("CLIC - Player velocity is: ");
    Serial.println(flap_playerVelocity);
  }

  // Apply gravity
  if ((curTime - flap_timeSinceLastGravUpdate) > FLAP_PLAYER_GRAVITY_TIMEOUT) {
    flap_timeSinceLastGravUpdate = curTime;
    flap_playerVelocity -= FLAP_PLAYER_GRAVITY;
    Serial.print("GRAV - Player velocity is: ");
    Serial.println(flap_playerVelocity);
  }

  // Cap player velocity
  if (flap_playerVelocity >= FLAP_PLAYER_MAX_VELOCITY) {
    flap_playerVelocity = FLAP_PLAYER_MAX_VELOCITY;
  } else if (flap_playerVelocity <= -FLAP_PLAYER_MAX_VELOCITY) {
    flap_playerVelocity = -FLAP_PLAYER_MAX_VELOCITY;
  }

  // Move player
  flap_player.y -= flap_playerVelocity;
  if (flap_player.y < 0) {
    flap_player.y = 0;
  } else if (flap_player.y > (SCREEN_HEIGHT - flap_player.height)) {
    flap_player.y = (SCREEN_HEIGHT - flap_player.height);
  }
}

void flap_render() {

  /// Player
  display.drawRect(flap_player.x, flap_player.y, flap_player.width,
                   flap_player.height, SSD1306_WHITE);
}
