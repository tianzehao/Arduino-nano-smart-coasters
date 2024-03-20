#include "HX711.h"
#include "OneButton.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <EEPROM.h>
#include "FireTimer.h"
#include <stdio.h>  

#define LED_PIN   11 //WS2812配置
#define LED_COUNT 8

#define PIN_INPUT 3 // 按键

#define SCREEN_WIDTH 128 // OLED直接接IIC接口
#define SCREEN_HEIGHT 32 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C

HX711 scale; //HX711
OneButton button(PIN_INPUT, true); // 按键
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //OLED
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); //WS2812
FireTimer msTimer; // 定时器

uint8_t dataPin = 6; //HX711
uint8_t clockPin = 7; //hx711
const int buzzerPin = 9; //蜂鸣器

// 全局变量
unsigned int water_null = 0; // 空杯重量
unsigned int water_full = 0; // 满水重量
unsigned int water_weight = 0; //当前重量
unsigned int water_percent = 0; // 当前水量百分比
long alarm_time[5] = {60000,900000,1800000,3600000,7200000}; // 喝水提醒时间间隔，默认1min(可选1、15、30、60、120)，修改此处可自定义提醒时间
unsigned int alarm_index = 0; // 喝水提醒时间间隔的索引号(0-4)
long firstPixelHue = 0; //LED ws2812彩虹模式参数
long now_time = 0; // 当前时间记录，用于计时

bool program_switch = 0; // 杯垫程序与称程序切换标识符

// Setup
void setup() {
  //HX711
  scale.begin(dataPin, clockPin);
  scale.tare();
  scale.set_scale(420.52); // 5kg称重传感器的系数
  // 按键
  button.attachClick(myClickFunction);
  button.attachDoubleClick(myDoubleClickFunction);
  button.attachMultiClick(multiClick);
  button.setDebounceMs(80);
  // OLED
  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS));
  // OLED清屏
  oled.clearDisplay();
  oled.display();
  oled.setTextSize(2); // 选择字号
  oled.setTextColor(WHITE);  // 字体颜色
  // WS2812
  strip.begin();
  strip.show(); // 关闭灯带
  strip.setBrightness(50); // 灯带亮度50 (最大255)
  // 无源蜂鸣器
  pinMode(buzzerPin, OUTPUT);
  // 定时器
  msTimer.begin(10);
  // 按键中断
  attachInterrupt(1, checkTicks, CHANGE);
  // 初始化随机数
  randomSeed(analogRead(0));
  // 初始提醒时间设定
  now_time = millis();
  // 从EEPROM读取数据
  alarm_index = EEPROM.read(0);
  water_null = EEPROM.read(10);
  water_full = EEPROM.read(20);
  /***初始化完成***/
  
  start_tone(); //播放开机音
}

// loop
void loop() {

  water_weight = scale.get_units(1); // 实时读取重量

 if(program_switch == 0) // 进入杯垫程序
 {

  if(water_null!=0 && water_weight >= (water_null-5)  &&  water_weight <= water_full) // 如果放置了杯子
  {
    water_percent = ((water_weight*1.0 - water_null*1.0)  / (water_full*1.0 - (water_null-5)*1.0))*100.0; // 计算水量百分比
    LEDdisplay(water_percent); // LED灯带显示实时水量
  }else if(water_null!=0 && water_weight < (water_null-5)){ // 没有放置杯子的情况
    for(int i=0;i<8;i++)
    {
      strip.setPixelColor(i, strip.Color(random(256),   random(256),   random(256)));
    }
    strip.show();
  }else{ // 其他情况，如杯子里面水过满、未进行水量校准等
    if(msTimer.fire()) // LED彩虹循环
      {
        strip.rainbow(firstPixelHue);
        strip.show();
        firstPixelHue += 256;
        if(firstPixelHue == 5*65536) firstPixelHue=0;
      }
  }
  
  // OLED显示倒计时间隔时间
  oled.clearDisplay();
  oled.setCursor(24,10);
  String oledstr = String(alarm_time[alarm_index] / 60000) + " min";
  oled.print(oledstr);
  oled.display();

  // 倒计时时间
  if(millis() - now_time >= alarm_time[alarm_index])
  {
    now_time = millis();
    for(unsigned int i=0; i<3; i++)
    {
      tone(buzzerPin,1046);delay(100);
      tone(buzzerPin,1568);delay(100);
      tone(buzzerPin,1318);delay(100);
      tone(buzzerPin,1568);delay(100);
    }
    noTone(buzzerPin);
  }

  button.tick(); // 按键服务
 }else{
  // 进入称的程序
  if(water_weight <= 5000)
  {
    oled.clearDisplay();
    oled.setCursor(36,10);
    String oledstr2 = String(water_weight) + " g";
    oled.print(oledstr2);
    oled.display();
  }
 }

}
// loop end******

// 开机音效函数
void start_tone()
{
  tone(buzzerPin,1046);
  delay(200);
  tone(buzzerPin,1175);
  delay(200);
  tone(buzzerPin,1318);
  delay(200);
  tone(buzzerPin,1568);
  delay(200);
  noTone(buzzerPin);
}

// LED ws2812进度条显示水量函数
void LEDdisplay(unsigned int water_percent)
{
  if(water_percent < 15)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 15  &&  water_percent <25)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 25  &&  water_percent <35)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 35  &&  water_percent <50)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else if(i==3) strip.setPixelColor(i, strip.Color(0,   255,   0));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 50  &&  water_percent <60)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else if(i==3) strip.setPixelColor(i, strip.Color(0,   255,   0));
      else if(i==4) strip.setPixelColor(i, strip.Color(0,   255,   255));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 60  &&  water_percent <70)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else if(i==3) strip.setPixelColor(i, strip.Color(0,   255,   0));
      else if(i==4) strip.setPixelColor(i, strip.Color(0,   255,   255));
      else if(i==5) strip.setPixelColor(i, strip.Color(0,   0,   255));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 70  &&  water_percent <85)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else if(i==3) strip.setPixelColor(i, strip.Color(0,   255,   0));
      else if(i==4) strip.setPixelColor(i, strip.Color(0,   255,   255));
      else if(i==5) strip.setPixelColor(i, strip.Color(0,   0,   255));
      else if(i==6) strip.setPixelColor(i, strip.Color(127,   101,   161));
      else strip.setPixelColor(i, strip.Color(0,   0,   0));
    }
    strip.show();
  }else if(water_percent >= 85)
  {
    for(unsigned int i = 0; i<8; i++)
    {
      if(i==0) strip.setPixelColor(i, strip.Color(255,   0,   0));
      else if(i==1) strip.setPixelColor(i, strip.Color(255,   165,   0));
      else if(i==2) strip.setPixelColor(i, strip.Color(255,   255,   0));
      else if(i==3) strip.setPixelColor(i, strip.Color(0,   255,   0));
      else if(i==4) strip.setPixelColor(i, strip.Color(0,   255,   255));
      else if(i==5) strip.setPixelColor(i, strip.Color(0,   0,   255));
      else if(i==6) strip.setPixelColor(i, strip.Color(127,   101,   161));
      else if(i==7) strip.setPixelColor(i, strip.Color(85,   43,   148));
    }
    strip.show();
  }
}

// 按键中断服务函数
void checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

// 单击按键
void myClickFunction() {
  alarm_index++;
  if(alarm_index == 5) alarm_index = 0;
  EEPROM.write(0, alarm_index);

  now_time = millis();
}  // myClickFunction

// 双击按键
void myDoubleClickFunction() {
  tone(buzzerPin,440);
  delay(50);
  noTone(buzzerPin); // 蜂鸣器提示音响一声
  oled.clearDisplay();
  oled.setCursor(14,8);
  oled.print("empty cup");
  oled.display(); // 显示屏提示放置空杯子
  water_null = scale.get_units(10);
  EEPROM.write(10,water_null);
  oled.clearDisplay();
  oled.setCursor(16,8);
  oled.print("-- OK --");
  oled.display(); // 提示OK
  delay(1000);
}  // myDoubleClickFunction

// 多次点击
void multiClick() {
  int n = button.getNumberClicks();
  if (n == 3) { // 
    tone(buzzerPin,440);
    delay(50);
    noTone(buzzerPin); // 蜂鸣器提示音响一声
    oled.clearDisplay();
    oled.setCursor(16,8);
    oled.print("full cup");
    oled.display(); // 显示屏提示放置空杯子
    water_full = scale.get_units(10);
    EEPROM.write(20,water_full);
    oled.clearDisplay();
    oled.setCursor(16,8);
    oled.print("-- OK --");
    oled.display(); // 提示OK
    delay(1000);
  } else if (n == 4) {
    // 进入称程序
    program_switch = 1;
  } else {
    
  }
} // multiClick