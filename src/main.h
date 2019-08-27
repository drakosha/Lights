#ifndef __MAIN_H__
#define __MAIN_H__

#define BOUNCE_INTERAL 50

#include <Arduino.h>
#include <Bounce2.h>
#include <ModbusMaster.h>
#include <SerialConsole.h>
#include <EEPROM.h>
#include <DMXSimple.h>
#include "switch.h"

// instantiate ModbusMaster object
// ModbusMaster node;

#define RELAY_UNIT_COUNT 2
#define RELAYS_PER_UNIT 24
#define RELAY_COUNT RELAY_UNIT_COUNT*RELAYS_PER_UNIT

#define UPDATE_RELAYS_EVERY 1000   // update interval in milliseconds
#define SWITCH_COUNT 24

extern int8_t relay_map[RELAY_COUNT];
extern Switch switches[SWITCH_COUNT];

void loadConfiguration();
void saveConfiguration();

void showRelayMap(Stream *serial);
void showSwitchConfig(Stream * serial);

void sendRelayStates(uint8_t unit, uint8_t address, ModbusMaster & node);
void processRelayUnit(int8_t unit);

void cmdHelp(Stream * serial);
#endif // __MAIN_H__


