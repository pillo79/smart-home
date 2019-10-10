#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "logicdevice.h"
#include "logicio.h"

class HW {
	public:
		static BitOutput outBoilerHeatRequest;
		static BitInput inTempLow;

		static void updateInputs();
		static void updateOutputs();
};

#endif /* __HARDWARE_H__ */
