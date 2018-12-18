#include "main.h"

void showRelaySwitches(Stream *serial, uint8_t unit, uint8_t start) {
  uint8_t i;

  serial->print(F("Relays    --"));
  for (i = start + 1; i < start + 13; i++) {
    serial->print(i);
    serial->print(i < 10 ? "--" : "-");
  }
  serial->println();
  serial->println(F("            |  |  |  |  |  |  |  |  |  |  |  |"));
  serial->print(  F("Switches    "));
  for (i = start; i < start + 12; i++) {
    serial->print(relay_map[i + unit * 24]);
    serial->print(relay_map[i + unit * 24] < 10 ? "  " : " ");
  }
  serial->println();
}

void showSwitchConfig(Stream *serial) {
  serial->println(F("Switches configuration (switch = pin [state]):"));
  serial->println(F("  "));

  for (uint8_t i = 0; i < SWITCH_COUNT; i++) {
    Switch & s = switches[i];

    if (!s.enabled()) continue;

    serial->print(i);
    serial->print(i < 10 ? "  = " : " = ");
    serial->print(s.getPin());
  }
}

void saveConfiguration() {
  uint8_t i;

  // saving relays map
  for (i = 0; i < RELAY_COUNT; i++) 
    EEPROM.update(i, relay_map[i]);

  // saving switches configuration
  for (i = 0; i < SWITCH_COUNT; i++) {
    EEPROM.update(i + RELAY_COUNT, switches[i].getPin());
  }
}

void loadConfiguration() {
  uint8_t i;
  uint8_t pin;

  for (i = 0; i < RELAY_COUNT; i++) 
    relay_map[i] = EEPROM.read(i);

  for (i = 0; i < SWITCH_COUNT; i++) {
    pin = EEPROM.read(i + RELAY_COUNT);
    if (pin) {
      switches[i].attach(pin, INPUT_PULLUP);
      switches[i].enable();
    } else
      switches[i].disable();
  }
}

void showRelayMap(Stream *serial) {
  serial->println(F("Relay map unit 1:"));
  showRelaySwitches(serial, 0, 0);
  serial->println();
  serial->println();
  showRelaySwitches(serial, 0, 12);
  serial->println();

  serial->println(F("Relay map unit 2:"));
  showRelaySwitches(serial, 1, 0);
  serial->println();
  showRelaySwitches(serial, 1, 12);
  serial->println();
};
