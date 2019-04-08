#include <VirtualWire.h>

//#define DEBUG // For debuggin purposes, comment when uploading to the real product

// Pin used to connect the arduino to the RF transmitter
const int recv_pin = 11;

// The struct that will be transmitted (the ID of the action)
typedef struct
{
  int action;
} Package;

bool state1, state2, state3, state4; // The state of the pins, so they can be toggled
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 2200;  // How many ms to wait before registering again

void setup()
{
#ifdef DEBUG
  Serial.begin(9600); // Debugging only
  Serial.println("Setting up receiver..");
#endif

  // Set the pins as OUTPUT and init the states
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  state1 = true;
  state2 = true;
  state3 = true;
  state4 = true;
  digitalWrite(2, state1);
  digitalWrite(3, state1);
  digitalWrite(4, state1);
  digitalWrite(5, state1);
  // Initialise the IO and ISR
  vw_setup(500); // Bits per sec
  vw_rx_start(); // Start the receiver PLL running
#ifdef DEBUG
  Serial.println("Up and running !");
#endif
}

void loop()
{
  // The recipient for the message
  Package message;
  byte taille_message = sizeof(Package);

  if (vw_get_message((byte *)&message, &taille_message)) // Non-blocking
  {

    // Debounce
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
#ifdef DEBUG
      Serial.print("Received : ");
      Serial.println(message.action);
#endif
      lastDebounceTime = millis();

      // Toggle
      switch (message.action)
      {
      case 1:
        state1 = !state1;
        digitalWrite(2, state1);
        break;

      case 2:
        state2 = !state2;
        digitalWrite(3, state2);
        break;

      case 3:
        state3 = !state3;
        digitalWrite(4, state3);
        break;

      case 4:
        state4 = !state4;
        digitalWrite(5, state4);
        break;
      }
      digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
      delay(50);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    }
    else
    {
      digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
      delay(50);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
