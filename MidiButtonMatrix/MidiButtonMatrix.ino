#include <Adafruit_VS1053.h>

#include <SoftwareSerial.h>

// define the pins used
#define VS1053_RX  2
#define VS1053_RESET 9

#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

byte rows[] = {22, 24, 26, 28, 30, 32, 34, 36};
const int rowCount = sizeof(rows) / sizeof(rows[0]);
byte cols[] = {23, 25, 27, 29, 31, 33, 35, 37};
const int colCount = sizeof(cols) / sizeof(cols[0]);
int potiWert = 0;
byte noteValue[1][8][8];

byte keys[colCount][rowCount];
byte oldKeys[colCount][rowCount];

byte startValue = 25;

SoftwareSerial VS1053_MIDI(0, 2);

void setup() {
  readMatrix();
  Serial.begin(9600);
  VS1053_MIDI.begin(31250); //
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      noteValue[0][i][j] = startValue;
      startValue++;
      oldKeys[i][j] = keys[i][j];
    }
  }


  for (int x = 0; x < rowCount; x++) {
    pinMode(rows[x], INPUT);
  }

  for (int x = 0; x < colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }

  pinMode(VS1053_RESET, OUTPUT);
  digitalWrite(VS1053_RESET, LOW);
  delay(10);
  digitalWrite(VS1053_RESET, HIGH);
  delay(10);
  midiSetChannelBank(0, VS1053_BANK_DRUMS1);
  midiSetInstrument(0, 1);
  midiSetChannelVolume(0, 50);
}

void loop() {
  potiWert = analogRead(A8);
  Serial.println(potiWert);
  readMatrix();
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (keys[i][j] != oldKeys[i][j]) {
        if (keys[i][j] == 0) {
          midiNoteOn(0, noteValue[0][i][j], analogRead(A8) / 10);
          Serial.println("StartetNote");
          oldKeys[i][j] = keys[i][j];
        }
        else if (keys[i][j] == 1) {
          midiNoteOff(0, noteValue[0][i][j], analogRead(A8) / 10);
          Serial.println("Stoppt Note");
          oldKeys[i][j] = keys[i][j];
        }
      }

    }
  }
}
