#include "main.h"

int8_t relay_map[RELAY_COUNT];
int8_t relay_state_map[RELAY_COUNT];
unsigned long last_updated_at;
bool relay_state_updated = false;

Switch switches[SWITCH_COUNT];

SerialConsole console(Serial);
ModbusMaster node;

void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(57600);
  console.enableEcho();
  
  loadConfiguration();

  Serial.println(F("Lights controller v1.0"));
  Serial.print(F("lights> "));

  processRelayUnit(0);
  processRelayUnit(1);
  relay_state_updated = true;

  Serial1.begin(9600);

  DmxSimple.usePin(3);
  DmxSimple.maxChannel(16);

  for(int i=1; i<16; i++) DmxSimple.write(i, 255);
  last_updated_at = millis();
}

void loop()
{
  int i;
  console.pull();

  for (i=0; i<SWITCH_COUNT; i++) {
    if (switches[i].enabled) switches[i].update();
  }
  
  if (relay_state_updated || (last_updated_at + UPDATE_RELAYS_EVERY < millis())) {
    relay_state_updated = false;    

    sendRelayStates(0, 1, node);
    delay(50);
    sendRelayStates(1, 2, node);

    last_updated_at = millis();
  }

  processRelayUnit(0);
  processRelayUnit(1);    
}

void sendRelayStates(uint8_t unit, uint8_t address, ModbusMaster & node) {
  uint16_t value;
  uint8_t word, bit, relay, relay_state;

  for (word=0; word <= RELAYS_PER_UNIT / 16; word++) {
    for (bit=0; bit < 16; bit++) {
      relay = word * 16 + bit;
      if (relay < RELAYS_PER_UNIT)
        relay_state = relay_state_map[relay + unit * RELAYS_PER_UNIT] ? 1 : 0;
      else
        relay_state = 0;

      bitWrite(value, bit, relay_state);
    }

    node.setTransmitBuffer(word, value);
  }
  node.begin(address, Serial1);
  node.writeMultipleCoils(0, RELAYS_PER_UNIT);
}

void processRelayUnit(int8_t unit) {
  int8_t sw, i;
  bool value;

  for (i=0; i<RELAYS_PER_UNIT; i++) {
    sw = relay_map[i + unit * RELAYS_PER_UNIT];
    
    if ((sw <= -1) || (sw >= SWITCH_COUNT))
      value = 0;
    else
      value = switches[sw].enabled ? (!switches[sw].read() & 1) : 0;      

    if (relay_state_map[i + unit * RELAYS_PER_UNIT] != value) 
      relay_state_updated = true;
    
    relay_state_map[i + unit * RELAYS_PER_UNIT] = value;
  }
}