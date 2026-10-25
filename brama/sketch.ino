const byte buzzer = 2;

const byte ledPins[] = {17, 4, 16};
const byte buttonPins[] = {5, 18};

const byte CLOSED = 0;
const byte OPENING = 1;
const byte OPEN = 2;
const byte CLOSING = 3;
const byte STOPPED = 4;

byte state = CLOSED;

unsigned long startTime = 0;
unsigned long songTime = 0;
byte note = 0;

void playSong() {

  if (state != OPENING && state != CLOSING) {
    ledcWriteTone(buzzer, 0);
    note = 0;
    return;
  }

  if (millis() - songTime >= 300) {

    songTime = millis();

    if (note == 0) {
      ledcWriteTone(buzzer, 262);
      note = 1;
    }

    else if (note == 1) {
      ledcWriteTone(buzzer, 330);
      note = 2;
    }

    else if (note == 2) {
      ledcWriteTone(buzzer, 392);
      note = 3;
    }

    else {
      ledcWriteTone(buzzer, 0);
      note = 0;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ledcAttach(buzzer, 2000, 8);

for (byte i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
}

for (byte i = 0; i < 2; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
}
}

void loop() {
  playSong();
    switch (state) {

        case CLOSED:

          digitalWrite(ledPins[0], HIGH);
          digitalWrite(ledPins[1], LOW);
          digitalWrite(ledPins[2], LOW);

          if (digitalRead(buttonPins[0]) == LOW) {

            state = OPENING;
            startTime = millis();
            note = 0;
            Serial.println("Otwieramy brame.....");
          }

    break;


        case OPENING:

            digitalWrite(ledPins[0], LOW);
            digitalWrite(ledPins[1], HIGH);
            digitalWrite(ledPins[2], LOW);

            if(digitalRead(buttonPins[1]) == LOW){
              state = STOPPED;
              ledcWriteTone(buzzer, 0);
              Serial.println("Stop.");
            }

            if (millis() - startTime >= 3000) {

                state = OPEN;
                startTime = millis();

                Serial.println("Brama otwarta");
            }

            break;


        case OPEN:

            digitalWrite(ledPins[0], LOW);
            digitalWrite(ledPins[1], LOW);
            digitalWrite(ledPins[2], HIGH);

            if (millis() - startTime >= 5000) {

                state = CLOSING;
                startTime = millis();

                Serial.println("Zamykamy brame.....");
            }

            break;


        case CLOSING:

            digitalWrite(ledPins[0], LOW);
            digitalWrite(ledPins[1], HIGH);
            digitalWrite(ledPins[2], LOW);

            if (millis() - startTime >= 3000) {

                state = CLOSED;
                startTime = millis();

                Serial.println("Brama zamknieta");
            }

            break;

        case STOPPED:
            digitalWrite(ledPins[0], LOW);
            digitalWrite(ledPins[1], LOW);
            digitalWrite(ledPins[2], LOW);

            if(digitalRead(buttonPins[0]) == LOW){
              state = OPENING;
              startTime = millis();
              note = 0;

              Serial.println("Wznawiamy otwieranie....");
            }

            break;

    }
}
