/*
  ---- basic communication example ----
  Control Arduino board functions with the following messages:
   r -> read digital pins
   w [pin] [value] -> write pin
   m [pin] [value] -> set pin mode
*/

#include <Messenger.h>
#include <PrintEx.h>
using namespace ios;

StreamEx stream = Serial;

Messenger message = Messenger();

void messageCompleted(void)
{
  //  const char *str;
  //  message.copyString(*str, 5);
  //  stream << "copyString=" << str << endl;

  char c = message.readChar();
  int pin, state;
  //stream << "char=" << c << endl;
  switch (c)
  {
    case 'r':
    case 'R':
      stream << F("read: ") << endl;
      for (char i = 2; i < 14; i++) // Read pins 2 to 13
        stream.printf("\tPin:%i=%i\n", i, digitalRead(i));
      break;

    case 'w':
    case 'W':
      pin = message.readInt(), state = message.readInt();
      stream << F("write: Pin") << pin << F(": State:") << state << endl;
      if (pin > 1 && pin < 20)
        digitalWrite(pin, state);
      else
        stream << F("Invalid Pin: ") << pin << endl;
      break;

    case 'm':
    case 'M':
      stream << F("pinMode: ");
      pin = message.readInt(), state = message.readInt();// == 1 ? OUTPUT : INPUT;
      switch (state)
      {
        case 0:
          pinMode(pin, INPUT);
          stream << F("Pin") << pin << F(" mode=INPUT") << endl;
          break;

        case 1:
          pinMode(pin, OUTPUT);
          stream << F("Pin") << pin << F(" mode=OUTPUT") << endl;
          break;

        case 2:
          pinMode(pin, INPUT_PULLUP);
          stream << F("Pin") << pin << F(" mode=INPUT_PULLUP") << endl;
          break;

        default:
          stream << F("Invalid Mode: ") << state << endl;
          break;
      }
      //stream << F("pinMode: Pin") << pin << F(": Mode:") << state << endl;
      //if (pin > 1 && pin < 20)
      // pinMode(pin, state);
      //else
      break;

    case 'a':
    case 'A':
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
      stream << F("Enabled all output pins.") << endl;
      break;

    case 'L':
    case 'l':
      for (char i = 2; i < 14; i++) // Read pins 2 to 13
        if (i != 7) digitalWrite(i, LOW);
      stream << F("Disabled all output pins.") << endl;
      break;

    default:
      stream << F("Invalid Entry: ") << c << endl;
      stream << F("Usage:\n\tr -> read digital pins\n\tw [pin] [1/0]->write pin\n\tm [pin] [0/1/2]->set pin mode") << endl;
      break;
  }
  stream << endl;
}

void setup(void)
{
  Serial.begin(9600);
  message.attach(messageCompleted);
  message.process('a');
  message.process(13);
}

void loop(void)
{
  while (Serial.available())
    message.process(Serial.read());
}




