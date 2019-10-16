#include "controllinodevice.h"

#include <errno.h>
#include <Arduino.h>

ControllinoDevice::ControllinoDevice(const char *name)
	: LogicDevice(name)
{

}

ControllinoDevice::~ControllinoDevice()
{

}

int ControllinoDevice::updateInputs()
{
	return 0;
}

int ControllinoDevice::updateOutputs()
{
	return 0;
}

bool ControllinoDevice::failing()
{
	return false;
}

int ControllinoDevice::getDigInput(int input)
{
	return digitalRead(input);
}

int ControllinoDevice::getDigOutput(int output)
{
	return digitalRead(output);
}

int ControllinoDevice::setDigOutput(int output, bool value)
{
	digitalWrite(output, value? HIGH : LOW);
	return 0;
}

int ControllinoDevice::getInputVal(int input)
{
	return analogRead(input);
}

int ControllinoDevice::getOutputVal(int output)
{
	UNUSED(output);
	return -ENXIO;
}

int ControllinoDevice::setOutputVal(int output, int value)
{
	UNUSED(output);
	UNUSED(value);
	return -ENXIO;
}
