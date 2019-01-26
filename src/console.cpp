#include "main.h"

void SerialConsole::commandHandler(int argc, char ** argv) {
  char *command = argv[0];
  uint8_t unit, pin, relay;
  int8_t sw;

  if (!strcmp(command, "ping")) {
    m_serial->println(F("pong"));
  } else if (!strcmp(command, "relay")) {
    unit = atoi(argv[1]) - 1;
    relay = atoi(argv[2]) - 1;
    sw = atoi(argv[3]) - 1;

    if (relay >= RELAYS_PER_UNIT) {
      m_serial->println(F("Error: we have only 24 relays per unit"));
    } else if (unit > 1) {
      m_serial->println(F("Error: we have only 2 units"));
    } else if (sw >= SWITCH_COUNT) {
      m_serial->print(F("Error: we have only "));
      m_serial->print(SWITCH_COUNT);
      m_serial->println(F(" switches"));
    } else {
      relay_map[relay + unit * RELAYS_PER_UNIT] = sw;
      m_serial->print(F("OK: Relay "));
      m_serial->print(relay + 1);
      m_serial->print(F(" assigned to switch "));
      m_serial->print(sw + 1);
      m_serial->print(F(" on unit "));
      m_serial->println(unit + 1);
    }
  } else if (!strcmp(command, "switch")) {
    sw = atoi(argv[2]) - 1;
    pin = atoi(argv[3]);
    if (sw >= SWITCH_COUNT) {
      m_serial->print(F("Error: we have only "));
      m_serial->print(SWITCH_COUNT);
      m_serial->println(F(" switches"));
    } else
      if (!strcmp(argv[1], "enable")) {
        if ((pin < 22) || (pin > 44)) {
          m_serial->println(F("Error: please, use only pins from 22 to 44"));
        } else {
          switches[sw].enable();
          switches[sw].attach(pin, INPUT_PULLUP);
          m_serial->print(F("OK: Switch "));
          m_serial->print(sw + 1);
          m_serial->print(F(" enabled on pin "));
          m_serial->println(pin);
        }
      } else if (!strcmp(argv[1], "disable")) {
        sw = atoi(argv[2]) - 1;
        switches[sw].disable();
        m_serial->print(F("OK: Switch "));
        m_serial->print(sw + 1);
        m_serial->println(F(" disabled"));
      }
      else
        m_serial->println(F("Error: Unknown switch command"));
  } else if (!strcmp(command, "save")) {
    saveConfiguration();
    m_serial->println(F("OK: Configuration saved."));
  } else if (!strcmp(command, "load")) {
    loadConfiguration();
    m_serial->println(F("OK: Configuration loaded."));
  } else if (!strcmp(command, "show")) {
    if (!strcmp(argv[1], "relay"))
      showRelayMap(m_serial);
    else if (!strcmp(argv[1], "switch"))
      showSwitchConfig(m_serial);
    else
      m_serial->println(F("Error: Unknown object"));
  } else if (!strcmp(command, "help")) {
    cmdHelp(m_serial);
  }
  
  m_serial->println();

  m_serial->print(F("lights> "));
}

void cmdHelp(Stream * serial) {
  serial->println(F("Commands:"));
  serial->println(F("relay <relay unit> <relay num> <switch> - Setup connection"));
  serial->println(F("  between relay on selected unit with switch"));
  serial->println(F("switch <enable|disable> <switch_id> <pin> - config switch"));
  serial->println(F("save - save configuration to EEPROM"));
  serial->println(F("load - load configuration from EEPROM"));
  serial->println(F("show <relay|switch>- show current configuration"));
}

void SerialConsole::errorHandler(consoleError err) {
  switch (err) {
    case ERR_SERIAL_MAX_LENGTH:
      m_serial->println(F("Error: max command length reached"));
      break;
    case ERR_SERIAL_MAX_PARAMETERS:
      m_serial->println(F("Error: max parameters count reached"));
      break;
  }
}
