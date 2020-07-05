#include "control.h"

#include "hardware.h"
#include "modbus.h"

void controlSetup()
{
	modbusSetup();
}

void controlLoop()
{
	modbusPoll();

	HW::outBoilerHeatRequest.setValue(HW::inTempLow.getValue());
}
