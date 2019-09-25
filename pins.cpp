#include "pins.h"

#include <Controllino.h>

#define PIN_TO_DESC(x) { #x, CONTROLLINO_ ## x }

const SignalDesc DIGOUT_SIGS[] = {
	PIN_TO_DESC(D0),
	PIN_TO_DESC(D1),
	PIN_TO_DESC(D2),
	PIN_TO_DESC(D3),
	PIN_TO_DESC(D4),
	PIN_TO_DESC(D5),
	PIN_TO_DESC(D6),
	PIN_TO_DESC(D7),
	PIN_TO_DESC(D8),
	PIN_TO_DESC(D9),
	PIN_TO_DESC(D10),
	PIN_TO_DESC(D11),
	{ NULL, 0 }
};

const SignalDesc RLYOUT_SIGS[] = {
	PIN_TO_DESC(R0),
	PIN_TO_DESC(R1),
	PIN_TO_DESC(R2),
	PIN_TO_DESC(R3),
	PIN_TO_DESC(R4),
	PIN_TO_DESC(R5),
	PIN_TO_DESC(R6),
	PIN_TO_DESC(R7),
	PIN_TO_DESC(R8),
	PIN_TO_DESC(R9),
	{ NULL, 0 }
};


const SignalDesc DIGIN_SIGS[] = {
	PIN_TO_DESC(A0),
	PIN_TO_DESC(A1),
	PIN_TO_DESC(A2),
	PIN_TO_DESC(A3),
	PIN_TO_DESC(A4),
	PIN_TO_DESC(A5),
	PIN_TO_DESC(A6),
	PIN_TO_DESC(A7),
	PIN_TO_DESC(A8),
	PIN_TO_DESC(A9),
	PIN_TO_DESC(IN0),
	PIN_TO_DESC(IN1),
	{ NULL, 0 }
};

const struct SignalDesc *findSig(const char *name, const struct SignalDesc *table)
{
	for (const struct SignalDesc *sig = table; sig->name; sig++)
		if (!strcmp(name, sig->name))
			return sig;

	return NULL;
}

const struct SignalDesc *findSig(const char *name)
{
	const struct SignalDesc *sig = findSig(name, DIGIN_SIGS);
	if (!sig) sig = findSig(name, DIGOUT_SIGS);
	if (!sig) sig = findSig(name, RLYOUT_SIGS);

	return sig;
}


void pinSetup()
{
	pinMode(CONTROLLINO_D0, OUTPUT);
	pinMode(CONTROLLINO_D1, OUTPUT);
	pinMode(CONTROLLINO_D2, OUTPUT);
	pinMode(CONTROLLINO_D3, OUTPUT);
	pinMode(CONTROLLINO_D4, OUTPUT);
	pinMode(CONTROLLINO_D5, OUTPUT);
	pinMode(CONTROLLINO_D6, OUTPUT);
	pinMode(CONTROLLINO_D7, OUTPUT);
	pinMode(CONTROLLINO_D8, OUTPUT);
	pinMode(CONTROLLINO_D9, OUTPUT);
	pinMode(CONTROLLINO_D10, OUTPUT);
	pinMode(CONTROLLINO_D11, OUTPUT);

	pinMode(CONTROLLINO_R0, OUTPUT);
	pinMode(CONTROLLINO_R1, OUTPUT);
	pinMode(CONTROLLINO_R2, OUTPUT);
	pinMode(CONTROLLINO_R3, OUTPUT);
	pinMode(CONTROLLINO_R4, OUTPUT);
	pinMode(CONTROLLINO_R5, OUTPUT);
	pinMode(CONTROLLINO_R6, OUTPUT);
	pinMode(CONTROLLINO_R7, OUTPUT);
	pinMode(CONTROLLINO_R8, OUTPUT);
	pinMode(CONTROLLINO_R9, OUTPUT);
}
