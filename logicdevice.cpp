#include "logicdevice.h"

#include <errno.h>

LogicDevice::LogicDevice(const char *name)
	: m_name(name)
{

}

LogicDevice::~LogicDevice()
{

}

int LogicDevice::updateInputs()
{
	return 0;
}

int LogicDevice::updateOutputs()
{
	return 0;
}

bool LogicDevice::failing()
{
	return false;
}

int LogicDevice::getDigInput(int input)
{
	UNUSED(input);
	return -ENXIO;
}

int LogicDevice::getDigOutput(int output)
{
	UNUSED(output);
	return -ENXIO;
}

int LogicDevice::setDigOutput(int output, bool value)
{
	UNUSED(output);
	UNUSED(value);
	return -ENXIO;
}

int LogicDevice::getInputVal(int input)
{
	UNUSED(input);
	return -ENXIO;
}

int LogicDevice::getOutputVal(int output)
{
	UNUSED(output);
	return -ENXIO;
}

int LogicDevice::setOutputVal(int output, int value)
{
	UNUSED(output);
	UNUSED(value);
	return -ENXIO;
}
