#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHT_PIN 15
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int GREEN_LED = 25;
const int YELLOW_LED = 26;
const int RED_LED = 27;
const int BUZZER = 14;

enum State {
  NORMAL,
  WARNING,
  CRITICAL
};

State state = NORMAL;

void setup() {

  Serial.begin(115200);

  dht.begin();

  lcd.init();
  lcd.backlight();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void setState(State newState) {

  state = newState;

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER);

  switch (state) {

    case NORMAL:
      digitalWrite(GREEN_LED, HIGH);
      break;

    case WARNING:
      digitalWrite(YELLOW_LED, HIGH);
      break;

    case CRITICAL:
      digitalWrite(RED_LED, HIGH);
      tone(BUZZER, 1000);
      break;
  }
}

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {

    Serial.println("DHT ERROR");

    delay(2000);

    return;
  }

  if (temperature < 25) {
    setState(NORMAL);
  }
  else if (temperature < 30) {
    setState(WARNING);
  }
  else {
    setState(CRITICAL);
  }

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);

  if (state == NORMAL) {
    lcd.print("NORMAL");
  }
  else if (state == WARNING) {
    lcd.print("WARNING");
  }
  else {
    lcd.print("CRITICAL");
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);

  Serial.print(" Humidity: ");
  Serial.print(humidity);

  Serial.print(" State: ");

  if (state == NORMAL) {
    Serial.println("NORMAL");
  }
  else if (state == WARNING) {
    Serial.println("WARNING");
  }
  else {
    Serial.println("CRITICAL");
  }

  delay(2000);
}