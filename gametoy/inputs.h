#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>

/*#if BOARD_TYPE == BOARD_ESP32S3*/
/*#define BTN_PIN 19*/
/*#define POT_PIN 14*/
/*#elif BOARD_TYPE == BOARD_ESP32C3*/
/*#define BTN_PIN 5*/
/*#define POT_PIN 2*/
/*#endif*/

#define BTN_PIN_A 2 // Was 5
#define BTN_PIN_B 1 // Was 4
#define POT_PIN 4   // Was 2

#define POT_MAX 4095
#define POT_INPUT_FILTER_THRESHOLD 10

void inputs_setup();

void inputs_updateBefore();
void inputs_updateAfter();

int inputs_btnPressed_A();
int inputs_btnPressed_B();

/*int inputs_btnLongPressed();*/ // Implement when needed
/*int inputs_btnReleased();*/    // Implement when needed

int inputs_scrollHasUpdated();
int inputs_scrollGetState();
int inputs_scrollGetMappedState();
int inputs_scrollGetMappedState(int min, int max);

#endif
