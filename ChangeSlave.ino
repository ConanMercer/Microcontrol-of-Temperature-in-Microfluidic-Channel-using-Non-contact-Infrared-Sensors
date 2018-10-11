#include "i2cmaster.h"
// Desired new slave address
byte NewAddress = 0x5A;
void setup() {
  Serial.begin(9600);
  i2c_init();
  PORTC = (1 << PORTC4) | (1 << PORTC5);
  delay(5000);
  ReadAddr(0);
  ChangeAddr(NewAddress, 0x00);
  ReadAddr(0);
  Serial.print("Power device on and off");
  Serial.print(NewAddress, HEX);
  Serial.println("do so within next 20 seconds");
  delay(20000);
  ReadTemp(0);
  ReadTemp(NewAddress << 1);
  Serial.println("Device Slave Address Remap Completed");
}
void loop() {
  delay(5000);
  ReadTemp(NewAddress << 1);
}
word ChangeAddr(byte NewAddr1, byte NewAddr2) {
  i2c_write(0x2E);
  i2c_write(0x00);
  i2c_write(0x00);
  if (i2c_write(0x6F) == 0) {
    i2c_stop();
    Serial.println("Erased data");
  }
  else {
    i2c_stop();
    Serial.println("Attempted erasing of data failed");
    return -1;
  }
  Serial.print("Writing data");
  Serial.print(NewAddr1, HEX);
  Serial.print(", ");
  Serial.println(NewAddr2, HEX);
  for (int a = 0; a != 256; a++) {
    i2c_start_wait(0 + I2C_WRITE);
    i2c_write(0x2E);
    i2c_write(NewAddr1);
    i2c_write(NewAddr2);
    if (i2c_write(a) == 0) {
      i2c_stop();
      delay(100);
      Serial.print("Found correct CRC: 0x");
      Serial.println(a, HEX);
      return a;
    }
  }
  i2c_stop();
  Serial.println("> Correct CRC not found");
  return -1;
}
void ReadAddr(byte Address) {
  Serial.println("Read address");
  Serial.print("  Sensor address: ");
  Serial.print(Address, HEX);
  Serial.print(", Data: ");
  i2c_start_wait(Address + I2C_WRITE);
  i2c_write(0x2E);
  i2c_rep_start(Address + I2C_READ);
  Serial.print(i2c_readAck(), HEX);
  Serial.print(", ");
  Serial.print(i2c_readAck(), HEX);
  Serial.print(", ");
  Serial.println(i2c_readNak(), HEX);
  i2c_stop();
}
float ReadTemp(byte Address) {
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  Serial.println("Read temperature");
  // Inform the user
  Serial.print("  Sensor address: ");
  Serial.print(Address, HEX);
  Serial.print(", ");
  i2c_start_wait(Address + I2C_WRITE);
  i2c_write(0x07);
  i2c_rep_start(Address + I2C_READ);
  data_low = i2c_readAck();
  data_high = i2c_readAck();
  pec = i2c_readNak();
  i2c_stop();
  float temp = 0x0000;
  temp = (float)(((data_high & 0x007F) << 8) + data_low);
  temp = (temp * 0.02) - 273.16;
  Serial.print(temp);
  Serial.println(" C");
  return temp;
}
