#include "main.h"

int8_t relay_map[48];
Switch switches[SWITCH_COUNT];

SerialConsole console(Serial);

void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(57600);
  console.enableEcho();
  
  loadConfiguration();

  Serial.println(F("Lights controller v1.0"));
  Serial.print(F("lights> "));

// communicate with Modbus slave ID 2 over Serial (port 0)
//  node.begin(1, Serial2);


}

void loop()
{
  console.pull();
  // node.setSlave(1);
  
  // Serial.print("Setting state ");
  // Serial.println(state);
  
  // node.setTransmitBuffer(0, state);
  // node.setTransmitBuffer(1, state);
  // node.writeMultipleCoils(0, 24);
  // delay(2000);
  // node.setSlave(2);
  // node.setTransmitBuffer(0, state);
  // node.setTransmitBuffer(1, state);
  // node.writeMultipleCoils(0, 24);
  // state = state ? 0 : 0xFFFF;
  // delay(15000);
    
}