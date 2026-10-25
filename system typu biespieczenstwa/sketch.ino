#include <DHT.h>
#include <Keypad.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHT_PIN 15
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo lockServo;
const int TRIG = 5;
const int ECHO = 18;
const int PIR = 19;
const int SERVO_PIN = 4;
const int GREEN_LED = 12;
const int RED_LED = 2;
const int BUZZER = 23;
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {
  32, 33, 25, 26
};

byte colPins[COLS] = {
  27, 14, 16, 13
};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

enum SystemState {
  DISARMED,
  ARMED,
  ALARM
};

SystemState state = DISARMED;
String enteredPIN = "";
const String correctPIN = "1234";
unsigned long lastSensorCheck = 0;

float getDistance() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

void showState() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM:");

  lcd.setCursor(0, 1);

  if (state == DISARMED) {
    lcd.print("DISARMED");
  }
  else if (state == ARMED) {
    lcd.print("ARMED");
  }
  else {
    lcd.print("!!! ALARM !!!");
  }
}

void changeState(SystemState newState) {

  state = newState;

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER);

  if (state == DISARMED) {

    digitalWrite(GREEN_LED, HIGH);

    lockServo.write(90);

    Serial.println("SYSTEM DISARMED");

  }

  else if (state == ARMED) {

    digitalWrite(RED_LED, HIGH);

    lockServo.write(0);

    Serial.println("SYSTEM ARMED");

  }

  else if (state == ALARM) {

    digitalWrite(RED_LED, HIGH);

    tone(BUZZER, 1000);

    Serial.println("!!! ALARM !!!");
  }

  showState();
}

void checkPIN() {

  Serial.print("Entered PIN: ");
  Serial.println(enteredPIN);

  if (enteredPIN == correctPIN) {

    Serial.println("CORRECT PIN");

    if (state == DISARMED) {

      changeState(ARMED);

    }
    else {

      changeState(DISARMED);
    }

  }
  else {

    Serial.println("WRONG PIN");

    tone(BUZZER, 1500, 500);
  }

  enteredPIN = "";
}

void checkSensors() {

  if (state != ARMED) {
    return;
  }

  float temperature = dht.readTemperature();

  float distance = getDistance();

  int motion = digitalRead(PIR);


  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Distance: ");
  Serial.println(distance);

  Serial.print("Motion: ");
  Serial.println(motion);


  if (motion == HIGH) {

    Serial.println("MOTION DETECTED");

    changeState(ALARM);

    return;
  }


  if (distance < 50) {

    Serial.println("OBJECT TOO CLOSE");

    changeState(ALARM);

    return;
  }

  if (!isnan(temperature) && temperature >= 35) {

    Serial.println("TEMPERATURE TOO HIGH");

    changeState(ALARM);

    return;
  }
}

void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(PIR, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  lockServo.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();

  changeState(DISARMED);

  Serial.println();
  Serial.println("SECURITY SYSTEM");
  Serial.println("Enter PIN");
}

void loop() {

  char key = keypad.getKey();

  if (key != NO_KEY) {

    Serial.print("Key: ");
    Serial.println(key);

    if (key == '*') {

      enteredPIN = "";

      Serial.println("PIN CLEARED");
    }

    else if (key == '#') {

      checkPIN();
    }

    else if (key >= '0' && key <= '9') {

      if (enteredPIN.length() < 4) {

        enteredPIN += key;

        Serial.print("PIN: ");

        for (int i = 0; i < enteredPIN.length(); i++) {
          Serial.print("*");
        }

        Serial.println();
      }
    }
  }


  if (millis() - lastSensorCheck >= 1000) {

    lastSensorCheck = millis();

    checkSensors();
  }
}