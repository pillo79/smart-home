#ifndef __LOGICDEVICE_H__
#define __LOGICDEVICE_H__

#include "global.h"

class LogicDevice {
	public:
		LogicDevice(const char *name);
		virtual ~LogicDevice();

		const char *name() { return m_name; }

		virtual int updateInputs();
		virtual int updateOutputs();
		virtual bool failing();

		virtual int getDigInput(int input);
		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);
		virtual int getInputVal(int input);
		virtual int getOutputVal(int output);
		virtual int setOutputVal(int output, int value);

	private:
		const char *m_name;
};

#endif /* __LOGICDEVICE_H__ */
