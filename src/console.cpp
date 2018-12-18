#include "main.h"

void SerialConsole::commandHandler(int argc, char ** argv) {
  char *command = argv[0];
  
  if (!strcmp(command, "ping")) {
    m_serial->println(F("pong"));
  } else if (!strcmp(command, "relay")) {
    uint8_t unit = atoi(argv[1]) - 1;
    uint8_t relay = atoi(argv[2]) - 1;
    uint8_t sw = atoi(argv[3]);

    if (relay > 47) {
      m_serial->println(F("Error: we have only 48 relays"));
    } else {
      relay_map[relay + unit * 24] = sw;
      m_serial->print(F("OK: Relay "));
      m_serial->print(relay + 1);
      m_serial->print(F(" assigned to switch "));
      m_serial->print(sw);
      m_serial->print(F(" on unit "));
      m_serial->println(unit + 1);
    }
  } else if (!strcmp(command, "save")) {
    saveConfiguration();
    m_serial->println(F("OK: Relay map saved."));
  } else if (!strcmp(command, "load")) {
    loadConfiguration();
    m_serial->println(F("OK: Relay map loaded."));
  } else if (!strcmp(command, "show")) {
    showRelayMap(m_serial);
  }
  
  m_serial->println();

  m_serial->print(F("lights> "));
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
