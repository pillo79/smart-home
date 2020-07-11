#ifndef __MODBUSRTU_H__
#define __MODBUSRTU_H__

/**
 * @file 	ModbusRTU.h
 * @version     1.21 (modified by CONTROLLINO team)
 * @date        2016.02.21 (2017.03.30)
 * @author 	Samuel Marco i Armengol (Thank you, Samuel!)
 * @contact     sammarcoarmengol@gmail.com
 * @contribution Helium6072
 * @contribution gabrielsan
 *
 * @description
 *  Arduino library for communicating with Modbus devices
 *  over RS232/USB/485 via RTU protocol.
 *
 *  Further information:
 *  http://modbus.org/
 *  http://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
 *
 * @license
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; version
 *  2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @defgroup setup Modbus Object Instantiation/Initialization
 * @defgroup loop Modbus Object Management
 * @defgroup buffer Modbus Buffer Management
 * @defgroup discrete Modbus Function Codes for Discrete Coils/Inputs
 * @defgroup register Modbus Function Codes for Holding/Input Registers
 *
 */

#include <inttypes.h>

/**
 * @struct modbus_msg_t
 * @brief
 * Master query structure:
 * This includes all the necessary fields to make the Master generate a Modbus query.
 * A Master may keep several of these structures and send them cyclically or
 * use them according to program needs.
 */
typedef struct
{
	uint8_t u8id;          /*!< Slave address between 1 and 247. 0 means broadcast */
	uint8_t u8fct;         /*!< Function code: 1, 2, 3, 4, 5, 6, 15 or 16 */
	uint16_t u16RegAdd;    /*!< Address of the first register to access at slave/s */
	uint16_t u16CoilsNo;   /*!< Number of coils or registers to access */
	uint16_t *au16reg;     /*!< Pointer to memory image in master */
} modbus_msg_t;

/**
 * @enum MB_FC
 * @brief
 * Modbus function codes summary.
 * These are the implement function codes either for Master or for Slave.
 *
 * @see also fctsupported
 * @see also modbus_msg_t
 */
enum MB_FC
{
	MB_FC_NONE                     = 0,   /*!< null operator */
	MB_FC_READ_COILS               = 1,	/*!< FCT=1 -> read coils or digital outputs */
	MB_FC_READ_DISCRETE_INPUT      = 2,	/*!< FCT=2 -> read digital inputs */
	MB_FC_READ_REGISTERS           = 3,	/*!< FCT=3 -> read registers or analog outputs */
	MB_FC_READ_INPUT_REGISTER      = 4,	/*!< FCT=4 -> read analog inputs */
	MB_FC_WRITE_COIL               = 5,	/*!< FCT=5 -> write single coil or output */
	MB_FC_WRITE_REGISTER           = 6,	/*!< FCT=6 -> write single register */
	MB_FC_WRITE_MULTIPLE_COILS     = 15,	/*!< FCT=15 -> write multiple coils or outputs */
	MB_FC_WRITE_MULTIPLE_REGISTERS = 16	/*!< FCT=16 -> write multiple registers */
};

enum MB_COM_STATES
{
	MB_COM_IDLE                     = 0,
	MB_COM_WAITING                  = 1

};

enum MB_ERR_LIST
{
	MB_ERR_NOT_MASTER                = -1,
	MB_ERR_POLLING                   = -2,
	MB_ERR_BUFF_OVERFLOW             = -3,
	MB_ERR_BAD_CRC                   = -4,
	MB_ERR_EXCEPTION                 = -5
};

enum
{
	MB_NO_REPLY = 255,
	MB_EXC_FUNC_CODE = 1,
	MB_EXC_ADDR_RANGE = 2,
	MB_EXC_REGS_QUANT = 3,
	MB_EXC_EXECUTE = 4
};

#define  MB_MAX_BUFFER  64	//!< maximum size for the communication buffer in bytes

class HardwareSerial;

/**
 * @class ModbusRTU
 * @brief
 * Arduino class library for communicating with Modbus devices over
 * USB/RS232/485 (via RTU protocol).
 */
class ModbusRTU
{
	private:
		HardwareSerial *port; //!< Pointer to Serial class object
		uint8_t u8id; //!< 0=master, 1..247=slave number
		uint8_t u8state;
		uint8_t u8lastError;
		uint8_t au8Buffer[MB_MAX_BUFFER];
		uint8_t u8BufferSize;
		uint8_t u8lastRec;
		uint16_t *au16regs;
		uint16_t u16InCnt, u16OutCnt, u16errCnt;
		uint16_t u16timeOut;
		uint32_t u32time, u32timeOut, u32overTime;
		uint8_t u8regsize;

		void init(uint8_t u8id);
		void sendTxBuffer();
		int8_t getRxBuffer();
		uint16_t calcCRC(uint8_t u8length);
		uint8_t validateAnswer();
		uint8_t validateRequest();
		void get_FC1();
		void get_FC3();
		int8_t process_FC1( uint16_t *regs, uint8_t u8size );
		int8_t process_FC3( uint16_t *regs, uint8_t u8size );
		int8_t process_FC5( uint16_t *regs, uint8_t u8size );
		int8_t process_FC6( uint16_t *regs, uint8_t u8size );
		int8_t process_FC15( uint16_t *regs, uint8_t u8size );
		int8_t process_FC16( uint16_t *regs, uint8_t u8size );
		void buildException( uint8_t u8exception ); // build exception message

	public:
		ModbusRTU(uint8_t u8id);
		void begin(long u32speed);
		void begin(long u32speed, uint8_t u8config);
		void setTimeOut( uint16_t u16timeOut); //!<write communication watch-dog timer
		uint16_t getTimeOut(); //!<get communication watch-dog timer value
		bool getTimeOutState(); //!<get communication watch-dog timer state
		int8_t query( modbus_msg_t telegram ); //!<only for master
		int8_t poll(); //!<cyclic poll for master
		int8_t poll( uint16_t *regs, uint8_t u8size ); //!<cyclic poll for slave
		uint16_t getInCnt(); //!<number of incoming messages
		uint16_t getOutCnt(); //!<number of outcoming messages
		uint16_t getErrCnt(); //!<error counter
		uint8_t getID(); //!<get slave ID between 1 and 247
		uint8_t getState();
		uint8_t getLastError(); //!<get last error message
		void setID( uint8_t u8id ); //!<write new ID for the slave
		void setTxendPinOverTime( uint32_t u32overTime );
		void end(); //!<finish any communication and release serial communication port
};

#endif /* __MODBUSRTU_H__ */
