#include <Wire.h>
#include <SparkFunMLX90614.h>
IRTherm IRSensor;
float newEmissivity = 0.98;
void setup()
{
  Serial.begin(9600); 
  Serial.println("Press any key to begin");
  while (!Serial.available()) ;
  IRSensor.begin(); 
  IRSensor.setUnit(TEMP_C);
  IRSensor.setEmissivity(newEmissivity);
  Serial.println("Emissivity: " + String(IRSensor.readEmissivity()));
}
void loop()
{
  if (IRSensor.read())
  {
    Serial.print("Object: " + String(IRSensor.object(), 2));
    Serial.write('°'); 
    Serial.println("F");
    Serial.print("Ambient: " + String(IRSensor.ambient(), 2));
    Serial.write('°'); 
    Serial.println("C");
    Serial.println();
  }
  delay(500);
}
