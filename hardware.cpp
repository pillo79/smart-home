#include "hardware.h"

#include "logicdevice.h"
#include "controllinodevice.h"

#include <Controllino.h>

static LogicDevice Undefined("Undefined");
static ControllinoDevice Controllino("Controllino");

static LogicDevice *DEVICES[] = {
	&Undefined,
	&Controllino,
};

#define INIT_IO(var, type, dev...) type HW::var( #var, &dev )

INIT_IO(inTempLow,				BitInput,	Controllino,	CONTROLLINO_IN0);
INIT_IO(outBoilerHeatRequest,			BitOutput,	Controllino,	CONTROLLINO_R4);

void HW::updateInputs()
{
	for (int i=0; i<sizeof(DEVICES)/sizeof(DEVICES[0]); ++i)
		DEVICES[i]->updateInputs();
}

void HW::updateOutputs()
{
	for (int i=0; i<sizeof(DEVICES)/sizeof(DEVICES[0]); ++i)
		DEVICES[i]->updateOutputs();
}
