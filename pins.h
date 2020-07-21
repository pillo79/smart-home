#ifndef __PINS_H__
#define __PINS_H__

struct SignalDesc {
	const char *name;
	int pin;
//	int (*rd)(int pin);
//	int (*wr)(int pin, int val);
};

extern const struct SignalDesc DIGOUT_SIGS[];
extern const struct SignalDesc RLYOUT_SIGS[];
extern const struct SignalDesc DIGIN_SIGS[];

void pinSetup();
const struct SignalDesc *findSig(const char *name, const struct SignalDesc *table);
const struct SignalDesc *findSig(const char *name);

#endif // __PINS_H__
