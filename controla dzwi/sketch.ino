#include <ESP32Servo.h>

Servo door;

const int BUTTON = 4;
const int SERVO_PIN = 13;

const int GREEN_LED = 26;
const int RED_LED = 27;
const int BUZZER = 25;

enum State {
  LOCKED,
  UNLOCKING,
  OPEN,
  ALARM
};

State state = LOCKED;

unsigned long stateStartTime = 0;

bool lastButtonState = HIGH;

void changeState(State newState) {

  state = newState;

  stateStartTime = millis();

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER);

  switch (state) {

    case LOCKED:

      door.write(0);

      digitalWrite(RED_LED, HIGH);

      Serial.println("LOCKED");

      break;

    case UNLOCKING:

      Serial.println("UNLOCKING");

      door.write(90);

      break;

    case OPEN:

      digitalWrite(GREEN_LED, HIGH);

      Serial.println("OPEN");

      break;

    case ALARM:

      digitalWrite(RED_LED, HIGH);

      tone(BUZZER, 1000);

      Serial.println("ALARM");

      break;
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  door.attach(SERVO_PIN);

  changeState(LOCKED);
}

void loop() {

  bool buttonState = digitalRead(BUTTON);

  if (lastButtonState == HIGH && buttonState == LOW) {

    if (state == LOCKED) {

      changeState(UNLOCKING);

    } else if (state == OPEN) {

      changeState(LOCKED);

    } else if (state == ALARM) {

      changeState(LOCKED);
    }

    delay(200);
  }

  lastButtonState = buttonState;

  if (state == UNLOCKING) {

    if (millis() - stateStartTime >= 1000) {

      changeState(OPEN);
    }
  }

  if (state == OPEN) {

    if (millis() - stateStartTime >= 5000) {

      changeState(LOCKED);
    }
  }

  if (state == ALARM) {

    if (millis() - stateStartTime >= 5000) {

      changeState(LOCKED);
    }
  }
}