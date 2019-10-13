#include "main.h"

int8_t relay_map[RELAY_COUNT];
int8_t relay_state_map[RELAY_COUNT];
unsigned long last_updated_at;
bool relay_state_updated = false;
bool slave_mode = false;

Switch switches[SWITCH_COUNT];

SerialConsole console(Serial);
ModbusMaster node;
Modbus slave(1, MODBUS_CONTROL_PIN_NONE);

void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
  pinMode(SLAVE_SWITCH_PIN, INPUT_PULLUP);
  delay(100);

  // Clean relay map
  for(int i=0; i<RELAY_COUNT; i++) relay_map[i] = 0;

  // Use slave mode if 0 livel on pin
  slave_mode = !digitalRead(SLAVE_SWITCH_PIN);

  loadConfiguration();
  Serial1.begin(9600);

  DmxSimple.usePin(3);
  DmxSimple.maxChannel(16);

  for(int i=1; i<=16; i++) DmxSimple.write(i, 255);
  last_updated_at = millis();

  Serial.begin(115200);
  slave.begin(115200);
  slave.cbVector[CB_READ_COILS] = readCoils;
  slave.cbVector[CB_WRITE_COILS] = writeRelays;
  slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = writeDimmers;

  if (slave_mode) return;

  processRelayUnit(0);
  processRelayUnit(1);
  relay_state_updated = true;

  console.enableEcho();
  Serial.println(F("Lights controller v1.0"));
  Serial.print(F("lights> "));
}

void loop()
{
  int i;
  if (slave_mode) slave.poll(); else console.poll();

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
  
  if (slave_mode) return;

  processRelayUnit(0);
  processRelayUnit(1);    
}

uint8_t readSwitches(uint8_t fc, uint16_t address, uint16_t length) {
  bool value;
  uint8_t sw;
  
  for(int i=0; i<length; i++) {
    sw = i + address - 1; // We starts from 1 coil
    
    if (sw > SWITCH_COUNT) return STATUS_ILLEGAL_DATA_ADDRESS;

    value = switches[sw].enabled ? (!switches[sw].read() & 1) : 0;
    slave.writeCoilToBuffer(i, value);
  }
  
  return STATUS_OK;
}

uint8_t readRelays(uint8_t fc, uint16_t address, uint16_t length) {
  uint8_t relay;
  
  for(uint16_t i=0; i<length; i++) {
    relay = i + address;
    
    if (relay >= RELAY_COUNT) return STATUS_ILLEGAL_DATA_ADDRESS;

    slave.writeCoilToBuffer(i, relay_state_map[relay] ? 1 : 0);
  }
  
  return STATUS_OK;
}

uint8_t readCoils(uint8_t fc, uint16_t address, uint16_t length) {
  if (address < MODBUS_RELAY_SHIFT) return readSwitches(fc, address, length);
  return readRelays(fc, address - MODBUS_RELAY_SHIFT, length); //Shifted to 101, 102....
}

uint8_t writeRelays(uint8_t fc, uint16_t address, uint16_t length) {
  bool value;
  uint8_t relay;
  for(uint16_t i=0; i<length; i++) {
    relay = i + address - MODBUS_RELAY_SHIFT;
    
    if (relay >= RELAY_COUNT) return STATUS_ILLEGAL_DATA_ADDRESS;

    value = slave.readCoilFromBuffer(i);
    relay_state_updated = relay_state_map[relay] == value;
    relay_state_map[relay] = value;
  }
  
  return STATUS_OK;
}

uint8_t writeDimmers(uint8_t fc, uint16_t address, uint16_t length) {
  uint8_t value;
  uint8_t channel;
  
  if ((address < 1) || (address + length > DMX_DIMMERS_COUNT))
    return STATUS_ILLEGAL_DATA_ADDRESS;

  for(uint16_t i=0; i<length; i++) {
    channel = i + address;
    value = (uint8_t) slave.readRegisterFromBuffer(channel);
    DmxSimple.write(channel, value);
  }
  
  return STATUS_OK;
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