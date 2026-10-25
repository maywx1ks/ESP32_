#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define SS_PIN 5
#define RST_PIN 22

MFRC522 rfid(SS_PIN, RST_PIN);

Servo door;

const int SERVO_PIN = 13;
const int RED_LED = 26;
const int GREEN_LED = 27;
const int BUZZER = 25;

byte allowedUID[] = {
  0x01,
  0x02,
  0x03,
  0x04
};

bool checkUID() {

  for (byte i = 0; i < 4; i++) {

    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      return false;
    }
  }

  return true;
}

void accessGranted() {

  Serial.println("ACCESS GRANTED");

  digitalWrite(GREEN_LED, HIGH);

  door.write(90);

  tone(BUZZER, 2000, 200);

  delay(3000);

  door.write(0);

  digitalWrite(GREEN_LED, LOW);
}

void accessDenied() {

  Serial.println("ACCESS DENIED");

  digitalWrite(RED_LED, HIGH);

  tone(BUZZER, 500, 1000);

  delay(1000);

  digitalWrite(RED_LED, LOW);
}

void setup() {

  Serial.begin(115200);

  SPI.begin();

  rfid.PCD_Init();

  door.attach(SERVO_PIN);
  door.write(0);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("RFID ACCESS SYSTEM");
  Serial.println("Scan card...");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  if (checkUID()) {
    accessGranted();
  } else {
    accessDenied();
  }

  rfid.PICC_HaltA();
}