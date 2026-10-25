#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int trigPins[3] = {5, 17, 4};
const int echoPins[3] = {18, 16, 2};

const int ledPins[3] = {25, 26, 27};

bool occupied[3];

const int MAX_DISTANCE = 100;

float getDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

void setup() {

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 3; i++) {

    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);

    pinMode(ledPins[i], OUTPUT);
  }

  lcd.setCursor(0, 0);
  lcd.print("SMART PARKING");

  delay(2000);

  lcd.clear();
}

void loop() {

  int freePlaces = 0;

  for (int i = 0; i < 3; i++) {

    float distance = getDistance(
      trigPins[i],
      echoPins[i]
    );

    occupied[i] = distance < MAX_DISTANCE;

    digitalWrite(
      ledPins[i],
      occupied[i] ? HIGH : LOW
    );

    if (!occupied[i]) {
      freePlaces++;
    }

    Serial.print("Place ");
    Serial.print(i + 1);
    Serial.print(": ");

    if (occupied[i]) {
      Serial.println("OCCUPIED");
    } else {
      Serial.println("FREE");
    }
  }

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Free: ");
  lcd.print(freePlaces);
  lcd.print("/3");

  lcd.setCursor(0, 1);

  for (int i = 0; i < 3; i++) {

    lcd.print(i + 1);

    if (occupied[i]) {
      lcd.print("X ");
    } else {
      lcd.print("O ");
    }
  }

  delay(1000);
}