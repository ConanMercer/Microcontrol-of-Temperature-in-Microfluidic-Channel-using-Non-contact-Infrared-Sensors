//---------------------------------------------------------------
// --- Code written by Conan Mercer 2018. National University of Ireland Galway
// --- For microcontrol of temperature in microfluidic channel using non-contact 
// --- infrared sensors (MLX90614xDCI)
//---------------------------------------------------------------

//---------------------------------------------------------------
// --- Libraries
//---------------------------------------------------------------
#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>

//---------------------------------------------------------------
// --- Variables and settings
//---------------------------------------------------------------

Adafruit_MLX90614 IR_1 = Adafruit_MLX90614(0x5A); // Initialise two IR sensors
Adafruit_MLX90614 IR_2 = Adafruit_MLX90614(0x3B); // mapped to two different addresses
const int NiChrome = 3; // Nichrome connected to MOSFET gate
bool T1Enable = false;
bool T2Enable = false;
double TEMP; 
int Tset = 37;  // Temperature set point
double aggKp = 4, aggKi = 0.4, aggKd = 8;     // PID values
double consKp = 6, consKi = 0.1, consKd = 8;
double Setpoint, Input, Output;  
PID myPID(&Input, &Output, &Setpoint, aggKp, aggKi, aggKd, DIRECT); 
const int sampleRate = 1; 
const long serialPing = 500; 
unsigned long now = 0; 
unsigned long lastMessage = 0; 
void setup() {
  IR_1.begin(); // Initialise two IR sensors
  IR_2.begin();
  TEMP = IR_1.readObjectTempC(); 
  Input = map(TEMP, 0, 1024, 0, 255); 
  Setpoint = 30;  
  Serial.begin(9600); 
  myPID.SetMode(AUTOMATIC);  
  myPID.SetSampleTime(sampleRate);
  lastMessage = millis(); 
}
void loop() {
  Setpoint = Tset-0.4; // Temperature set point offset value
  TEMP = IR_1.readObjectTempC();
  Input = map(TEMP, 0, 255, 0, 255); 
  double gap = abs(Setpoint - Input);
  myPID.SetTunings(aggKp, aggKi, aggKd);
  myPID.Compute();
  now = millis(); 
  if (gap < 9) {
    T2Enable = false;
    T1Enable = true;
  }
  if (T1Enable) {
    analogWrite(NiChrome, Output);  // Control heating with PID controller
  }
  if (gap > 9) {
    T1Enable = false;
    T2Enable = true;
  }
  if (T2Enable) {
    analogWrite(NiChrome, 255); // Heat wire at max power
  }
  if (now - lastMessage > serialPing) {
    Serial.print(IR_1.readAmbientTempC()); //Print temperature values to serial
    Serial.print(",");
    Serial.print(IR_1.readObjectTempC());
    Serial.print(",");
    Serial.print(IR_2.readAmbientTempC());
    Serial.print(",");
    Serial.print(IR_2.readObjectTempC());
    Serial.println();
    lastMessage = now;
  }
}
