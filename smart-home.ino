#include "control.h"
#include "network.h"
#include "log.h"
#include "pins.h"

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
