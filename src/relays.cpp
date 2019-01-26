#include "main.h"

void showRelaySwitches(Stream *serial, uint8_t unit, uint8_t start) {
  uint8_t i;
  int8_t sw;

  serial->print(F("Relays    --"));
  for (i = start + 1; i < start + 13; i++) {
    serial->print(i);
    serial->print(i < 10 ? "--" : "-");
  }
  serial->println();
  serial->println(F("            |  |  |  |  |  |  |  |  |  |  |  |"));
  serial->print(  F("Switches    "));
  for (i = start; i < start + 12; i++) {
    sw = relay_map[i + unit * 24] + 1;
    if (sw) {
      serial->print(sw);
      serial->print(((sw < 10) && (sw > 0)) ? "  " : " ");
    } else serial->print(F("x  "));
  }
  serial->println();
}

void showSwitchConfig(Stream *serial) {
  serial->println(F("Switches configuration (switch = pin [state]):"));
  serial->println(F("  "));

  for (uint8_t i = 0; i < SWITCH_COUNT; i++) {
    Switch & s = switches[i];

    if (!s.enabled) continue;

    serial->print(i + 1);
    serial->print(i < 11 ? "  = " : " = ");
    serial->print(s.getPin());
    serial->print(F(" ["));
    serial->print(s.read() ? F("HIGH") : F("LOW"));
    serial->println(F("]"));
  }
}

void saveConfiguration() {
  uint8_t i;

  // saving relays map
  for (i = 0; i < RELAY_COUNT; i++) 
    EEPROM.update(i, relay_map[i]);

  // saving switches configuration
  for (i = 0; i < SWITCH_COUNT; i++) {
    EEPROM.update(i + RELAY_COUNT, switches[i].enabled ? switches[i].getPin() : -1);
  }
}

void loadConfiguration() {
  uint8_t i;
  int8_t pin;

  for (i = 0; i < RELAY_COUNT; i++) 
    relay_map[i] = EEPROM.read(i);

  for (i = 0; i < SWITCH_COUNT; i++) {
    pin = EEPROM.read(i + RELAY_COUNT);
    if (pin != -1) {
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
