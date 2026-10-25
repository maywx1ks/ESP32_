#include <Arduino.h>
#include "DHT.h"

const byte ALARM = 0;
const byte OKEJ = 1;
const byte JESTCIEPLO = 2;

byte state = OKEJ;
byte note = 0;

const int DHT_PIN = 15;
const byte buzzer = 5;

const byte ledPins[] = {2, 4, 16}; // red, yellow, green

const byte potentiometr = 34;

const int melody[] = {262, 330, 392, 523};
const byte melodyLength = sizeof(melody) / sizeof(melody[0]);

DHT dht(DHT_PIN, DHT22);

unsigned long songTime = 0;
unsigned long serialTime = 0;

void setup() {

  Serial.begin(115200);

  dht.begin();
  ledcAttach(buzzer, 2000, 8);
  for (byte i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void playSong(float temperature, int threshold) {

  if (temperature < threshold) {
    ledcWriteTone(buzzer, 0);
    note = 0;
    return;
  }

  if (millis() - songTime >= 300) {

    songTime = millis();

    ledcWriteTone(buzzer, melody[note]);

    note++;

    if (note >= melodyLength) {
      note = 0;
    }
  }
}

void setState(float temperature) {

  if (temperature >= 30) {
    state = ALARM;
  }
  else if (temperature >= 20) {
    state = JESTCIEPLO;
  }
  else {
    state = OKEJ;
  }
}

void showLeds() {

  switch (state) {

    case ALARM:

      digitalWrite(ledPins[0], HIGH);
      digitalWrite(ledPins[1], LOW);
      digitalWrite(ledPins[2], LOW);

      break;

    case JESTCIEPLO:

      digitalWrite(ledPins[0], LOW);
      digitalWrite(ledPins[1], HIGH);
      digitalWrite(ledPins[2], LOW);

      break;

    case OKEJ:

      digitalWrite(ledPins[0], LOW);
      digitalWrite(ledPins[1], LOW);
      digitalWrite(ledPins[2], HIGH);

      break;
  }
}

void printStatus(float temperature, int threshold) {

  if (millis() - serialTime < 1000) {
    return;
  }

  serialTime = millis();

  Serial.println("Temperatura: " + String(temperature, 1) + " C");
  Serial.println("Prog alarmu: " + String(threshold) + " C");

  if (temperature >= threshold) {
    Serial.println("ALARM!");
  }
  else {
    Serial.println("OK");
  }

  Serial.println();
}

void loop() {

  int valuePotentiometr = analogRead(potentiometr);

  int threshold = map(
    valuePotentiometr,
    0,
    4095,
    20,
    30
  );

  float temperature = dht.readTemperature();

  if (isnan(temperature)) {

    ledcWriteTone(buzzer, 0);

    return;
  }

  setState(temperature);

  showLeds();

  playSong(temperature, threshold);

  printStatus(temperature, threshold);
}