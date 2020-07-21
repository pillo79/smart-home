#ifndef __SHELL_H__
#define __SHELL_H__

#include "modbusdevice.h"

class ShellDevice : public ModbusDevice {
	public:
		ShellDevice();
		virtual ~ShellDevice() { };

		virtual const modbus_msg_t *getMessages();

		static void cmdReadReg(int *args);
		static void cmdWriteReg(int *args);
		static void cmdReadCoil(int *args);
		static void cmdWriteCoil(int *args);
	private:
		static bool m_msgReady;
		static bool m_msgWaitReply;
		static int16_t m_regs[32];
		static modbus_msg_t m_msg[2];
};

extern ShellDevice ShellDeviceInstance;

void shell(char *msg);

#endif // __SHELL_H__
