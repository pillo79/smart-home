#include "modbusdevice.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

static modbus_msg_t MODBUS_NO_OP[] = { { 0 } };

ModbusDevice::ModbusDevice(const char *name, int modAddress)
	: LogicDevice(name)
	, m_address (modAddress)
	, m_failures (0)
{
}


Aermec_ANLI::Aermec_ANLI(const char *name, int modAddress)
	: ModbusDevice(name, modAddress)
{
}

const modbus_msg_t *Aermec_ANLI::getMessages()
{
	return NULL;
}

int Aermec_ANLI::getDigInput(int input)
{
	return 0;
}

int Aermec_ANLI::getDigOutput(int output)
{
	return 0;
}

int Aermec_ANLI::setDigOutput(int output, bool value)
{
	return 0;
}

int Aermec_ANLI::getInputVal(int input)
{
	return 0;
}

int Aermec_ANLI::setOutputVal(int output, int value)
{
	return 0;
}




#if 0

/* Z-10-D-IN */
Seneca_10DI::Seneca_10DI(int modAddress)
	: ModbusDevice(modAddress)
	, mInputs(0)
{
}

int Seneca_10DI::updateInputs()
{
	return mbReadReg(40002, 1, &mInputs);
}

int Seneca_10DI::getDigInput(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 10) return -ENXIO;
	return mInputs & (1 << (input-1));
}



/* Z-10-D-OUT */
Seneca_10DO::Seneca_10DO(int modAddress)
	: ModbusDevice(modAddress)
	, mOutputs(0)
{
}

int Seneca_10DO::updateOutputs()
{
	return mbWriteReg(40003, 1, &mOutputs);
}

int Seneca_10DO::getDigOutput(int output)
{
	if (output < 1) return -ENXIO;
	if (output > 10) return -ENXIO;
	return mOutputs & (1 << (output-1));
}

int Seneca_10DO::setDigOutput(int output, bool value)
{
	if (output < 1) return -ENXIO;
	if (output > 10) return -ENXIO;
	if (value)
		mOutputs |= (1 << (output-1));
	else
		mOutputs &= ~(1 << (output-1));

	return 0;
}



/* ZC-24DO */
Seneca_24DO::Seneca_24DO(int modAddress)
	: ModbusDevice(modAddress)
	, mOutputs(0)
{
}

int Seneca_24DO::updateOutputs()
{
	uint16_t words[2] = {
		mOutputs & 0xffff,
		mOutputs >> 16
	};

	return mbWriteReg(40301, 2, words);
}

int Seneca_24DO::getDigOutput(int output)
{
	if (output < 1) return -ENXIO;
	if (output > 24) return -ENXIO;
	return mOutputs & (1 << (output-1));
}

int Seneca_24DO::setDigOutput(int output, bool value)
{
	if (output < 1) return -ENXIO;
	if (output > 24) return -ENXIO;
	if (value)
		mOutputs |= (1 << (output-1));
	else
		mOutputs &= ~(1 << (output-1));

	return 0;
}



/* ZC-16DI-8DO */
Seneca_16DI_8DO::Seneca_16DI_8DO(int modAddress)
	: ModbusDevice(modAddress)
	, mInputs(0)
	, mOutputs(0)
{
}

int Seneca_16DI_8DO::updateInputs()
{
	int ret;

	// read digital inputs
	ret = mbReadReg(40301, 1, &mInputs);
	if (ret < 1) return ret;

	// read counters
	ret = mbReadReg(40009, 16, mCounters);
	return ret;
}

int Seneca_16DI_8DO::updateOutputs()
{
	return mbWriteReg(40005, 1, &mOutputs);
}

int Seneca_16DI_8DO::getDigInput(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 16) return -ENXIO;
	return mInputs & (1 << (input-1));
}

int Seneca_16DI_8DO::getDigOutput(int output)
{
	if (output < 1) return -ENXIO;
	if (output > 8) return -ENXIO;
	return mOutputs & (1 << (output-1));
}

int Seneca_16DI_8DO::setDigOutput(int output, bool value)
{
	if (output < 1) return -ENXIO;
	if (output > 8) return -ENXIO;
	if (value)
		mOutputs |= (1 << (output-1));
	else
		mOutputs &= ~(1 << (output-1));
	return 0;
}

int Seneca_16DI_8DO::getInputVal(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 8) return -ENXIO;

	input = (input-1) << 1;
	return (mCounters[input] << 16) | mCounters[input+1];
}



/* Z-4RTD-2 */
Seneca_4RTD::Seneca_4RTD(int modAddress)
	: ModbusDevice(modAddress)
{
	for (int i=0; i<4; ++i)
		mInputs[i] = 0;
}

int Seneca_4RTD::updateInputs()
{
	int ret = mbReadReg(40003, 4, (uint16_t*) mInputs);
	if (ret < 0) {
		uint16_t reset_code = 0xcccc;
		mbWriteReg(40029, 1, &reset_code);
	}
	return ret;
}

int Seneca_4RTD::getInputVal(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 4) return -ENXIO;
	return mInputs[input-1];
}



/* Z-4AI */
Seneca_4AI::Seneca_4AI(int modAddress)
	: ModbusDevice(modAddress)
{
	for (int i=0; i<4; ++i)
		mInputs[i] = 0;
}

int Seneca_4AI::updateInputs()
{
	return mbReadReg(40017, 4, mInputs);
}

int Seneca_4AI::getInputVal(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 4) return -ENXIO;
	return mInputs[input-1];
}



/* Z-3AO */
Seneca_3AO::Seneca_3AO(int modAddress)
	: ModbusDevice(modAddress)
{
	for (int i=0; i<3; ++i)
		mOutputs[i] = 0;
}

int Seneca_3AO::updateOutputs()
{
	return mbWriteReg(40005, 3, mOutputs);
}

int Seneca_3AO::getOutputVal(int output)
{
	if (output < 1) return -ENXIO;
	if (output > 3) return -ENXIO;
	return mOutputs[output-1];
}

int Seneca_3AO::setOutputVal(int output, int val)
{
	if (output < 1) return -ENXIO;
	if (output > 3) return -ENXIO;

	mOutputs[output-1] = val;
	return 0;
}



/* Sensore potenza */
Eastron_SDM230::Eastron_SDM230(int modAddress)
	: ModbusDevice(modAddress)
{
	for (int i=0; i<2; ++i)
		mInputs[i] = 0;
}

int Eastron_SDM230::updateInputs()
{
	return mbReadReg(30343, 2, mInputs);
}

int Eastron_SDM230::getInputVal(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 2) return -ENXIO;
	return mInputs[input-1];
}



/* Sensore temp/umidita */
Burosoft_Temp::Burosoft_Temp(int modAddress)
	: ModbusDevice(modAddress)
{
	for (int i=0; i<2; ++i)
		mInputs[i] = 0;
}

int Burosoft_Temp::updateInputs()
{
	return mbReadReg(40001, 2, mInputs);
}

int Burosoft_Temp::getInputVal(int input)
{
	if (input < 1) return -ENXIO;
	if (input > 2) return -ENXIO;
	return mInputs[input-1];
}

#endif
