
#include "Tlc5940.h"
#define ColumnOne 18      //Gate of 1.MOSFET which connects to Column "1" (anodes of all Strings in Column "1")
#define ColumnTwo 19      //Gate of 2.MOSFET which connects to Column "2" (anodes of all StringS in Column "2")
#define ColumnThree 2    //Gate of 3.MOSFET which connects to Column "3" (anodes of all Strings in Column "3")
#define ColumnFour 4     //Gate of 4.MOSFET which connects to Column "4" (anodes of all Strings in Column "4")
#define ColumnFive 14      //Gate of 5.MOSFET which connects to Column "5" (anodes of all Strings in Column "5")
#define ColumnSix 15      //Gate of 6.MOSFET which connects to Column "6" (anodes of all StringS in Column "6")
#define ColumnSeven 16    //Gate of 7.MOSFET which connects to Column "7" (anodes of all Strings in Column "7")
#define ColumnEight 17     //Gate of 8.MOSFET which connects to Column "8" (anodes of all Strings in Column "8")

#define chargingProcessPin 7 //Pin seven is for checking if the board is currently charging
#define chargingFullPin 6 //Pin six is for checking if the board is fully charge
#define playModePin 12  //Pin twwelche checks if the board is in play mode

byte redValue[8];   //saves the values of one row red LED's
byte greenValue[8]; //saves the values of one row green LED's
byte blueValue[8];  //saves the values of one row blue LED's

byte RedLeds[8][8];
byte GreenLeds[8][8];
byte BlueLeds[8][8];

int redLedsStrip[4];
int greenLedsStrip[4];
int blueLedsStrip[4];

byte rgbCase[8];
boolean isPressedDown[8][8];

byte imageZustand = 0;
bool chargingStateProcess = LOW;
bool chargingFullProcess = LOW;
bool playModeProcess = LOW;

byte switchCase = 0;
int ColumnDuration = 2042;     // ON time of each Layer in microseconds
int column = 0;                // starting with Column 0
unsigned long oldMicros = 0;// starting counter to decide when if(micros()>=oldMicros)is true to change active layer, count++1, MatrixUpdate(); don't know what micros() is? Look here: http://arduino.cc/de/Reference/Micros#.UygzAIWmWd4
int count = 0;              // starting counter which is included in if() statement to change the shown picture, not necessary when using a static picture
int stretch = 8;           // set timer for the change of the shown picture, if animation should move really fast then low stretch value, if animation should move really slow than big stretch value
int bright = 255 ;          // set brightness for all Strings(4095=20mA, 0=0ma, 1000=4.88mA,....). If you want individually brightness for a String you have to change the value in tab "function"

float timePassed = 0;   //

void setup()
{
  //Initialize the baud rate
  Serial.begin(9600);

  //Check the state pins
  pinMode(chargingProcessPin, INPUT_PULLUP);
  pinMode(chargingFullPin, INPUT_PULLUP);
  pinMode(playModePin, INPUT_PULLUP);

  //Initialize the Columns
  pinMode(ColumnOne, OUTPUT);  // declare arduino nano pin A0(ColumnOne) as OUTPUT
  pinMode(ColumnTwo, OUTPUT);  // declare arduino nano pin A1(ColumnTwo) as OUTPUT
  pinMode(ColumnThree, OUTPUT);// declare arduino nano pin A2(ColumnThree) as OUTPUT
  pinMode(ColumnFour, OUTPUT); // declare arduino nano pin A3(ColumnFour) as OUTPUT
  pinMode(ColumnFive, OUTPUT);  // declare arduino nano pin A4(ColumnFive) as OUTPUT
  pinMode(ColumnSix, OUTPUT);  // declare arduino nano pin A5(ColumnSix) as OUTPUT
  pinMode(ColumnSeven, OUTPUT);// declare arduino nano pin D2(ColumnSeven) as OUTPUT
  pinMode(ColumnEight, OUTPUT); // declare arduino nano pin D4(ColumnrEight) as OUTPUT

  //Initialize the TLC's
  Tlc.init();               // configures the arduino to use the tlc5940, be sure to connect the arduino correctly to the tlc

  reset();

  //Read the state pins
  chargingStateProcess = digitalRead(chargingProcessPin);
  chargingFullProcess = digitalRead(chargingFullPin);
  playModeProcess = digitalRead(playModePin);

  //Check if it should display "charging", "fullyCharged" or "isInPlaymode"
  if (playModeProcess == LOW) {
    for (int i = 0; i < 8; i++) {
      redValue[i] = i * 34;
    }
    for (int i = 0; i < 8; i++) {
      greenValue[i] = 0;
    }
    for (int i = 0; i < 8; i++) {
      blueValue[i] = 255 - 34 * i;
      BlueValueChangeRow(blueValue[i], i);
      rgbCase[i] = 1;
    }
    imageZustand = 2;
  }
  else if (chargingStateProcess == HIGH && chargingFullProcess == LOW && playModeProcess == HIGH) {
    imageZustand = 1;
  } else {
    IsCharging();
    imageZustand = 0;
  }

}

void loop() {
  if (micros() >= oldMicros) {                                        // waits until LayerDuration is reached and than goes through cycle
    oldMicros = micros() + ColumnDuration;                             // updates oldMicros value by adding the micros() with LayerDuration
    switch (imageZustand) {
      case 0:
        Tlc.update();
        
        chargingStateProcess = digitalRead(chargingProcessPin);
        chargingFullProcess = digitalRead(chargingFullPin);
        playModeProcess = digitalRead(playModePin);

        if (chargingStateProcess == HIGH && chargingFullProcess == LOW && playModeProcess == HIGH) {
          imageZustand = 1;
          count = 0;
          for (int i = 0; i < 4; i++) {
            redLedsStrip[i] = 0;
            Tlc.set(24 + i, redLedsStrip[i]);             // set H1B brightness to HchB OUTPUT(OUTPUT 7);
          }
        }
        else if (playModeProcess == LOW) {
          for (int i = 0; i < 8; i++) {
            redValue[i] = i * 34;
          }
          for (int i = 0; i < 8; i++) {
            greenValue[i] = 0;
          }
          for (int i = 0; i < 8; i++) {
            blueValue[i] = 255 - 34 * i;
            BlueValueChangeRow(blueValue[i], i);
            rgbCase[i] = 1;
          }
          for (int i = 0; i < 4; i++) {
            redLedsStrip[i] = 0;
            greenLedsStrip[i] = 0;
            Tlc.set(24 + i, redLedsStrip[i]);             // set H1B brightness to HchB OUTPUT(OUTPUT 7);
            Tlc.set(28 + i, greenLedsStrip[i]);             // set H1B brightness to HchB OUTPUT(OUTPUT 7);
          }
          count = 0;
          imageZustand = 2;
        }

        break;

      case 1:
        if (count == 4) {
          FullyCharged();
        }

        chargingStateProcess = digitalRead(chargingProcessPin);
        chargingFullProcess = digitalRead(chargingFullPin);
        playModeProcess = digitalRead(playModePin);

        if (playModeProcess == LOW) {
          for (int i = 0; i < 4; i++) {
            greenLedsStrip[i] = 0;
            Tlc.set(28 + i, greenLedsStrip[i]);             // set H1B brightness to HchB OUTPUT(OUTPUT 7);
          }
          for (int i = 0; i < 8; i++) {
            redValue[i] = i * 34;
          }
          for (int i = 0; i < 8; i++) {
            greenValue[i] = 0;
          }
          for (int i = 0; i < 8; i++) {
            blueValue[i] = 255 - 34 * i;
            BlueValueChangeRow(blueValue[i], i);
            rgbCase[i] = 1;
          }
          count = 0;
          imageZustand = 2;
        }
        break;

      case 2:
        if (count == stretch) {
          RainbowCycle();
        }
        MatrixUpdate(column);                                                 // sets the values for the tlc5940 Outputs and puts all MOSFET Gates HIGH (not active) except for one MOSFET Low (active) -->this layer is ON, also look under tab "function"
        column++;                                                           // layer counter +1, so MatrixUpdate(layer) will affect the next layer during the next if() cycle

        chargingStateProcess = digitalRead(chargingProcessPin);
        chargingFullProcess = digitalRead(chargingFullPin);
        playModeProcess = digitalRead(playModePin);

        if (chargingStateProcess == LOW && chargingFullProcess == HIGH && playModeProcess == HIGH) {
          for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
              BlueLeds[i][j] = 0;
              RedLeds[i][j] = 0;
              GreenLeds[i][j] = 0;
            }
          }
          IsCharging();
          imageZustand = 0;
        }
        else if (chargingStateProcess == HIGH && chargingFullProcess == LOW && playModeProcess == HIGH) {
          for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
              BlueLeds[i][j] = 0;
              RedLeds[i][j] = 0;
              GreenLeds[i][j] = 0;
            }
          }
          imageZustand = 1;
        }
        break;
    }
    count++;

    if (column == 9) {
      column = 1; // we only have 8 columns, so we start with layer 1 again if column counter goes to 9
    }
  }
}
