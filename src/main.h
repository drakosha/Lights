#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include <Bounce2.h>
#include <ModbusMaster.h>
#include <SerialConsole.h>
#include <EEPROM.h>
#include "switch.h"

// instantiate ModbusMaster object
// ModbusMaster node;

#define RELAY_COUNT 64
#define SWITCH_COUNT 22
#define BOUNCE_INTERAL 50

extern int8_t relay_map[48];
extern Switch switches[SWITCH_COUNT];

void loadConfiguration();
void saveConfiguration();

void showRelayMap(Stream *serial);

#endif // __MAIN_H__


