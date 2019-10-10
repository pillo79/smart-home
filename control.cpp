#include "control.h"

#include "hardware.h"

void controlSetup()
{

}

void controlLoop()
{
	HW::updateInputs();

	HW::outBoilerHeatRequest.setValue(HW::inTempLow.getValue());

	HW::updateOutputs();
}
