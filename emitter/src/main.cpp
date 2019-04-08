#include "Arduino.h" // Default lib
#include "MPU6050_tockn.h" // For the MPU
#include <Wire.h> // Also for the MPU
#include <VirtualWire.h> // For the RF 433MHz

//#define DEBUG ; // Use for Serial verbose Debugging (comment for silent/real use)

MPU6050 mpu6050(Wire);

float roll, pitch, yaw;
const int LedPin = 7;
const int transmitPin = 12;
int led;


// Set the color for the Status LED
// 0 is off
// 1 is up and running
void setLedColor(int led_status) {
  digitalWrite(LedPin, led_status);
}

// A struct that will be sent over RF433
typedef struct {
  int action;
} Package;

// Routine called to send the package via RF433
void sendAction(int val) {
  Package mypackage;
  mypackage.action = val;
  vw_send((byte *)&mypackage, sizeof(mypackage));
  vw_wait_tx();
}

// Setup of all the various systems
void setup() {
#ifdef DEBUG
  // Set Serial for debugging
  Serial.begin(38400);
#endif

  // Set MPU
  vw_set_tx_pin(transmitPin);
  vw_setup(500);       // Bits per sec
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(false);

  // Set RF 433 & Status LED
  pinMode(transmitPin, OUTPUT); // 433MHz
  pinMode(LedPin, OUTPUT); // Led

  //Once all is initialized, light up the LED
  setLedColor(1);
}

// The actual main(), that recursively checks for patterns and send
// the command to the receiver
void loop() {
  delay(80);
  mpu6050.update();

  // Yaw, pitch and roll (how is the sensor in space ?)
  pitch = mpu6050.getAngleX();
  roll = mpu6050.getAngleY();
  yaw = mpu6050.getAngleZ();

#ifdef DEBUG
  Serial.print("\tPitch: "); Serial.print(pitch);
  Serial.print("\tYaw: "); Serial.print(yaw);
  Serial.print("\tRoll: "); Serial.println(roll);
#endif
  // Check for position
  // Wand straight up
  if (pitch > 75) {
    setLedColor(0);
    sendAction(1);
  }
  // Wand straight down
  else if (pitch < -75) {
    setLedColor(0);
    sendAction(2);
  }
  // Wand swung left
  else if (yaw < -60) {
    setLedColor(0);
    sendAction(3);
  }
  // Wand swung right
  else if (yaw > 60) {
    setLedColor(0);
    sendAction(4);
  }
  else{setLedColor(1);}
}
