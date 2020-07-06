#include "control.h"
#include "network.h"
#include "pins.h"
#define TIME_DIFF(a,b)     ((int32_t)((uint32_t)(a) - (uint32_t)(b))

void setup()
{
	logSetup();
	pinSetup();
	netSetup();
	controlSetup();

	Serial.begin(9600);
	Serial.println("--- Device ready");
}

void loop()
{
	netLoop();
	controlLoop();
}
