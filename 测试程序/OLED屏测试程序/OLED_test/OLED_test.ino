#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String scaletxt = "";

void setup() {
  // put your setup code here, to run once:

  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  oled.clearDisplay();
  oled.display();

  oled.drawLine(65, 0, 65, 32, SSD1306_WHITE);
  oled.display();

  oled.setTextSize(2); //选择字号
  oled.setTextColor(WHITE);  //字体颜色
  oled.setCursor(0,8);
  oled.print("123 g");

  oled.setCursor(73,8);
  oled.print("123m");

  oled.display();
  delay(1000);
  oled.clearDisplay();
  oled.display();

  delay(3000);
  scaletxt = "155 g";
  oled.setCursor(0,8);
  oled.print(scaletxt);
  oled.display();

  oled.drawRect(0,0,128,32,1);
  oled.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  for(unsigned int i=0; i<1000; i++)
  {
    scaletxt = String(i) + " g";
    oled.setCursor(0,8);
    oled.print(scaletxt);
    oled.display();
  }*/

}
