#include <Arduino.h>
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
int state;
int swSP = D3;
int swR = D4;
unsigned long cd = 0;
bool lastSP = HIGH;
unsigned long start = 0;
unsigned long count = 0;
unsigned long sec = 0;
unsigned long minu = 0;
unsigned long milli = 0;
void countdown(long in);
String show;

void setup()
{
  Serial.begin(115200);
  lcd.init();
  Wire.begin();
  pinMode(swSP, INPUT);
  pinMode(swR, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  lcd.backlight();
  state = 0;
}

void loop()
{
  bool curSP = digitalRead(swSP);
  if (state == 0)
  {
    lcd.setCursor(3, 0);
    lcd.print("Press Start");
    lcd.setCursor(0, 1);
    lcd.print("TypeToCountDown");
    if (lastSP == HIGH && curSP == LOW)
    {
      lcd.clear();
      state = 1;
      start = millis();
    }
    lastSP = curSP;
  }
  if (state == 1)
  {
    count = millis() - start;
    minu = (count / 60000);
    sec = (count / 1000) % 60;
    milli = count % 1000;
    lcd.setCursor(2, 0);
    lcd.print("Group : HOPE");
    show = String(minu) + ":" + String(sec) + ":" + milli + " ";
    lcd.setCursor(4, 1);
    lcd.print(show);
    digitalWrite(D6, HIGH);
    digitalWrite(D5, LOW);
    if (lastSP == HIGH && curSP == LOW)
    {
      state = 2;
    }
    lastSP = curSP;
    if (digitalRead(swR) == 0)
    {

      while (Serial.available() > 0)
      {
        Serial.read();
      }

      lcd.clear();
      state = 0;
      show = 0;
    }
  }
  if (state == 2)
  {
    start = millis() - count;
    digitalWrite(D6, LOW);
    digitalWrite(D5, HIGH);
    if (lastSP == HIGH && curSP == LOW)
    {
      state = 1;
    }
    lastSP = curSP;
    if (digitalRead(swR) == 0)
    {
      while (Serial.available() > 0)
      {
        Serial.read();
      }
      cd = 0;
      lcd.clear();
      state = 0;
      show = 0;
    }
  }
  if (state != 1 && state != 2)
  {
    if (Serial.available() > 0)
    {
      lcd.clear();
      while (Serial.available() > 0)
      {
        Serial.read();
      }
      state = 3;
    }
  }

  if (state == 3)
  {
    lcd.setCursor(2, 0);
    lcd.print("Set HH:MM:SS");

    while (Serial.available() == 0)
    {
      if (digitalRead(swR) == 0)
      {
        lcd.clear();
        state = 0;
        show = 0;
        break;
      }
    }
    cd = Serial.parseInt();

    Serial.println(cd);
    if (cd == 0)
      state = 0;
    else
    {
      lcd.clear();
      countdown(cd);
    }
  }
}
void countdown(long in)
{
  lcd.clear();
  unsigned long h = (in / 60000) * 3600 * 1000;
  unsigned long m = ((in / 100) % 100) * 60 * 1000;
  unsigned long s = (in % 100) * 1000;
  unsigned long count = h + m + s;
  unsigned long time = count;
  unsigned long millisStart = millis();
  while (h >= 0 && m >= 0 && s >= 0)
  {
    if (digitalRead(swR) == 0)
    {
      while (Serial.available() > 0)
      {
        Serial.read();
      }
      lcd.clear();
      state = 0;
      show = 0;
      break;
    }
    lcd.setCursor(0, 0);
    lcd.print("CountDown");
    lcd.setCursor(0, 1);
    count = time - (millis() - millisStart);
    s = (count / 1000) % 60;
    m = (count / 60000) % 60;
    h = (count / 60000) / 60;
    lcd.print(String(h) + ":" + String(m) + ":" + String(s) + " ");
    if (h == 0 && m == 0 && s == 0)
    {
      while (Serial.available() > 0)
      {
        Serial.read();
      }
      lcd.clear();
      cd = 0;
      state = 0;
      break;
    }
    yield();
  }
}