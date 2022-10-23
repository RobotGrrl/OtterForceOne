#include <Streaming.h>
#include <Wire.h>
#include "MS5837.h"
#include "rgb_lcd.h"
#include <ESP32Servo.h>
#include "Board.h"

MS5837 sensor;
rgb_lcd lcd;
Servo suction;

bool ENABLED = false;
bool DIR = true; // true = fwd, false = rev


#define SLOW_DOWN 100


#define THRUSTER_OFF 1500       // us
#define THRUSTER_FWD_MIN 1600   // us
#define THRUSTER_FWD_MAX 2000   // us
#define THRUSTER_REV_MIN 1400   // us
#define THRUSTER_REV_MAX 1000   // us


#define PWM1_Ch    0
#define PWM2_Ch    7
#define PWM_Res    8
#define PWM_Freq   1000



long last_esc_write = 0;
long last_auton_press = 0;
long last_suction_press = 0;
long last_auton_release = 0;
long last_suction_release = 0;
long last_lcd_update = 0;


void setup() {
  
  Serial.begin(9600);
  Serial.println("Starting");


  pinMode(HEARTBEAT_LED, OUTPUT);
  pinMode(HEADLIGHT_L_LED, OUTPUT);
  pinMode(HEADLIGHT_R_LED, OUTPUT);
  pinMode(AUTON_BUTTON, INPUT);
  pinMode(SUCTION_BUTTON, INPUT);


  
  Wire.begin();
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.clear();

  
  lcd.print("otter force one");
  lcd.setCursor(5, 1);
  lcd.print("init");
  lcd.setPWM(BLUE, 255);
  

  // Initialize pressure sensor
  // Returns true if initialization was successful
  // We can't continue with the rest of the program unless we can initialize the sensor
  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }
  
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)



  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  suction.setPeriodHertz(50); // standard 50hz servo
  suction.attach(SUCTION_PWM, 1000, 2000);


  lcd.clear();
  lcd.print("otter force one");
  lcd.setCursor(0, 1);
  lcd.print("arming suction");
  lcd.setPWM(BLUE, 255);

  // arm
  delay(5000);
  Serial << "arming suction" << endl;
  suction.writeMicroseconds(THRUSTER_OFF);
  delay(2000);
  suction.writeMicroseconds(THRUSTER_REV_MIN);
  delay(1000);
  suction.writeMicroseconds(THRUSTER_FWD_MIN);
  delay(1000);
  suction.writeMicroseconds(THRUSTER_OFF);
  delay(2000);
  last_esc_write = millis();



  Serial << "Otter Force One READY" << endl;

}

void loop() {


  // -------------
  sensorUpdate();

  if(digitalRead(AUTON_BUTTON) == HIGH && millis()-last_auton_press >= 1000) {
    Serial << "auton pressed!111!1" << endl;
    ENABLED = true;
    last_auton_press = millis();
  }

  if(digitalRead(SUCTION_BUTTON) == HIGH && millis()-last_suction_press >= 1000) {
    Serial << "suction pressed!111!1" << endl;
    DIR = true;
    last_suction_press = millis();
  }

  if(digitalRead(AUTON_BUTTON) == LOW && millis()-last_auton_release >= 1000) {
    ENABLED = false;
    last_auton_release = millis();
  }

  if(digitalRead(SUCTION_BUTTON) == LOW && millis()-last_suction_release >= 1000) {
    DIR = false;
    last_suction_release = millis();
  }


  // ---------------

  if(millis()-last_lcd_update >= 500) {

    lcd.clear();
    lcd.setCursor(0,0);

    if(ENABLED) {
      lcd.print("[x]");
      lcd.setPWM(GREEN, 255);
      digitalWrite(HEADLIGHT_L_LED, HIGH);
      digitalWrite(HEADLIGHT_R_LED, HIGH);
    } else {
      lcd.print("[ ]");
      lcd.setPWM(RED, 255);
      digitalWrite(HEADLIGHT_L_LED, LOW);
      digitalWrite(HEADLIGHT_R_LED, LOW);
    }
  
    if(DIR) {
      lcd.print("    FWD");
    } else {
      lcd.print("    REV");
    }

    lcd.setCursor(1,1);
    lcd.print(sensor.depth());
    lcd.print("m  ");
    lcd.print(sensor.temperature());
    lcd.print("C");
  
  
    last_lcd_update = millis();

  }


  // -------------


  if(ENABLED) {
    if(DIR) {
      if(millis()-last_esc_write >= 100) {
        suction.writeMicroseconds(THRUSTER_FWD_MAX-SLOW_DOWN);
        last_esc_write = millis();
      }
    } else {
      if(millis()-last_esc_write >= 100) {
        suction.writeMicroseconds(THRUSTER_REV_MAX+SLOW_DOWN);
        last_esc_write = millis();
      }
    }
  } else {
    if(millis()-last_esc_write >= 100) {
      suction.writeMicroseconds(THRUSTER_OFF);
      last_esc_write = millis();
    }
  }


  // ----------------------------

  
  uint16_t slice = millis() % 1000;
  digitalWrite(HEARTBEAT_LED, slice < 100 || (slice > 200 && slice < 300));

  
}



void sensorUpdate() {
  // Update pressure and temperature readings
  sensor.read();

  /*
  Serial.print("Pressure: "); 
  Serial.print(sensor.pressure()); 
  Serial.println(" mbar");
  
  Serial.print("Temperature: "); 
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  
  Serial.print("Depth: "); 
  Serial.print(sensor.depth()); 
  Serial.println(" m");
  
  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude()); 
  Serial.println(" m above mean sea level");
  */

}
