#include "hardware.h"

#include "logicdevice.h"
#include "modbusdevice.h"
#include "controllinodevice.h"
#include "modbus.h"
#include "shell.h"

#include <Controllino.h>

static LogicDevice Undefined("Undefined");
static ControllinoDevice Controllino("Controllino");

static Aermec_ANLI Aermec_ANLI_1("heatpump", 1);
//static Seneca_4RTD Seneca_4RTD_2("temp", 2);

static AM2302 AM2302_Temp("house_temp", 42);

ModbusDevice * const MODBUS_DEV_TABLE[] = {
	&Aermec_ANLI_1,
//	&Seneca_4RTD_2,
	&AM2302_Temp,
	&ShellDeviceInstance,
	NULL
};

#define INIT_IO(var, type, dev...) type HW::var( #var, &dev )
INIT_IO(inTempLow,				BitInput,	Controllino,	CONTROLLINO_IN0);
INIT_IO(outBoilerHeatRequest,			BitOutput,	Controllino,	CONTROLLINO_R4);
