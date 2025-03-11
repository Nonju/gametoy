#include "spacemeteorgame.h"
#include "definitions.h"
#include "display.h"
#include "inputs.h"

extern Adafruit_SSD1306 display;

Game smg_game = {1, 0, 3};
GameObject smg_player = {SMG_PLAYER_X,
                         SCREEN_HEIGHT / 2,
                         SMG_PLAYER_WIDTH,
                         SMG_PLAYER_HEIGHT,
                         9001,
                         1};
GameObject smg_missile = {
    0, 0, SMG_MISSILE_WIDTH, SMG_MISSILE_HEIGHT, SMG_MISSILE_SPEED, 0};
GameObject smg_meteors[SMG_METEOR_MAX_AMOUNT];
int smg_timeSinceLastMissileMove = 0;
int smg_timeSinceLastMeteorMove = 0;
int smg_timeSinceLastMeteorSpawn = 0;
int smg_meteorSpawnTimeout = SMG_METEOR_SPAWN_TIME;

void smg_init() {

  smg_game.started = 1;
  smg_game.score = 0;
  smg_game.health = 3;

  smg_missile.alive = 0;

  smg_meteorSpawnTimeout = SMG_METEOR_SPAWN_TIME;

  for (int i = 0; i < SMG_METEOR_MAX_AMOUNT; i++) {
    smg_meteors[i].alive = 0;
  }
}

void smg_update(MenuState *menuState) {
  if (smg_game.started == 1) { // Active game
    _smg_update_game();
  } else { // End-game screen
    _smg_update_end(menuState);
  }
}

void _smg_update_game() {
  int curTime = millis();

  if (inputs_scrollHasUpdated()) {
    smg_player.y = inputs_scrollGetMappedState(
        SMG_HUD_SAFE_AREA, SCREEN_HEIGHT - smg_player.height);
  }

  if (inputs_btnPressed_A()) {
    Serial.println("ATTEMPTING TO SHOOT");
    _smg_spawnMissile();
  }

  // Meteor spawn
  /*if ((curTime - smg_timeSinceLastMeteorSpawn) > SMG_METEOR_SPAWN_TIME) {*/
  if ((curTime - smg_timeSinceLastMeteorSpawn) > smg_meteorSpawnTimeout) {
    Serial.println("ATTEMPTING TO SPAWN METEOR");
    smg_timeSinceLastMeteorSpawn =
        curTime; // Actually time since last _spawn attempt_ but w/e..
    _smg_spawnMeteor();
  }

  // Meteor move
  if ((curTime - smg_timeSinceLastMeteorMove) > SMG_METEOR_MOVE_TIME) {
    smg_timeSinceLastMeteorMove = curTime;
    for (int i = 0; i < SMG_METEOR_MAX_AMOUNT; i++) {
      if (smg_meteors[i].alive == 0)
        continue;
      smg_meteors[i].x = smg_meteors[i].x - smg_meteors[i].speed;
      if ((smg_meteors[i].x + smg_meteors[i].width) <= 0) {
        /*smg_meteors[i].alive = 0;*/
        _smg_destroyMeteor(i);
      } else if (_smg_isColliding(smg_player,
                                  smg_meteors[i]) ==
                 1) { // Player was hit by meteor
        /*smg_meteors[i].alive = 0;*/
        _smg_destroyMeteor(i);
        smg_game.health = smg_game.health - 1;
        Serial.print("Player was HIT - ");
        Serial.print(smg_game.health);
        Serial.println(" hp left!");

      } else if (_smg_isColliding(smg_missile, smg_meteors[i]) ==
                 1) { // Missile hit
        /*smg_meteors[i].alive = 0;*/
        _smg_destroyMeteor(i);
        smg_missile.alive = 0;
        smg_game.score = smg_game.score + 1;
      }

      /*
       * OLD COLLISION CHECK
      else if (smg_meteors[i].x <= (smg_player.x + smg_player.width)) {
        if (smg_meteors[i].y >= smg_player.y &&
            smg_meteors[i].y <=
                (smg_player.y +
                 smg_player.height)) { // Player was hit by meteor
          smg_meteors[i].alive = 0;
          smg_game.health = smg_game.health - 1;
          Serial.print("Player was HIT - ");
          Serial.print(smg_game.health);
          Serial.println(" hp left!");
        }
      }
       else if ((smg_missile.y >= smg_meteors[i].y &&
                  smg_missile.y <=
                      (smg_meteors[i].y + smg_meteors[i].height)) ||
                 (smg_meteors[i].y >= smg_missile.y &&
                  smg_meteors[i].y <=
                      (smg_missile.y +
                       smg_missile
                           .height))) { // Missle / meteor on collision path
        if (smg_missile.x >= smg_meteors[i].x &&
            smg_missile.x <=
                (smg_meteors[i].x + smg_meteors[i].width)) { // Missile hit
          smg_meteors[i].alive = 0;
          smg_missile.alive = 0;
          smg_game.score = smg_game.score + 1;
        }
      }*/
    }
  }

  // Missile move
  if ((curTime - smg_timeSinceLastMissileMove) > SMG_MISSILE_MOVE_TIME) {
    smg_timeSinceLastMissileMove = curTime;
    smg_missile.x = smg_missile.x + smg_missile.speed;
    if (smg_missile.x >= SCREEN_WIDTH) {
      smg_missile.alive = 0;
    }
  }

  // Loss check
  if (smg_game.health <= 0) {
    Serial.println("Player is DEAD");
    smg_game.started = 0;
  }
}

void _smg_update_end(MenuState *menuState) {
  if (inputs_btnPressed_A()) {
    smg_init(); // Reset game before returning
    *menuState = MENU;
  }
}

void smg_render() {
  if (smg_game.started == 1) {
    // Player
    display.drawRect(smg_player.x, smg_player.y, smg_player.width,
                     smg_player.height, SSD1306_WHITE);

    // HUD - score
    display.setCursor(5, 2);
    display.print("score: ");
    display.println(smg_game.score);

    // HUD - hp
    display.setCursor(SCREEN_WIDTH * 0.65, 2);
    display.cp437(true);
    display.print("hp: ");
    for (int i = 0; i < smg_game.health; i++) {
      display.write(3); // Heart char
    }
    display.println();
    /*display.println(smg_game.health);*/

    // Missile
    if (smg_missile.alive == 1) {
      display.drawRect(smg_missile.x, smg_missile.y, smg_missile.width,
                       smg_missile.height, SSD1306_WHITE);
    }

    // Meteors
    for (int i = 0; i < SMG_METEOR_MAX_AMOUNT; i++) {
      if (smg_meteors[i].alive == 0)
        continue;
      display.drawRect(smg_meteors[i].x, smg_meteors[i].y, smg_meteors[i].width,
                       smg_meteors[i].height, SSD1306_WHITE);
    }

  } else {
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println("Game Over!");
    display.setCursor(0, 20);
    display.print("score: ");
    display.println(smg_game.score);
  }
}

void _smg_spawnMissile() {
  if (smg_missile.alive == 1) {
    return; // Currently only allowing 1 missile, might chagne latöer
  }
  smg_missile.y = smg_player.y + (smg_player.height / 2);
  smg_missile.x = smg_player.x + smg_player.width;
  smg_missile.alive = 1;
}

void _smg_spawnMeteor() {
  // Find empty index
  int freeIndex = -1;
  for (int i = 0; i < SMG_METEOR_MAX_AMOUNT; i++) {
    if (smg_meteors[i].alive == 0) {
      freeIndex = i;
      break;
    }
  }
  if (freeIndex == -1) {
    return; // All slots are occupied
  }

  // Insert meteor at free index
  smg_meteors[freeIndex].x = SCREEN_WIDTH - smg_meteors[freeIndex].width - 2;
  smg_meteors[freeIndex].width =
      random(SMG_METEOR_MIN_WIDTH,
             SMG_METEOR_MAX_WIDTH); // Setting this just in case for now
  smg_meteors[freeIndex].height =
      random(SMG_METEOR_MIN_HEIGHT,
             SMG_METEOR_MAX_HEIGHT); // Setting this just in case for now
  smg_meteors[freeIndex].y = random(SMG_HUD_SAFE_AREA, SCREEN_HEIGHT);
  smg_meteors[freeIndex].speed =
      random(SMG_METEOR_MIN_SPEED, SMG_METEOR_MAX_SPEED);
  smg_meteors[freeIndex].alive = 1;
}

void _smg_destroyMeteor(int index) {
  smg_meteors[index].alive = 0;
  if (smg_meteorSpawnTimeout > SMG_METEOR_FASTEST_SPAWN_TIME) {
    smg_meteorSpawnTimeout -= 10;
  }
}

// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
int _smg_isColliding(GameObject o1, GameObject o2) {
  if (o1.alive == 0 || o2.alive == 0) {
    return 0;
  }

  if (o1.x < o2.x + o2.width && o1.x + o1.width > o2.x &&
      o1.y < o2.y + o2.height && o1.y + o1.height > o2.y) {
    return 1;
  }
  return 0;
}
