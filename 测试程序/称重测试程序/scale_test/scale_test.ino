#include "HX711.h"

HX711 scale;

uint8_t dataPin = 6;
uint8_t clockPin = 7;
//uint8_t dataPin  = 19;//for esp32
//uint8_t clockPin = 18;//for esp32

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  scale.begin(dataPin, clockPin);

  //scale.set_scale(127.15);

  //Serial.print("UNITS: ");
  //Serial.println(scale.get_units(10));

  /*
  Serial.println("\nEmpty the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();*/

  scale.tare();
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  scale.set_scale(420.52);

/*
  Serial.println("\nPut 1000 gram in the scale, press a key to continue");
  while(!Serial.available());
  while(Serial.available()) Serial.read();

  scale.calibrate_scale(2488.4, 5);
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  Serial.println("\nScale is calibrated, press a key to continue");
  // Serial.println(scale.get_scale());
  // Serial.println(scale.get_offset());
  while(!Serial.available());
  while(Serial.available()) Serial.read();
  delay(3000);*/
}


void loop()
{
  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
  delay(250);
}


// -- END OF FILE --