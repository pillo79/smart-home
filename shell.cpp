#include "pins.h"
#include "shell.h"
#include "log.h"
#include "version.h"

#include <avr/wdt.h>
#include <Arduino.h>
#include <stdio.h>

void printTable(const struct SignalDesc *table)
{
	for (const struct SignalDesc *sig = table; sig->name; sig++) {
		lprintf("%4s", sig->name);
	}
	lprintf("\n");

	for (const struct SignalDesc *sig = table; sig->name; sig++) {
		lprintf("%4i", (digitalRead(sig->pin) == HIGH) ? 1 : 0);
	}
	lprintf("\n");
}

void printSig(const struct SignalDesc *sig)
{
	lprintf("%4s\n%4i\n", sig->name, (digitalRead(sig->pin) == HIGH) ? 1 : 0);
}

ShellDevice ShellDeviceInstance;
modbus_msg_t ShellDevice::m_msg[2];
int16_t ShellDevice::m_regs[32];
bool ShellDevice::m_msgReady;
bool ShellDevice::m_msgWaitReply;

ShellDevice::ShellDevice()
	: ModbusDevice ("shell", 0)
{
	m_msgReady = m_msgWaitReply = false;
	m_msg[1].u8fct = 0;
}

const modbus_msg_t *ShellDevice::getMessages()
{
	if (m_msgReady) {
		m_msgReady = false;
		m_msgWaitReply = true;
		return m_msg;
	} else if (m_msgWaitReply) {
		int i;
		switch (m_msg[0].u8fct) {
			case MB_FC_READ_REGISTERS:
				for (i=0; i<m_msg[0].u16CoilsNo; ++i)
					lprintf("R[%3i]: %i\n", m_msg[0].u16RegAdd+i, m_regs[i]);
				break;
			case MB_FC_READ_COILS:
				for (i=0; i<m_msg[0].u16CoilsNo; ++i)
					lprintf("C[%3i]: %i\n", m_msg[0].u16RegAdd+i, (m_regs[i >> 4] & (1 << (i&15))) ? 1 : 0);
				break;
			default: break;
		}
		m_msgWaitReply = false;
		return NULL;
	} else {
		return NULL;
	}
}

static void ShellDevice::cmdReadReg(int *args)
{
	m_msg[0].u8id		= args[0];
	m_msg[0].u8fct	 	= MB_FC_READ_REGISTERS;
	m_msg[0].u16RegAdd	= args[1];
	m_msg[0].u16CoilsNo	= args[2];
	m_msg[0].au16reg	= m_regs+0;
	m_msgReady = true;
}

static void ShellDevice::cmdWriteReg(int *args)
{
	m_msg[0].u8id		= args[0];
	m_msg[0].u8fct		= MB_FC_WRITE_REGISTER;
	m_msg[0].u16RegAdd	= args[1];
	m_msg[0].u16CoilsNo	= 1;
	m_msg[0].au16reg	= m_regs+0;

	m_regs[0] = args[2];
	m_msgReady = true;
}

static void ShellDevice::cmdReadCoil(int *args)
{
	m_msg[0].u8id		= args[0];
	m_msg[0].u8fct		= MB_FC_READ_COILS;
	m_msg[0].u16RegAdd	= args[1];
	m_msg[0].u16CoilsNo	= args[2];
	m_msg[0].au16reg	= m_regs+0;
	m_msgReady = true;
}

static void ShellDevice::cmdWriteCoil(int *args)
{
	m_msg[0].u8id		= args[0];
	m_msg[0].u8fct		= MB_FC_WRITE_COIL;
	m_msg[0].u16RegAdd	= args[1];
	m_msg[0].u16CoilsNo	= 1;
	m_msg[0].au16reg	= m_regs+0;

	m_regs[0] = args[2];
	m_msgReady = true;
}

static void parseLongCommand(char *cmd)
{
	typedef int (*callback)(int *args);
	callback fcn;
	int arg_min = 0, arg_max = 0;
	int arg_value[4];

	if (!strcmp(cmd, "mb_read_reg")) {
		arg_min = 2;
		arg_max = 3;
		arg_value[2] = 1;
		fcn = &ShellDevice::cmdReadReg;
	}
	else if (!strcmp(cmd, "mb_write_reg")) {
		arg_min = 3;
		arg_max = 3;
		fcn = &ShellDevice::cmdWriteReg;
	}
	else if (!strcmp(cmd, "mb_read_coil")) {
		arg_min = 2;
		arg_max = 3;
		arg_value[1] = 16;
		fcn = &ShellDevice::cmdReadCoil;
	}
	else if (!strcmp(cmd, "mb_write_coil")) {
		arg_min = 3;
		arg_max = 3;
		fcn = &ShellDevice::cmdWriteCoil;
	} else {
		lprintf("Unknown command %s\n", cmd);
		return;
	}

	int args = 0;
	char *tok = cmd;
	while(tok && *tok) {
		tok = strtok(NULL, " \t\r\n");
		if (!tok) break;
		const char *endptr;
		arg_value[args] = strtol(tok, &endptr, 0);
		if (*endptr) break;
		++args;
	}

	if ((args < arg_min) || (args > arg_max))
	{
		lprintf("Bad args %i (not in %i-%i)\n", args, arg_min, arg_max);
		return;
	}

	fcn(arg_value);
}

void shell(char *msg)
{
	enum { cmdNone, cmdRead, cmdSet, cmdReset, cmdInvert, cmdReboot } cmd;

	char *tok = strtok(msg, " \t\r\n");

	switch (*tok) {
		case 'V':
			lprintf("Date: " BUILD_DATE "\nTime: " BUILD_TIME "\nFw rev: " BUILD_TAG "\n");
			return;

		case '\0':
			printTable(DIGIN_SIGS);
			printTable(DIGOUT_SIGS);
			printTable(RLYOUT_SIGS);
			return;

		case '+':
			cmd = cmdSet;
			++tok;
			break;

		case '-':
			cmd = cmdReset;
			++tok;
			break;

		case '*':
			cmd = cmdInvert;
			++tok;
			break;

		case '@':
			cmd = cmdReboot;
			break;

		case 'D':
		case 'R':
		case 'I':
		case 'A':
			cmd = cmdRead;
			break;

		default:
			parseLongCommand(tok);
			return;
	}

	const struct SignalDesc *sig = NULL;
	switch (cmd) {
		default:
			break;

		case cmdRead:
		case cmdSet:
		case cmdReset:
		case cmdInvert:
			sig = findSig(tok);
			if (!sig) return;
			break;
	}

	switch (cmd) {
		case cmdRead:
			printSig(sig);
			break;

		case cmdSet:
			digitalWrite(sig->pin, HIGH);
			printSig(sig);
			break;

		case cmdReset:
			digitalWrite(sig->pin, LOW);
			printSig(sig);
			break;

		case cmdInvert:
			digitalWrite(sig->pin, digitalRead(sig->pin) ? LOW : HIGH);
			printSig(sig);
			break;

		case cmdReboot:
			wdt_disable();
			wdt_enable(WDTO_2S);
			while(1);
			break;
	}
}
