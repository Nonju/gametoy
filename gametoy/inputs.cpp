#include "inputs.h"

int btnState, lastBtnState;
int scrollState, lastScrollState;

void inputs_setup() { pinMode(BTN_PIN, INPUT_PULLUP); };

void inputs_updateBefore() {
  btnState = digitalRead(BTN_PIN);

  // Try to filter out some jitter
  int newScrollState = analogRead(POT_PIN);
  if (newScrollState < (lastScrollState - POT_INPUT_FILTER_THRESHOLD) ||
      newScrollState > (lastScrollState + POT_INPUT_FILTER_THRESHOLD)) {
    scrollState = newScrollState;
  }
}

void inputs_updateAfter() {
  lastBtnState = btnState;
  lastScrollState = scrollState;
}

int inputs_btnPressed() {
  return (btnState == LOW && btnState != lastBtnState) ? 1 : 0;
}

// FIXME - Improve jitter-handling
int inputs_scrollHasUpdated() { return scrollState == lastScrollState ? 0 : 1; }

int inputs_scrollGetState() { return scrollState; }

int inputs_scrollGetMappedState(int min, int max) {
  // Might've connected the pot the wrong way :^) Reversing min/max for now!!
  return map(scrollState, 0, POT_MAX, max, min);
}
int inputs_scrollGetMappedState() {
  // Might've connected the pot the wrong way :^) Reversing min/max for now!!
  return inputs_scrollGetMappedState(0, 100);
}
