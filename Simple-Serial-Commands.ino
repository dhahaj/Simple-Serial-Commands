/*
  ---- basic communication example ----
  Control Arduino board functions with the following messages:
   r -> read digital pins
   w [pin] [value] -> write pin
   m [pin] [value] -> set pin mode
*/
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "Messenger.h"
#include <PrintEx.h>

#define I2C_ADDR 0x3F
#define dw(x,v) digitalWrite((x),(v))
#define ALL_LOW for(int i = 2;i<14;i++) { if(i!=7) digitalWrite(i, LOW); }
#define DELAY 2000

using namespace ios;
StreamEx stream = Serial;

Messenger message = Messenger();
boolean testing = false;

// LiquidCrystal_PCF8574 lcd(I2C_ADDR);

void messageCompleted(void)
{
  char c = message.readChar();

  // Ensure we are not testing or attempting to
  // start another test.
  if ( testing && (c == 'e' || c == 'E')) {
    ALL_LOW
    stream << F("Testing finished.") << endl;
    testing = false;
    return;
  }
  else if (testing)// || c == 't' || c == 'T')
    return;

  int pin, state, val;
  String msg = "";

  switch (c)
  {
    case 'r':
    case 'R':
      // lcd.home(); lcd.clear();
      // lcd.print("Reading Pins");
      stream << F("read: ") << endl;
      for (char i = 2; i < 14; i++) {// Read pins 2 to 13
        val = digitalRead(i);
        stream.printf("\tPin:%i=%i\n", i, val);
        // lcd.print("i=" + String(val) + ",");
      }
      break;

    case 'w':
    case 'W':
      // lcd.home(); lcd.clear();
      pin = message.readInt(), state = message.readInt();
      stream << F("write: Pin") << pin << F(": State:") << state << endl;
      if (pin > 1 && pin < 20) {
        digitalWrite(pin, state);
        // lcd.print("Write pin: " + pin);
      }
      else {
        stream << F("Invalid Pin: ") << pin << endl;
        // lcd.print("Invalid Pin: " + pin);
      }
      break;

    case 'm':
    case 'M':
      // lcd.home(); lcd.clear();
      stream << F("pinMode: ");
      pin = message.readInt(), state = message.readInt();// == 1 ? OUTPUT : INPUT;
      msg = "PinMode: " + pin;
      switch (state)
      {
        case 0:
          pinMode(pin, INPUT);
          stream << F("Pin") << pin << F(" mode=INPUT") << endl;
          msg += " INPUT";
          break;

        case 1:
          pinMode(pin, OUTPUT);
          stream << F("Pin") << pin << F(" mode=OUTPUT") << endl;
          msg += " OUTPUT";
          break;

        case 2:
          pinMode(pin, INPUT_PULLUP);
          stream << F("Pin") << pin << F(" mode=INPUT_PULLUP") << endl;
          msg += " PULLUP";
          break;

        default:
          stream << F("Invalid Mode: ") << state << endl;
          msg = "Invalid Mode.";
          break;
      }
      // lcd.print(msg);
      break;

    case 'a':
    case 'A':
      // lcd.home(); lcd.clear();
      for (char i = 2; i < 14; i++) // Read pins 2 to 13
      {
        if (i != 7) pinMode(i, OUTPUT);
        else pinMode(i, INPUT);
      }
      stream << F("Set all pins to default modes.") << endl;
      break;

    case 'H':
    case 'h':
      for (char i = 2; i < 14; i++) // Read pins 2 to 13
        if (i != 7) digitalWrite(i, HIGH);
      // lcd.home(); lcd.clear();
      stream << F("Enabled all output pins.") << endl;
      // lcd.print(F("Enable all outputs"));
      break;

    case 'L':
    case 'l':
      ALL_LOW
      // lcd.home(); lcd.clear();
      //      for (char i = 2; i < 14; i++) // Read pins 2 to 13
      //        if (i != 7) digitalWrite(i, LOW);
      stream << F("Disabled all output pins.") << endl;
      // lcd.print(F("Disable all outputs"));
      break;

    case 't':
    case 'T':
      stream << F("Starting test.") << endl;
      testing = true;
      runTest();
      break;

    //    case 'e':
    //    case 'E':
    //      ALL_LOW
    //      testing = false;
    //      stream << F("Testing finished.") << endl;
    //      break;

    default:
      // lcd.home(); lcd.clear();
      // lcd.print(F("Invalid argument:"));
      // lcd.setCursor(1 , 0);
      // lcd.print("  " + c);
      stream << F("Invalid Entry: ") << c << endl;
      stream << F("Usage:\n\tr -> read digital pins\n\tw [pin] [1/0]->write pin\n\tm [pin] [0/1/2]->set pin mode") << endl;
      break;
  }
  stream << endl;
}

void setup(void)
{
  Serial.begin(9600);

  // lcd.begin(16, 2);
  // lcd.setBacklight(225);
  // lcd.home(); lcd.clear();
  // lcd.print(F("Welcome!"));
  //  delay(1000);
  // lcd.home(); lcd.clear();

  message.attach(messageCompleted);
  message.process('a');
  message.process(13);
  delay(10);
}

void loop(void)
{
  while (Serial.available())
    message.process(Serial.read());
}

void runTest() {
  ALL_LOW
  dw(3, 1);
  dw(13, 1);
  delay(10);
  stream << F("Test starting.") << endl;
  delay(DELAY);
  stream << F("Battery power on.") << endl;
  dw(12, 1);
  delay(DELAY + 1000);
  stream << F("Starting Low Battery test.") << endl;
  dw(2, 1);
  delay(DELAY + 1400);
  stream << F("Ending Low Battery test.") << endl;
  dw(2, 0);
  delay(100);
  stream << F("Waiting for user completion.") << endl;
}

