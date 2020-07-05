#ifndef __CONTROLLINODEVICE_H__
#define __CONTROLLINODEVICE_H__

#include "global.h"
#include "logicdevice.h"

class ControllinoDevice : public LogicDevice {
	public:
		ControllinoDevice(const char *name);
		virtual ~ControllinoDevice();

		virtual int getDigInput(int input);
		virtual int getDigOutput(int output);
		virtual int setDigOutput(int output, bool value);
		virtual int getInputVal(int input);
		virtual int getOutputVal(int output);
		virtual int setOutputVal(int output, int value);
};

#endif /* __CONTROLLINODEVICE_H__ */
