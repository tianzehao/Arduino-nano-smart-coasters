const int buzzerPin = 9;
int fre;

void setup()
{
  pinMode(buzzerPin, OUTPUT);
}

void loop()
{
  /*
  //频率从200到800
  for (int i = 200; i <= 800; i++)
  {
    tone(buzzerPin, i);
    delay(5);
  }
  for (int i = 800; i >= 200; i--)
  {
    tone(buzzerPin, i);
    delay(10);
  }*/
  
  tone(buzzerPin,261.6255653005986);
  delay(100);
  tone(buzzerPin,293.66476791740763);
  delay(100);
  tone(buzzerPin,329.62755691286986);
  delay(100);
  tone(buzzerPin,349.22823143300394);
  delay(100);
  tone(buzzerPin,391.9954359817492);
  delay(100);
  tone(buzzerPin,440);
  delay(100);
  tone(buzzerPin,493.88330125612424);
  delay(100);
  tone(buzzerPin,523.2511306011972);
  delay(100);

}