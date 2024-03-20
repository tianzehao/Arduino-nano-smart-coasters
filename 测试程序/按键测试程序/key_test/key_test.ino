#include "OneButtonTiny.h"

// The actions I ca do...
typedef enum {
  ACTION_OFF,   // set LED "OFF".
  ACTION_ON,    // set LED "ON"
  ACTION_SLOW,  // blink LED "SLOW"
  ACTION_FAST   // blink LED "FAST"
} MyActions;

#define PIN_INPUT A1
#define PIN_LED 13

OneButtonTiny button(PIN_INPUT, true);

MyActions nextAction = ACTION_OFF;  // no action when starting



void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);  // sets the digital pin as output
  button.attachClick(myClickFunction);
  button.attachDoubleClick(myDoubleClickFunction);
  button.attachLongPressStart(myDoubleClickFunction);
  button.setDebounceMs(80);  // set 80 msec. debouncing time. Default is 50 msec.
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long now = millis();

  // keep watching the push button:
  button.tick();

  if (nextAction == ACTION_OFF) {
    // do nothing.
    digitalWrite(PIN_LED, LOW);

  } else if (nextAction == ACTION_ON) {
    // turn LED on
    digitalWrite(PIN_LED, HIGH);

  } else if (nextAction == ACTION_SLOW) {
    // do a slow blinking
    if (now % 1000 < 500) {
      digitalWrite(PIN_LED, LOW);
    } else {
      digitalWrite(PIN_LED, HIGH);
    }  // if

  } else if (nextAction == ACTION_FAST) {
    // do a fast blinking
    if (now % 200 < 100) {
      digitalWrite(PIN_LED, LOW);
    } else {
      digitalWrite(PIN_LED, HIGH);
    }  // if
  }    // if

}

// this function will be called when the button was pressed 1 time and them some time has passed.
void myClickFunction() {
  if (nextAction == ACTION_OFF)
    nextAction = ACTION_ON;
  else
    nextAction = ACTION_OFF;
}  // myClickFunction


// this function will be called when the button was pressed 2 times in a short timeframe.
void myDoubleClickFunction() {
  if (nextAction == ACTION_ON) {
    nextAction = ACTION_SLOW;

  } else if (nextAction == ACTION_SLOW) {
    nextAction = ACTION_FAST;

  } else if (nextAction == ACTION_FAST) {
    nextAction = ACTION_ON;
  }  // if
}  // myDoubleClickFunction


// this function will be called when a long press was detected.
void myLongPressFunction() {
  nextAction = ACTION_OFF;
}  // myLongPressFunction
