#include "pins.h"
#include "shell.h"

#include <Arduino.h>
#include <stdio.h>

int printTable(char *buf, const struct SignalDesc *table)
{
	char *p = buf;

	for (const struct SignalDesc *sig = table; sig->name; sig++) {
		p+=sprintf(p, "%4s", sig->name);
	}
	p+=sprintf(p, "\n");

	for (const struct SignalDesc *sig = table; sig->name; sig++) {
		p+=sprintf(p, "%4i", (digitalRead(sig->pin) == HIGH) ? 1 : 0);
	}
	p+=sprintf(p, "\n");

	return p-buf;
}

int printSig(char *buf, const struct SignalDesc *sig)
{
	return sprintf(buf, "%4s\n%4i\n", sig->name, (digitalRead(sig->pin) == HIGH) ? 1 : 0);
}

static void strip(char *msg) {
	char *i, *o;
	for (i=msg, o=msg; *i; ++i) {
		if (*i<33 || *i>127)
			continue;
		else *o++=*i;
	}
	*o = '\0';
}

void shell(struct ShellState *s)
{
	s->reply[0] = '\0';
	char *i = s->cmd;
	char *p = s->reply;

	enum { cmdNone, cmdRead, cmdSet, cmdReset } cmd;

	strip(s->cmd);
	Serial.println(s->cmd);

	switch (*i) {
		case '\0':
			p += printTable(p, DIGIN_SIGS);
			p += printTable(p, DIGOUT_SIGS);
			p += printTable(p, RLYOUT_SIGS);
			return;

		case '+':
			cmd = cmdSet;
			++i;
			break;

		case '-':
			cmd = cmdReset;
			++i;
			break;

		case 'D':
		case 'R':
		case 'I':
		case 'A':
			cmd = cmdRead;
			break;

		default:
			return;
	}

	const struct SignalDesc *sig = NULL;
	switch (cmd) {
		case cmdNone:
			break;

		case cmdRead:
		case cmdSet:
		case cmdReset:
			sig = findSig(i);
			if (!sig) return;
			break;
	}

	switch (cmd) {
		case cmdRead:
			p += printSig(p, sig);
			break;

		case cmdSet:
			digitalWrite(sig->pin, HIGH);
			p += printSig(p, sig);
			break;

		case cmdReset:
			digitalWrite(sig->pin, LOW);
			p += printSig(p, sig);
			break;
	}
}
