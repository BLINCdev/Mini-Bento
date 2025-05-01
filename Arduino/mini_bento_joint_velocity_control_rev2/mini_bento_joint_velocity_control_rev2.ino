// Mini Bento Arm / PS2 Thumb Joystick - Joint Velocity Controller
// Version 0.1
// Author: Michael (Rory) Dawson
//
// Objective: This sketch provides sequential joint velocity control of Mini Bento Arm using a PS2 Thumb Joystick. 
// The vertical axis on the thumb joystick is used to proportionally control the velocity of the selected joint
// and the tactile button when you push down the thumb joystick is used to sequentially switch between the joints.
// The QTY:4 XL330-M288-T are pre-configured in the Dynamixel Wizard 2.0 software to be in position mode with CW/CCW joint
// limits as defined in the setup() section. This example code has been tested and verified to work with the OpenRB-150.
// 
// Definitions:
// ID1: shoulder rotate
// ID2: elbow flex
// ID3: wrist rotate (not available on the Mini Bento Arm)
// ID4: wrist flex
// ID5: hand open/close
//
// PS2 Thumb Joystick Pinout:
// GND: Connect to GND on OpenRB-150
// +5V: Connect to 3.3V (VCC) pin on OpenRB-150
// VRX: not connected in this sketch
// VRY: Connect to A0 pin on OpenRB-150
// SW: Connect to digital pin 5 on OpenRB-150
//
// Dynamixel Connections:
// Connect XL330-M288-T or similar to TTL port on OpenRB-150
// Connect 5V/4A PSU to power port OpenRB-150
//
// References:
// Structs: http://playground.arduino.cc/Code/Struct
// OpenRB-150: https://emanual.robotis.com/docs/en/parts/controller/openrb-150/

#include <DynamixelShield.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560)
  #include <SoftwareSerial.h>
  SoftwareSerial soft_serial(7, 8); // DYNAMIXELShield UART RX/TX
  #define DEBUG_SERIAL soft_serial
#elif defined(ARDUINO_SAM_DUE) || defined(ARDUINO_SAM_ZERO)
  #define DEBUG_SERIAL SerialUSB    
#else
  #define DEBUG_SERIAL Serial
#endif

const float DXL_PROTOCOL_VERSION = 2.0;
const int BENTO_NUM = 5;

DynamixelShield dxl;

//This namespace is required to use Control table item names
using namespace ControlTableItem;

// Define global array of structs to hold the parameters for each digit
struct motorParam {
  int enabled;             // 0 = disabled (not connected), 1 = enabled (connected)
  int CW_limit;            // the CW position limit for the servo (varies from 0-4095)
  int CCW_limit;           // the CCW position limit for the servo (varies from 0-4095)
  int min_vel;             // the minimum velocity (varies from 0-1023)
  int max_vel;             // the maximum velocity (varies from 0-1023)
  int state;               // 0 = stopped, 1 = moving CW, 2 = moving CCW
  int pos;                 // the current target position
  int pos_;                // the present position (feedback from servo)
  int vel;                 // the current target velocity
  int vel_;                // the present velocity (feedback from servo)
} motor[BENTO_NUM+1];

// Assign joystick axis and buttons to pins on the arduino
// Also connect the joystick VCC to Arduino 5V, and joystick GND to Arduino GND.

const int VERT = A0; // analog
const int HORIZ = A1; // analog
const int SEL =  5; // digital
int prev_state = 1;
int servoID = 1;
int chA_deadband = 20;  
int chA_neutral = 512;
int chB_deadband = 20; 
int chB_neutral = 512;

// This sketch outputs serial data at 115200 baud (open Serial Monitor to view).

void setup()
{
  // make the SEL line an input
  pinMode(SEL,INPUT_PULLUP);
  
  // Define shoulder rotate paramaters (X-series)
  motor[1].enabled = 0;       // disabled by default unless detected via pin command
  motor[1].CW_limit = 1500;       // the CW position limit for the servo
  motor[1].CCW_limit = 3073;   // the CCW position limit for the servo
  motor[1].min_vel = 1;           // the minimum velocity
  motor[1].max_vel = 50;         // the maximum velocity
  motor[1].pos = 2047;            // the current target position
  motor[1].vel = 10;           // the current target velocity

  // Define elbow extension paramaters (X-series)
  motor[2].enabled = 0;       // disabled by default unless detected via pin command
  motor[2].CW_limit = 1730;       // the CW position limit for the servo
  motor[2].CCW_limit = 2700;   // the CCW position limit for the servo
  motor[2].min_vel = 1;           // the minimum velocity
  motor[2].max_vel = 50;         // the maximum velocity
  motor[2].pos = 2047;            // the current target position
  motor[2].vel = 10;           // the current target velocity

  // Define wrist rotation paramaters (X-series)
  motor[3].enabled = 0;       // disabled by default unless detected via pin command
  motor[3].CW_limit = 1028;       // the CW position limit for the servo
  motor[3].CCW_limit = 3073;   // the CCW position limit for the servo
  motor[3].min_vel = 1;           // the minimum velocity
  motor[3].max_vel = 60;         // the maximum velocity
  motor[3].pos = 2047;            // the current target position
  motor[3].vel = 10;           // the current target velocity

  // Define wrist flexion paramaters (X-series)
  motor[4].enabled = 0;       // disabled by default unless detected via pin command
  motor[4].CW_limit = 848;       // the CW position limit for the servo
  motor[4].CCW_limit = 3248;   // the CCW position limit for the servo
  motor[4].min_vel = 1;           // the minimum velocity
  motor[4].max_vel = 60;         // the maximum velocity
  motor[4].pos = 2047;            // the current target position
  motor[4].vel = 10;           // the current target velocity

  // Define hand close/open paramaters (X-series)
  motor[5].enabled = 0;       // disabled by default unless detected via pin command
  motor[5].CW_limit = 1650;       // the CW position limit for the servo
  motor[5].CCW_limit = 2658;   // the CCW position limit for the servo
  motor[5].min_vel = 1;           // the minimum velocity
  motor[5].max_vel = 60;         // the maximum velocity
  motor[5].pos = 2047;            // the current target position
  motor[5].vel = 10;           // the current target velocity

 
  // For Uno, Nano, Mini, and Mega, use UART port of DYNAMIXEL Shield to debug.
  DEBUG_SERIAL.begin(115200);

  // Set Port baudrate to 1000000 bps. This has to match with DYNAMIXEL baudrate.
  dxl.begin(1000000);
  
  // Set Port Protocol Version. This has to match with DYNAMIXEL protocol version.
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);


  // Get DYNAMIXEL information and initialize the connected servos
  delay(100);  // Add delay so there is time for the bus to initialize before starting to ping servos
  for(int i=1; i<=BENTO_NUM; i++)
  {
    motor[i].enabled = dxl.ping(i);
    dxl.torqueOff(i);   // Turn off torque when configuring items in EEPROM area
    dxl.setOperatingMode(i, OP_POSITION);  // Set operating mode to position control mode
    dxl.torqueOn(i);    // Turn the torque back on
    // Read the latest position and velocity values from the servo
    motor[i].pos_ = dxl.getPresentPosition(i);
    motor[i].vel_ = dxl.getPresentVelocity(i);
    StopVelocity(i);    // Set the initial positions equal to the current positions to ensure each servo is stopped
  }
  dxl.ledOn(servoID);   // Set the LED of the initially selected servo in the sequential switching list
}

void loop() 
{
  // read values from the external potentiometer
  int potValue = analogRead(VERT); // will be 0-1023
  int select = digitalRead(SEL); // will be HIGH (1) if not pressed, and LOW (0) if pressed

  // Read the latest position and velocity values from the servo
  motor[servoID].pos_ = dxl.getPresentPosition(servoID);
  motor[servoID].vel_ = dxl.getPresentVelocity(servoID);

  // Check for a switching event. If detected then switch to the next item in the sequential
  // switching list.
  if (select == LOW && prev_state == HIGH)
  {
    // If the active servo is moving then stop it
    if (motor[servoID].state != 0)
    {
      StopVelocity(servoID);
    }
    dxl.ledOff(servoID);  // turn off the LED of the old active joint
    servoID = servoID + 1;
    while (motor[servoID].enabled == 0) // Skip servos that are not connected
    {
      servoID = servoID + 1;
    }

    // Reset back to the first servo when exceed the number of servos in the arm
    if (servoID > BENTO_NUM)
    {
     servoID = 1;
    }
    dxl.ledOn(servoID);   // turn on the LED of the new active joint 
  }
  
  // Update the switch state
  prev_state = select;

  // Map the value of the external potentiometer to the goal position and velocity of the servo.
  // If outside of deadband and above neutral then move CCW
  if (potValue < chA_neutral - chA_deadband)
  {
    motor[servoID].pos = motor[servoID].CCW_limit;
    motor[servoID].vel = map(potValue, chA_neutral - chA_deadband, 0, motor[servoID].min_vel, motor[servoID].max_vel);
    dxl.setGoalPosition(servoID, motor[servoID].pos);
    dxl.writeControlTableItem(PROFILE_VELOCITY, servoID, motor[servoID].vel);
    motor[servoID].state = 2;  
  }
  // If outside of deadband and below neutral then move CW
  else if (potValue >= chA_neutral + chA_deadband)
  {
    motor[servoID].pos = motor[servoID].CW_limit;
    motor[servoID].vel = map(potValue-(chA_neutral+chA_deadband), 0, chA_neutral - chA_deadband, motor[servoID].min_vel, motor[servoID].max_vel);
    //motor[servoID].vel = 50;
    dxl.setGoalPosition(servoID, motor[servoID].pos);
    dxl.writeControlTableItem(PROFILE_VELOCITY, servoID, motor[servoID].vel);
    motor[servoID].state = 1;  
  }
  // Stop the servo if it is inside the deadband range and was previously moving. 
  // You should only stop the servo this way once otherwise it can cause the servo to sag down from gravity if done repeatedly
  else if (motor[servoID].state > 0)
  {
    StopVelocity(servoID);
    motor[servoID].state = 0;     
  }
 
  // Output sensor and motor feedback to the serial monitor
  DEBUG_SERIAL.print("Potentiometer Value: ");
  DEBUG_SERIAL.print(potValue);
  DEBUG_SERIAL.print(" Select Value: ");
  if(select == HIGH)
    DEBUG_SERIAL.print("not pressed");
  else
    DEBUG_SERIAL.print("PRESSED!");
  DEBUG_SERIAL.print(", servoID: ");
  DEBUG_SERIAL.print(servoID);
  DEBUG_SERIAL.print(", Present Position: ");
  DEBUG_SERIAL.print(motor[servoID].pos_);
  DEBUG_SERIAL.print(", Target Velocity: ");
  DEBUG_SERIAL.print(motor[servoID].vel);
  DEBUG_SERIAL.print(", Present Velocity: ");
  DEBUG_SERIAL.println(motor[servoID].vel_);
}  

// Stop the active servo by setting its target position equal to its current position
void StopVelocity(int j)
{
  // Offset by the value of the velocity to avoid jump back from feedback delay
  if (motor[j].pos == motor[j].CW_limit)
  {
    motor[j].pos = motor[j].pos_ + motor[j].vel_;
  }
  else if (motor[j].pos == motor[j].CCW_limit)
  {
    motor[j].pos = motor[j].pos_ + motor[j].vel_;
  }
  else
  {
    motor[j].pos = motor[j].pos_; // Set target position equal to the present position
  }
  motor[j].vel = motor[j].max_vel;
  dxl.setGoalPosition(j, motor[j].pos);
  dxl.writeControlTableItem(PROFILE_VELOCITY, j, motor[j].vel);
}
