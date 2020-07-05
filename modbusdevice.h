#ifndef __MODBUSDEVICE_H__
#define __MODBUSDEVICE_H__

#include "global.h"
#include "logicdevice.h"

#include "ModbusRTU.h"

class ModbusDevice : public LogicDevice {
	public:
		ModbusDevice(const char *name, int modAddress);
		virtual ~ModbusDevice() { };

		bool failing() { return (m_failures > 20); }

		virtual const modbus_msg_t *getMessages();

	protected:
		int m_address;
		int m_failures;
};

class Seneca_4RTD : public ModbusDevice {
	public:
		Seneca_4RTD(const char *name, int modAddress);
		virtual ~Seneca_4RTD() { };

		virtual const modbus_msg_t *getMessages();

		virtual int getInputVal(int input);

	private:
		int16_t m_inputs[4]; // signed (negative temperatures)
		modbus_msg_t m_messages[2];
};

class Aermec_ANLI : public ModbusDevice {
	public:
		Aermec_ANLI(const char *name, int modAddress);
		virtual ~Aermec_ANLI() { };

		virtual const modbus_msg_t *getMessages();

		virtual int getDigInput(int input);
		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);
		virtual int getInputVal(int input);
		virtual int setOutputVal(int output, int value);

	private:
		bool m_updated;
		int16_t m_inputs[4]; // signed (negative temperatures)
		modbus_msg_t m_readMessages[2];
		modbus_msg_t m_writeMessages[2];
};

#if 0

class Seneca_10DI : public ModbusDevice {
	public:
		Seneca_10DI(int modAddress);
		virtual ~Seneca_10DI() { };

		virtual const modbus_msg_t *mbMessages();

		virtual int getDigInput(int input);

	private:
		uint16_t m_inputs;
		modbus_msg_t m_mbMessages[2];
};

class Seneca_10DO : public ModbusDevice {
	public:
		Seneca_10DO(int modAddress);
		virtual ~Seneca_10DO() { };

		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);
	private:
		uint16_t mOutputs;
};

class Seneca_24DO : public ModbusDevice {
	public:
		Seneca_24DO(int modAddress);
		virtual ~Seneca_24DO() { };

		virtual int updateOutputs();

		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);
	private:
		uint32_t mOutputs;
};

class Seneca_16DI_8DO : public ModbusDevice {
		public:
		Seneca_16DI_8DO(int modAddress);
		virtual ~Seneca_16DI_8DO() { };

		virtual int updateInputs();
		virtual int updateOutputs();

		virtual int getDigInput(int input);
		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);

		virtual int getInputVal(int input);
	private:
		uint16_t mInputs;
		uint16_t mOutputs;

		uint16_t mCounters[16];
};

class Seneca_4AI : public ModbusDevice {
	public:
		Seneca_4AI(int modAddress);
		virtual ~Seneca_4AI() { };

		virtual int updateInputs();

		virtual int getInputVal(int input);
	private:
		uint16_t mInputs[4];
};

class Seneca_3AO : public ModbusDevice {
	public:
		Seneca_3AO(int modAddress);
		virtual ~Seneca_3AO() { };

		virtual int updateOutputs();

		virtual int getOutputVal(int output);
		virtual int setOutputVal(int output, int value);
	private:
		uint16_t mOutputs[3];
};

class Eastron_SDM230 : public ModbusDevice {
	public:
		Eastron_SDM230(int modAddress);
		virtual ~Eastron_SDM230() { };

		virtual int updateInputs();

		virtual int getInputVal(int input);
	private:
		uint16_t mInputs[2];
};

class Burosoft_Temp : public ModbusDevice {
	public:
		Burosoft_Temp(int modAddress);
		virtual ~Burosoft_Temp() { };

		virtual int updateInputs();

		virtual int getInputVal(int input);
	private:
		uint16_t mInputs[2];
};

#endif

#endif /* __MODBUSDEVICE_H__ */
