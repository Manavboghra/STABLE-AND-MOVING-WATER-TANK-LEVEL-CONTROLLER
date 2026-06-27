#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int delayTime = 100;

// Level controller
const float speedOfSound = 0.034;
const float tankHeight = 250; // in cm
const float maxLevel = 200;
const float minLevel = 50;
const float maxAllowableLevel = 225;
bool levelControlActive = true;
bool waterLevelDangerouslyHigh = false;

// Tilt
bool isTilted = false;

// Temperature (TMP36 on A0)
const int tempPin = A0;
const float maxSafeTemp = 60.0;
bool tempDangerouslyHigh = false;
float currentTemp = 0.0;

// Digital pins
const byte trigPin = 2;
const byte echoPin = 4;
const byte buzzerPin = 6;
const byte motorPin = 7;
const byte tiltSensorPin = 8;

LiquidCrystal_I2C lcd(0x20, 16, 2);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(motorPin, OUTPUT);

  digitalWrite(trigPin, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(tiltSensorPin, HIGH);
  digitalWrite(motorPin, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("  IoT Group 20   ");
  lcd.setCursor(0, 1);
  lcd.print("  Starting...   ");
  delay(2000);
  lcd.clear();
}

void beepAlert()
{
  for (int i = 0; i < 4; i++)
  {
    tone(buzzerPin, 800);
    delay(30);
    noTone(buzzerPin);
  }
}

// SENSOR READERS
float measureDistance()
{
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  return (duration * speedOfSound) / 2;
}

float readTemperature()
{
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1023.0);
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;
}

// CHECK FUNCTIONS
void checkTilt()
{
  if (digitalRead(tiltSensorPin) == LOW)
  {
    isTilted = true;
    levelControlActive = false;
    beepAlert();
  }
  else
  {
    isTilted = false;
    levelControlActive = true;
    if (!waterLevelDangerouslyHigh && !tempDangerouslyHigh)
      noTone(buzzerPin);
  }
}

void checkTemperature()
{
  currentTemp = readTemperature();

  Serial.print("Temp: ");
  Serial.print(currentTemp);
  Serial.println(" C");

  if (currentTemp >= maxSafeTemp)
  {
    tempDangerouslyHigh = true;
    beepAlert();
  }
  else
  {
    tempDangerouslyHigh = false;
    if (!waterLevelDangerouslyHigh && !isTilted)
      noTone(buzzerPin);
  }
}

void controlLevel()
{
  if (levelControlActive)
  {
    float level = tankHeight - measureDistance();

    if (level < minLevel)
      digitalWrite(motorPin, HIGH);

    if (level >= maxLevel)
      digitalWrite(motorPin, LOW);

    if (level >= maxAllowableLevel)
    {
      waterLevelDangerouslyHigh = true;
      beepAlert();
    }
    else
    {
      waterLevelDangerouslyHigh = false;
      if (!isTilted && !tempDangerouslyHigh)
        noTone(buzzerPin);
    }
  }
  else
  {
    digitalWrite(motorPin, LOW);
  }
}


void refreshScreen()
{
  float level = tankHeight - measureDistance();

  // ── Line 1: priority alert OR water level ──
  lcd.setCursor(0, 0);

  if (isTilted)
  {
    lcd.print("!! TANK TILTED !!");
  }
  else if (waterLevelDangerouslyHigh)
  {
    lcd.print("!! OVERFLOW !!  ");
  }
  else if (level < minLevel)
  {
    lcd.print("!! Low Water !!  ");
  }
  else if (tempDangerouslyHigh)
  {
    lcd.print("!! TEMP HIGH !! ");
  }
  else
  {
    // Normal — show water level
    String motorState = digitalRead(motorPin) == HIGH ? "ON " : "OFF";
    String lvlStr = "Lvl:";
    lvlStr += String((int)level);
    lvlStr += "cm M:";
    lvlStr += motorState;
    while (lvlStr.length() < 16) lvlStr += " ";
    lcd.print(lvlStr);
  }

  // ── Line 2: T, P, Tilt — always shown ──
  lcd.setCursor(0, 1);

  String line2 = "T:";
  line2 += String((int)currentTemp);
  line2 += "% Ti:";
  line2 += isTilted ? "Y" : "N";

  // Pad to 16 chars
  while (line2.length() < 16) line2 += " ";
  lcd.print(line2);
}

// BLINK
void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

// LOOP
void loop()
{
  blink();
  checkTilt();
    checkTemperature();
  controlLevel();
  refreshScreen();
  delay(delayTime);
}