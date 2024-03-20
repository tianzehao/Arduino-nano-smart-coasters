#include "OneButton.h"

#define PIN_INPUT 3

OneButton button(PIN_INPUT, true);

void checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

void singleClick() {
  Serial.println("singleClick() detected.");
} // singleClick

void doubleClick() {
  Serial.println("doubleClick() detected.");
} // doubleClick


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("One Button Example with interrupts.");

  attachInterrupt(/*digitalPinToInterrupt(PIN_INPUT)*/1, checkTicks, CHANGE);

  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  button.tick();
  digitalWrite(13, HIGH);
  delay(30);
  digitalWrite(13, LOW);
  delay(30);
}
