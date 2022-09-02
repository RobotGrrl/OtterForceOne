/*
 * Otter Force One - Thruster Test
 * ----------------------------------
 * Test the thrusters to see they are 
 * functioning to expectations. 
 * ... and that they're pointing the right
 * direction. :P
 * 
 */

#include <ESP32Servo.h>
#include <Streaming.h>
#include "Board.h"

#define THRUSTER_OFF 1500 // us
#define THRUSTER_REV_MAX 1100 // us
#define THRUSTER_REV_MIN 1499 // us
#define THRUSTER_FWD_MIN 1501 // us
#define THRUSTER_FWD_MAX 1900 // us

#define PRINT_DEL 500

#define PWM1_Ch    0
#define PWM2_Ch    7
#define PWM_Res    8
#define PWM_Freq   1000

Servo suction;
Servo waterjet;

enum all_mode {
  IDLE_MODE,
  KNOB_OFF,
  KNOB_MODE1,
  KNOB_MODE2,
  KNOB_MODE3,
  KNOB_MODE4
};

uint8_t the_mode = IDLE_MODE;

long last_print = 0;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600); // Pi

  pinMode(USER_BUTTON, INPUT);
  pinMode(WAKE_SWITCH, INPUT);
  pinMode(SUCTION_SWITCH, INPUT);
  pinMode(JET_SWITCH, INPUT);
  pinMode(BATT_LEVEL, INPUT);
  pinMode(KNOB_PIN, INPUT);
  pinMode(PRESSURE_SENSOR, INPUT);
  pinMode(NEO_PIN, OUTPUT);
  pinMode(HEARTBEAT_LED, OUTPUT);
  pinMode(HB_LED_LEFT, OUTPUT);
  pinMode(SUCTION_PWM, OUTPUT);
  pinMode(JET_PWM, OUTPUT);
  pinMode(HB_LED_RIGHT, OUTPUT);
  pinMode(COMMS_LED, OUTPUT);

  pwmInit();

  // allocate timers
  ESP32PWM::allocateTimer(0);
  suction.setPeriodHertz(50); // standard 50hz servo
  waterjet.setPeriodHertz(50); // standard 50hz servo
  suction.attach(SUCTION_PWM, THRUSTER_REV_MAX, THRUSTER_FWD_MAX);
  waterjet.attach(JET_PWM, THRUSTER_REV_MAX, THRUSTER_FWD_MAX);
  
  Serial << "Otter Force One - all systems go" << endl;
}

void loop() {

  switch(the_mode) {
    case IDLE_MODE:
      if(millis()-last_print >= PRINT_DEL) {
        uint16_t knob_val = analogRead(KNOB_PIN);
        Serial << "Knob: " << knob_val << endl;
        last_print = millis();
      }
    break;
    case KNOB_MODE1: {
      
      uint16_t knob_val = analogRead(KNOB_PIN);
      if(knob_val < 1070) knob_val = 1070;
      uint16_t output_val = map(knob_val, 1070, 4096, THRUSTER_FWD_MIN, THRUSTER_FWD_MAX);
      float percentage = map(output_val, THRUSTER_FWD_MIN, THRUSTER_FWD_MAX, 0, 100);
      
      suction.writeMicroseconds(output_val);

      if(millis()-last_print >= PRINT_DEL) {
        Serial << "Suction Fwd: " << output_val << " " << percentage << "% knob: " << knob_val << endl;
        last_print = millis();
      }
      
    }
    break;
    case KNOB_MODE2: {

      uint16_t knob_val = analogRead(KNOB_PIN);
      if(knob_val < 1070) knob_val = 1070;
      uint16_t output_val = map(knob_val, 1070, 4096, THRUSTER_REV_MIN, THRUSTER_REV_MAX);
      float percentage = map(output_val, THRUSTER_REV_MIN, THRUSTER_REV_MAX, 0, 100);
      
      suction.writeMicroseconds(output_val);

      if(millis()-last_print >= PRINT_DEL) {
        Serial << "Suction Rev: " << output_val << " " << percentage << "% knob: " << knob_val << endl;
        last_print = millis();
      }
      
    }
    break;
    case KNOB_MODE3: {
      
      uint16_t knob_val = analogRead(KNOB_PIN);
      if(knob_val < 1070) knob_val = 1070;
      uint16_t output_val = map(knob_val, 1070, 4096, THRUSTER_FWD_MIN, THRUSTER_FWD_MAX);
      float percentage = map(output_val, THRUSTER_FWD_MIN, THRUSTER_FWD_MAX, 0, 100);
      
      waterjet.writeMicroseconds(output_val);

      if(millis()-last_print >= PRINT_DEL) {
        Serial << "Waterjet Fwd: " << output_val << " " << percentage << "% knob: " << knob_val << endl;
        last_print = millis();
      }
      
    }
    break;
    case KNOB_MODE4: {

      uint16_t knob_val = analogRead(KNOB_PIN);
      if(knob_val < 1070) knob_val = 1070;
      uint16_t output_val = map(knob_val, 1070, 4096, THRUSTER_REV_MIN, THRUSTER_REV_MAX);
      float percentage = map(output_val, THRUSTER_REV_MIN, THRUSTER_REV_MAX, 0, 100);
      
      waterjet.writeMicroseconds(output_val);

      if(millis()-last_print >= PRINT_DEL) {
        Serial << "Waterjet Rev: " << output_val << " " << percentage << "% knob: " << knob_val << endl;
        last_print = millis();
      }
      
    }
    break;
  }


  if(Serial.available() > 0) {
    char c = Serial.read();
    switch(c) {
      case 'h':
        Serial << "Otter Force One - Command Line Interface" << endl;
        Serial << "h: Help" << endl;
        Serial << "1: knob off" << endl;
        Serial << "2: knob: suction fwd" << endl;
        Serial << "3: knob: suction rev" << endl;
        Serial << "4: knob: water jet fwd" << endl;
        Serial << "5: knob: water jet rev" << endl;
        Serial << "p: print knob" << endl;
      break;
      case '1':
        Serial << "knob: off" << endl;
        the_mode = KNOB_OFF;
      break;
      case '2':
        Serial << "knob: suction fwd" << endl;
        the_mode = KNOB_MODE1;
      break;
      case '3':
        Serial << "knob: suction rev" << endl;
        the_mode = KNOB_MODE2;
      break;
      case '4':
        Serial << "knob: water jet fwd" << endl;
        the_mode = KNOB_MODE3;
      break;
      case '5':
        Serial << "knob: water jet rev" << endl;
        the_mode = KNOB_MODE4;
      break;
      case 'p':
        if(the_mode != IDLE_MODE) {
          Serial << "start printing" << endl;
          the_mode = IDLE_MODE;
        } else {
          Serial << "stop printing" << endl;
          the_mode = KNOB_OFF;
        }
      break;
    }
  }

}


void pwmInit() {
  
  // there are 16 channels to control the PWM duty 
  // cycle independently. The frequency has to be 
  // shared between each couple of channels routed
  // to the same timer.
  // ch0 & ch1 share timer0
  // ch2 & ch3 share timer1
  // ch4 & ch5 share timer2
  // ch6 & ch7 share timer3
  
//  ledcAttachPin(SUCTION_PWM, 0);   // pin, channel
//  ledcSetup(0, PWM_Freq, PWM_Res); // channel, frequency, resolution
//
//  ledcAttachPin(JET_PWM, 7);       // pin, channel
//  ledcSetup(7, PWM_Freq, PWM_Res); // channel, frequency, resolution

  ledcAttachPin(HB_LED_LEFT, 4);   // pin, channel
  ledcSetup(4, PWM_Freq, PWM_Res); // channel, frequency, resolution

  ledcAttachPin(HB_LED_RIGHT, 5);  // pin, channel
  ledcSetup(5, PWM_Freq, PWM_Res); // channel, frequency, resolution

}


// EK's notes:
// ------------
// what is the max and min for writeMicroseconds?
// it is this:
// // Default Arduino Servo.h
// #define DEFAULT_uS_LOW 544
// #define DEFAULT_uS_HIGH 2400
// in ESP32Servo.h
// so yes, it is within the range
// do the default values have to change?
// guess so?
// commented out and added for these thrusters:
// // ROV thrusters
// #define DEFAULT_uS_LOW 1100
// #define DEFAULT_uS_HIGH 1900
// nevermind, looks like that wasn't necessary as
// the min and max are set in the attach() function
