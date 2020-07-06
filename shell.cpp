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

static void strip(char *msg) {
	char *i, *o;
	for (i=msg, o=msg; *i; ++i) {
		if (*i<33 || *i>127)
			continue;
		else *o++=*i;
	}
	*o = '\0';
}

void shell(char *msg)
{
	char *i = msg;

	enum { cmdNone, cmdRead, cmdSet, cmdReset, cmdInvert, cmdReboot } cmd;

	strip(msg);

	switch (*i) {
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
			++i;
			break;

		case '-':
			cmd = cmdReset;
			++i;
			break;

		case '*':
			cmd = cmdInvert;
			++i;
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
			sig = findSig(i);
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
