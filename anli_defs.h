#ifndef __ANLI_DEFS_H__
#define __ANLI_DEFS_H__

enum ANLI_COILS {
	ANLI_COIL_POWER_CMD = 0,
	ANLI_COIL_HEAT_CMD = 1,
	ANLI_COIL_DHW_CMD = 2,
	ANLI_COIL_REMOTE_THERM_ENABLE = 3,
	ANLI_COIL_ALARM_RESET_CMD = 4,
	ANLI_COIL_COMPR1_STAT = 5,
	ANLI_COIL_COMPR2_STAT = 6,
	ANLI_COIL_BOILER_STAT = 7,
	ANLI_COIL_DHW_STAT = 8,
	ANLI_COIL_ALARM_GEN = 13,
	ANLI_COIL_PREALARM_GEN = 14,
	ANLI_COIL_PREALARM_MIN = 15,
	ANLI_COIL_PREALARM_MAX = 70,
	ANLI_COIL_ALARM_MIN = 71,
	ANLI_COIL_ALARM_MAX = 142,
	// -- end of list
	ANLI_COIL_COUNT,
};

enum ANLI_REGS {
	ANLI_REG_MACH_TYPE = 0,		// Type of machine: 0 – ANL,1 – ANLI, 2 – ANR/ANF/SRP, 3 – ANL-C
	ANLI_REG_INV_TYPE = 1,		// Type of inverter machine 0=longertek 1=APY
	ANLI_REG_NTC1_VAL = 2,		// NTC1 input (TUA) (SIW)
	ANLI_REG_NTC2_VAL = 3,		// NTC2 input (TIA) (SUW)
	ANLI_REG_NTC3_VAL = 4,		// NTC3 input (TSB) (SS)
	ANLI_REG_NTC4_VAL = 5,		// NTC4 input (TGP) (SGP)
	ANLI_REG_NTC5_VAL = 6,		// NTC5 input (TAE) (SAE)
	ANLI_REG_HIGH_PRESS_VAL = 7,	// High pressure transducer input (AP) (TAP)
	ANLI_REG_LOW_PRESS_VAL = 8,	// Low pressure transducer input (BP) (TBP)
	ANLI_REG_ANALOG_IN_VAL = 9,	// 0-10V dd input
	ANLI_REG_DIG_OUT_VAL = 10,	// Relay digital output status
	ANLI_REG_SAFETY_FORCE = 11,	// Safety band on force OFF
	ANLI_REG_COMPR_TIMER = 12,	// Time until compressor start-up/switch-off
	ANLI_REG_PRIMARY_KHOURS = 13,	// Hours of primary compressor operation (in thousands)
	ANLI_REG_PRIMARY_HOURS = 14,	// Hours of primary compressor operation
	ANLI_REG_PRIMARY_KPEAKS = 15,	// Number of peaks made by the compressor (in thousands)
	ANLI_REG_PRIMARY_PEAKS = 16,	// Number of peaks made by the compressor
	ANLI_REG_SW_VER_MAJ = 17,	// Major sw version
	ANLI_REG_SW_VER_MIN = 18,	// Minor sw version
	ANLI_REG_SETPT_ADJUST = 19,	// Machine adjustment setpoint (including corrections)
	ANLI_REG_PRESS_SETPT = 20,	// Condensation control pressure setpoint
	ANLI_REG_PRESS_DIFF = 21,	// Pressure differential for condensation control
	ANLI_REG_AUX_KHOURS = 22,	// Hours of auxiliary compressor operation (in thousands)
	ANLI_REG_AUX_HOURS = 23,	// Hours of auxiliary compressor operation
	ANLI_REG_AUX_KPEAKS = 24,	// Number of peaks made by the auxiliary compressor (in thousands)
	ANLI_REG_AUX_PEAKS = 25,	// Number of peaks made by the auxiliary compressor
	ANLI_REG_PWM_VALUE = 26,	// Power supplied by the machine (chiller ON-OFF) Use frequency (chiller inverter)
	ANLI_REG_PRESS_DROP_VALUE = 27,	// Compressor pressure drop during suction
	ANLI_REG_INVERTER_POWER = 28,	// Power requested from the inverter control
	ANLI_REG_DIP_SWTICH_VAL = 29,	// Dip-switch configuration (0x0000 = all OFF - 0x0FF = all ON)
	ANLI_REG_ONOFF_EN_STATUS = 30,	// ON_OFF command enablement status Season from panel or remote panel
	ANLI_REG_THERM_EN_STATUS = 31,	// Enablement status for room thermostat connected to ID3 input
	ANLI_REG_STATE = 32,		// Machine operation status: 0 - Chiller Off, 1 - Chiller On, 2 – Capacity control injection, 3 – Defrosting for injection, 4 – Defrosting for reverse cycle
	ANLI_REG_HEATER_TYPE = 33,	// Electric heater/boiler accessory: 0=absent 1=electric heater 2=boiler
	ANLI_REG_APY_CURRENT_VAL = 34,	// Inverter current APY
	ANLI_REG_APY_OUT_VOLT_VAL = 35,	// Inverter output voltage APY
	ANLI_REG_APY_BUS_VOLT_VAL = 36,	// Inverter BUS voltage APY
	ANLI_REG_APY_TEMP_VAL = 37,	// Inverter cooling fin temperature APY
	ANLI_REG_MAX_HOT_SETPT = 38,	// Limit set point hot
	ANLI_REG_RW_COOL_SETPT = 39,	// [RW] Cooling setpoint
	ANLI_REG_RW_COOL_BAND = 40,	// [RW] Cooling setpoint band
	ANLI_REG_RW_HEAT_SETPT = 41,	// [RW] Heating setpoint
	ANLI_REG_RW_HEAT_BAND = 42,	// [RW] Heating setpoint band
	ANLI_REG_RW_SETPT_ADJ = 43,	// [RW] Setpoint correction
	ANLI_REG_RW_COOL_SETPT1 = 44,	// [RW] Cooling setpoint 1
	ANLI_REG_RW_COOL_BAND1 = 45,	// [RW] External AT in cool mode 1
	ANLI_REG_RW_COOL_SETPT2 = 46,	// [RW] Cooling setpoint 2
	ANLI_REG_RW_COOL_BAND2 = 47,	// [RW] External AT in cool mode 2
	ANLI_REG_RW_HEAT_SETPT1 = 48,	// [RW] Heating setpoint 1
	ANLI_REG_RW_HEAT_BAND1 = 49,	// [RW] External AT in heat mode 1
	ANLI_REG_RW_HEAT_SETPT2 = 50,	// [RW] Heating setpoint 2
	ANLI_REG_RW_HEAT_BAND2 = 51,	// [RW] External AT in heat mode 2
	ANLI_REG_RW_DHW_SETPT = 52,	// [RW] Domestic water setpoint
	ANLI_REG_RW_DHW_BAND = 53,	// [RW] Domestic water band
	ANLI_REG_RW_POWER_LIMIT = 54,	// [RW] Power percentage requested from the thermostat
	ANLI_REG_RW_PWD_SET_VMF = 55,	// [RW] PWD_SET_VMF
	ANLI_REG_RW_PROBE_DHW = 56,	// [RW] Remote probe DHW
	// -- end of list
	ANLI_REG_COUNT
};

extern const char * const ANLI_COIL_MSGS[ANLI_COIL_COUNT];
extern const char * const ANLI_REG_MSGS[ANLI_REG_COUNT];

#endif /* __ANLI_DEFS_H__ */
