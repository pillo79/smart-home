#include "anli_defs.h"

const char * const ANLI_COIL_MSGS[ANLI_COIL_COUNT] = {
	[   0] = "Standby/On command (toggle) 0=OFF 1=ON",
	[   1] = "Operation mode command (toggle) 0=COOL 1=HEAT",
	[   2] = "Domestic water remote command 0=Normal, 1=Domestic",
	[   3] = "Enable remote thermostat 0=Disabled, 1=Enabled",
	[   4] = "Alarm reset command 1=Alarm reset",
	[   5] = "Compressor1 status 0=OFF 1=ON",
	[   6] = "Compressor2 status 0=OFF 1=ON",
	[   7] = "Boiler/electric heater status 0=OFF 1=ON",
	[   8] = "Hot domestic water production status 0=not active 1=active",
	[   9] = "Domestic water ID status 0=ID closed – 1=ID open",
	[  10] = "Remote ON_OFF ID status 0=ID closed – 1=ID open",
	[  11] = "Season ID status 0=ID closed (cool), 1=ID open (heat)",
	[  12] = "Room thermostat ID status 0=ID closed – 1=ID open",
	[  13] = "Alarm summary",
	[  14] = "Pre-alarm summary",
	[  15] = "Compressor thermomagnetic switch pre-alarm",
	[  16] = "Fan thermomagnetic switch pre-alarm",
	[  17] = "High pressure switch pre-alarm",
	[  18] = "Flow switch/differential pressure switch (water) pre-alarm",
	[  19] = "Low pressure switch pre-alarm",
	[  20] = "Pre-alarm for absence of NTC1 input probe (SIW)",
	[  21] = "Pre-alarm for absence of NTC2 input probe (SUW)",
	[  22] = "Anti-freeze pre-alarm",
	[  23] = "Pre-alarm for absence of NTC4 input probe (SGP)",
	[  24] = "Pre-alarm for high discharge gas temperature",
	[  25] = "Pre-alarm for absence of compressor delivery transducer",
	[  26] = "High pressure pre-alarm",
	[  27] = "Pre-alarm for absence of NTC3 input probe (SS)",
	[  28] = "Pre-alarm for absence of suction transducer",
	[  29] = "Low pressure pre-alarm",
	[  30] = "Low output pre-alarm",
	[  31] = "Pump thermomagnetic switch pre-alarm",
	[  32] = "High pressure capacity control pre-alarm",
	[  33] = "Low pressure capacity control pre-alarm",
	[  34] = "Discharge temperature capacity control pre-alarm",
	[  35] = "Bemf error pre-alarm (longertek inverter)",
	[  36] = "Pre-alarm for internal communication error (longertek)",
	[  37] = "Overcurrent pre-alarm (longertek)",
	[  38] = "Pre-alarm for absence of load (longertek)",
	[  39] = "Pre-alarm for incorrect voltage (longertek)",
	[  40] = "Pre-alarm for start-up error (longertek)",
	[  41] = "Pre-alarm for IPM protection error (longertek)",
	[  42] = "Pre-alarm for EEPROM error (longertek)",
	[  43] = "Pre-alarm for compressor stalling (longertek)",
	[  44] = "Pre-alarm for absence of communication (longertek)",
	[  45] = "PFC module pre-alarm (longertek)",
	[  46] = "Pre-alarm for cooling over-temperature (APY)",
	[  47] = "Pre-alarm for overcurrent in acceleration (APY)",
	[  48] = "Pre-alarm for overcurrent at steady speed (APY)",
	[  49] = "Pre-alarm for overcurrent in deceleration (APY)",
	[  50] = "Pre-alarm for undervoltage on BUS DC (APY)",
	[  51] = "Pre-alarm for overvoltage on BUS DC (APY)",
	[  54] = "Pre-alarm for converter PFC fault (APY)",
	[  55] = "Pre-alarm for overcurrent in acceleration (APY)",
	[  56] = "Overload pre-alarm (APY)",
	[  57] = "Pre-alarm for overcurrent at steady speed (APY)",
	[  58] = "Pre-alarm for overcurrent in deceleration (APY)",
	[  59] = "Pre-alarm for incorrectly connected compressor (APY)",
	[  60] = "Pre-alarm for absence of communication (APY)",
	[  61] = "Pre-alarm for cooling fin temperature sensor error (APY)",
	[  65] = "Fault condition pre-alarm (APY)",
	[  68] = "Reverse cycle valve pre-alarm",
	[  69] = "Pre-alarm for high water inlet temperature",
	[  70] = "Pre-alarm for reverse cycle due to high discharge gas temperature",
	[  85] = "Compressor thermomagnetic switch alarm",
	[  86] = "Fan thermomagnetic switch alarm",
	[  87] = "High pressure switch alarm",
	[  88] = "Flow switch/differential pressure switch (water) alarm",
	[  89] = "Low pressure switch alarm",
	[  90] = "Alarm for absence of NTC1 input probe (SIW)",
	[  91] = "Alarm for absence of NTC2 input probe (SUW)",
	[  92] = "Anti-freeze alarm",
	[  93] = "Alarm for absence of NTC4 input probe (SGP)",
	[  94] = "Alarm for high discharge gas temperature",
	[  95] = "Alarm for absence of compressor delivery transducer",
	[  96] = "High pressure alarm",
	[  97] = "Alarm for absence of NTC3 input probe (SS)",
	[  98] = "Alarm for absence of suction transducer",
	[  99] = "Low pressure alarm",
	[ 100] = "Low output alarm",
	[ 101] = "Pump thermomagnetic switch alarm",
	[ 102] = "High pressure capacity control alarm",
	[ 103] = "Low pressure capacity control alarm",
	[ 104] = "Discharge temperature capacity control alarm",
	[ 105] = "Bemf error alarm (longertek inverter)",
	[ 106] = "Alarm for internal communication error (longertek)",
	[ 107] = "Overcurrent alarm (longertek)",
	[ 108] = "Alarm for absence of load (longertek)",
	[ 109] = "Alarm for incorrect voltage (longertek)",
	[ 110] = "Alarm for start-up error (longertek)",
	[ 111] = "Alarm for IPM protection error (longertek)",
	[ 112] = "Alarm for EEPROM error (longertek)",
	[ 113] = "Alarm for compressor stalling (longertek)",
	[ 114] = "Alarm for absence of communication (longertek)",
	[ 115] = "PFC module alarm (longertek)",
	[ 116] = "Alarm for cooling over-temperature (APY)",
	[ 117] = "Alarm for overcurrent in acceleration (APY)",
	[ 118] = "Alarm for overcurrent at steady speed (APY)",
	[ 119] = "Alarm for overcurrent in deceleration (APY)",
	[ 120] = "Alarm for undervoltage on BUS DC (APY)",
	[ 121] = "Overvoltage alarm (APY)",
	[ 124] = "Alarm for converter PFC fault (APY)",
	[ 125] = "Alarm for overcurrent in acceleration (APY)",
	[ 126] = "Overload alarm (APY)",
	[ 127] = "Alarm for overcurrent at steady speed (APY)",
	[ 128] = "Alarm for overcurrent in deceleration (APY)",
	[ 129] = "Alarm for incorrectly connected compressor (APY)",
	[ 130] = "Alarm for absence of communication (APY)",
	[ 131] = "Alarm for cooling fin temperature sensor error (APY)",
	[ 135] = "Fault condition alarm (APY)",
	[ 138] = "Alarm for faulty reverse cycle valve",
	[ 139] = "Alarm for high water inlet temperature",
	[ 140] = "Alarm for reverse cycle due to high discharge gas temperature",
	[ 141] = "(Code 57) Alarm remote reading of the probe within DHW",
	[ 142] = "(Code 58) Alarm reading of temperature sensor outside air",
};

const char * const ANLI_REG_MSGS[ANLI_REG_COUNT] = {
	[   0] = "Type of machine: 0 – ANL,1 – ANLI, 2 – ANR/ANF/SRP, 3 – ANL-C",
	[   1] = "Type of inverter machine 0=longertek 1=APY",
	[   2] = "NTC1 input (TUA) (SIW)",
	[   3] = "NTC2 input (TIA) (SUW)",
	[   4] = "NTC3 input (TSB) (SS)",
	[   5] = "NTC4 input (TGP) (SGP)",
	[   6] = "NTC5 input (TAE) (SAE)",
	[   7] = "High pressure transducer input (AP) (TAP)",
	[   8] = "Low pressure transducer input (BP) (TBP)",
	[   9] = "0-10V dd input",
	[  10] = "Relay digital output status",
	[  11] = "Safety band on force OFF",
	[  12] = "Time until compressor start-up/switch-off",
	[  13] = "Hours of primary compressor operation (in thousands)",
	[  14] = "Hours of primary compressor operation",
	[  15] = "Number of peaks made by the compressor (in thousands)",
	[  16] = "Number of peaks made by the compressor",
	[  17] = "Major sw version",
	[  18] = "Minor sw version",
	[  19] = "Machine adjustment setpoint (including corrections)",
	[  20] = "Condensation control pressure setpoint",
	[  21] = "Pressure differential for condensation control",
	[  22] = "Hours of auxiliary compressor operation (in thousands)",
	[  23] = "Hours of auxiliary compressor operation",
	[  24] = "Number of peaks made by the auxiliary compressor (in thousands)",
	[  25] = "Number of peaks made by the auxiliary compressor",
	[  26] = "Power supplied by the machine (chiller ON-OFF) Use frequency (chiller inverter)",
	[  27] = "Compressor pressure drop during suction",
	[  28] = "Power requested from the inverter control",
	[  29] = "Dip-switch configuration (0x0000 = all OFF - 0x0FF = all ON)",
	[  30] = "ON_OFF command enablement status Season from panel or remote panel",
	[  31] = "Enablement status for room thermostat connected to ID3 input",
	[  32] = "Machine operation status: 0 - Chiller Off, 1 - Chiller On, 2 – Capacity control injection, 3 – Defrosting for injection, 4 – Defrosting for reverse cycle",
	[  33] = "Electric heater/boiler accessory: 0=absent 1=electric heater 2=boiler",
	[  34] = "Inverter current APY",
	[  35] = "Inverter output voltage APY",
	[  36] = "Inverter BUS voltage APY",
	[  37] = "Inverter cooling fin temperature APY",
	[  38] = "Limit set point hot",
	[  39] = "[RW] Cooling setpoint",
	[  40] = "[RW] Cooling setpoint band",
	[  41] = "[RW] Heating setpoint",
	[  42] = "[RW] Heating setpoint band",
	[  43] = "[RW] Setpoint correction",
	[  44] = "[RW] Cooling setpoint 1",
	[  45] = "[RW] External AT in cool mode 1",
	[  46] = "[RW] Cooling setpoint 2",
	[  47] = "[RW] External AT in cool mode 2",
	[  48] = "[RW] Heating setpoint 1",
	[  49] = "[RW] External AT in heat mode 1",
	[  50] = "[RW] Heating setpoint 2",
	[  51] = "[RW] External AT in heat mode 2",
	[  52] = "[RW] Domestic water setpoint",
	[  53] = "[RW] Domestic water band",
	[  54] = "[RW] Power percentage requested from the thermostat",
	[  55] = "[RW] PWD_SET_VMF",
	[  56] = "[RW] Remote probe DHW",
};