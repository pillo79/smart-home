#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "global.h"

class ModbusDevice;
extern ModbusDevice * const MODBUS_DEV_TABLE[];

void modbusSetup();
void modbusPoll();

#endif /* __MODBUS_H__ */
