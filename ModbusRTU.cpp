#include "ModbusRTU.h"

#include <Arduino.h>
#include <Print.h>
#include <SoftwareSerial.h>

#define RS485_PIN_DE      0b01000000
#define RS485_PIN_RE      0b00100000
#define RS485_DIR_REG     DDRJ
#define RS485_PORT_REG    PORTJ
#define RS485_CLEAR_DE    RS485_PORT_REG &= ~RS485_PIN_DE
#define RS485_CLEAR_RE    RS485_PORT_REG &= ~RS485_PIN_RE
#define RS485_SET_DE      RS485_PORT_REG |= RS485_PIN_DE
#define RS485_SET_RE      RS485_PORT_REG |= RS485_PIN_RE

enum
{
	RESPONSE_SIZE = 6,
	EXCEPTION_SIZE = 3,
	CHECKSUM_SIZE = 2
};

/**
 * @enum MESSAGE
 * @brief
 * Indexes to telegram frame positions
 */
enum MESSAGE
{
	ID                             = 0, //!< ID field
	FUNC, //!< Function code position
	ADD_HI, //!< Address high byte
	ADD_LO, //!< Address low byte
	NB_HI, //!< Number of coils or registers high byte
	NB_LO, //!< Number of coils or registers low byte
	BYTE_CNT  //!< byte counter
};

const unsigned char fctsupported[] =
{
	MB_FC_READ_COILS,
	MB_FC_READ_DISCRETE_INPUT,
	MB_FC_READ_REGISTERS,
	MB_FC_READ_INPUT_REGISTER,
	MB_FC_WRITE_COIL,
	MB_FC_WRITE_REGISTER,
	MB_FC_WRITE_MULTIPLE_COILS,
	MB_FC_WRITE_MULTIPLE_REGISTERS
};

#define T35  5

/* _____PUBLIC FUNCTIONS_____________________________________________________ */

/**
 * @brief
 * Full constructor for a Master/Slave through USB/RS232C
 *
 * @param u8id   node address 0=master, 1..247=slave
 * @ingroup setup
 */
ModbusRTU::ModbusRTU(uint8_t u8id)
{
	init(u8id);
}

/**
 * @brief
 * Initialize class object.
 *
 * Sets up the serial port using specified baud rate.
 * Call once class has been instantiated, typically within setup().
 *
 * @see http://arduino.cc/en/Serial/Begin#.Uy4CJ6aKlHY
 * @param speed   baud rate, in standard increments (300..115200)
 * @ingroup setup
 */
void ModbusRTU::begin(long u32speed)
{

	port = &Serial3;

	port->begin(u32speed);

	// return RS485 transceiver to transmit mode
	RS485_DIR_REG |= RS485_PIN_DE | RS485_PIN_RE;
	RS485_CLEAR_DE;
	RS485_CLEAR_RE;

	while(port->read() >= 0);
	u8lastRec = u8BufferSize = 0;
	u16InCnt = u16OutCnt = u16errCnt = 0;
}

/**
 * @brief
 * Initialize class object.
 *
 * Sets up the serial port using specified baud rate.
 * Call once class has been instantiated, typically within setup().
 *
 * @see http://arduino.cc/en/Serial/Begin#.Uy4CJ6aKlHY
 * @param speed   baud rate, in standard increments (300..115200)
 * @param config  data frame settings (data length, parity and stop bits)
 * @ingroup setup
 */
void ModbusRTU::begin(long u32speed,uint8_t u8config)
{

	port = &Serial3;

	port->begin(u32speed, u8config);
	// return RS485 transceiver to transmit mode
	RS485_DIR_REG |= RS485_PIN_DE | RS485_PIN_RE;
	RS485_CLEAR_DE;
	RS485_CLEAR_RE;

	while(port->read() >= 0);
	u8lastRec = u8BufferSize = 0;
	u16InCnt = u16OutCnt = u16errCnt = 0;
}

/**
 * @brief
 * Method to write a new slave ID address
 *
 * @param 	u8id	new slave address between 1 and 247
 * @ingroup setup
 */
void ModbusRTU::setID( uint8_t u8id)
{
	if (( u8id != 0) && (u8id <= 247))
	{
		this->u8id = u8id;
	}
}

/**
 * @brief
 * Method to write the overtime count for txend pin.
 * It waits until count reaches 0 after the transfer is done.
 * With this, you can extend the time between txempty and
 * the falling edge if needed.
 *
 * @param 	uint32_t	overtime count for txend pin
 * @ingroup setup
 */
void ModbusRTU::setTxendPinOverTime( uint32_t u32overTime )
{
	this->u32overTime = u32overTime;
}

/**
 * @brief
 * Method to read current slave ID address
 *
 * @return u8id	current slave address between 1 and 247
 * @ingroup setup
 */
uint8_t ModbusRTU::getID()
{
	return this->u8id;
}

/**
 * @brief
 * Initialize time-out parameter
 *
 * Call once class has been instantiated, typically within setup().
 * The time-out timer is reset each time that there is a successful communication
 * between Master and Slave. It works for both.
 *
 * @param time-out value (ms)
 * @ingroup setup
 */
void ModbusRTU::setTimeOut( uint16_t u16timeOut)
{
	this->u16timeOut = u16timeOut;
}

/**
 * @brief
 * Return communication Watchdog state.
 * It could be usefull to reset outputs if the watchdog is fired.
 *
 * @return TRUE if millis() > u32timeOut
 * @ingroup loop
 */
bool ModbusRTU::getTimeOutState()
{
	return ((unsigned long)(millis() -u32timeOut) > (unsigned long)u16timeOut);
}

/**
 * @brief
 * Get input messages counter value
 * This can be useful to diagnose communication
 *
 * @return input messages counter
 * @ingroup buffer
 */
uint16_t ModbusRTU::getInCnt()
{
	return u16InCnt;
}

/**
 * @brief
 * Get transmitted messages counter value
 * This can be useful to diagnose communication
 *
 * @return transmitted messages counter
 * @ingroup buffer
 */
uint16_t ModbusRTU::getOutCnt()
{
	return u16OutCnt;
}

/**
 * @brief
 * Get errors counter value
 * This can be useful to diagnose communication
 *
 * @return errors counter
 * @ingroup buffer
 */
uint16_t ModbusRTU::getErrCnt()
{
	return u16errCnt;
}

/**
 * Get modbus master state
 *
 * @return = 0 IDLE, = 1 WAITING FOR ANSWER
 * @ingroup buffer
 */
uint8_t ModbusRTU::getState()
{
	return u8state;
}

/**
 * Get the last error in the protocol processor
 *
 * @return   MB_NO_REPLY = 255      Time-out
 * @return   MB_EXC_FUNC_CODE = 1   Function code not available
 * @return   MB_EXC_ADDR_RANGE = 2  Address beyond available space for ModbusRTU registers
 * @return   MB_EXC_REGS_QUANT = 3  Coils or registers number beyond the available space
 * @ingroup buffer
 */
uint8_t ModbusRTU::getLastError()
{
	return u8lastError;
}

/**
 * @brief
 * *** Only ModbusRTU Master ***
 * Generate a query to an slave with a modbus_msg_t telegram structure
 * The Master must be in MB_COM_IDLE mode. After it, its state would be MB_COM_WAITING.
 * This method has to be called only in loop() section.
 *
 * @see modbus_msg_t
 * @param modbus_msg_t  modbus telegram structure (id, fct, ...)
 * @ingroup loop
 * @todo finish function 15
 */
int8_t ModbusRTU::query( modbus_msg_t telegram )
{
	uint8_t u8regsno, u8bytesno;
	if (u8id!=0) return -2;
	if (u8state != MB_COM_IDLE) return -1;

	if ((telegram.u8id==0) || (telegram.u8id>247)) return -3;

	au16regs = telegram.au16reg;

	// telegram header
	au8Buffer[ ID ]         = telegram.u8id;
	au8Buffer[ FUNC ]       = telegram.u8fct;
	au8Buffer[ ADD_HI ]     = highByte(telegram.u16RegAdd );
	au8Buffer[ ADD_LO ]     = lowByte( telegram.u16RegAdd );

	switch( telegram.u8fct )
	{
		case MB_FC_READ_COILS:
		case MB_FC_READ_DISCRETE_INPUT:
		case MB_FC_READ_REGISTERS:
		case MB_FC_READ_INPUT_REGISTER:
			au8Buffer[ NB_HI ]      = highByte(telegram.u16CoilsNo );
			au8Buffer[ NB_LO ]      = lowByte( telegram.u16CoilsNo );
			u8BufferSize = 6;
			break;
		case MB_FC_WRITE_COIL:
			au8Buffer[ NB_HI ]      = ((au16regs[0] > 0) ? 0xff : 0);
			au8Buffer[ NB_LO ]      = 0;
			u8BufferSize = 6;
			break;
		case MB_FC_WRITE_REGISTER:
			au8Buffer[ NB_HI ]      = highByte(au16regs[0]);
			au8Buffer[ NB_LO ]      = lowByte(au16regs[0]);
			u8BufferSize = 6;
			break;
		case MB_FC_WRITE_MULTIPLE_COILS: // TODO: implement "sending coils"
			u8regsno = telegram.u16CoilsNo / 16;
			u8bytesno = u8regsno * 2;
			if ((telegram.u16CoilsNo % 16) != 0)
			{
				u8bytesno++;
				u8regsno++;
			}

			au8Buffer[ NB_HI ]      = highByte(telegram.u16CoilsNo );
			au8Buffer[ NB_LO ]      = lowByte( telegram.u16CoilsNo );
			au8Buffer[ BYTE_CNT ]    = u8bytesno;
			u8BufferSize = 7;

			for (uint16_t i = 0; i < u8bytesno; i++)
			{
				if(i%2)
				{
					au8Buffer[ u8BufferSize ] = lowByte( au16regs[ i/2 ] );
				}
				else
				{
					au8Buffer[ u8BufferSize ] = highByte( au16regs[ i/2] );
				}
				u8BufferSize++;
			}
			break;

		case MB_FC_WRITE_MULTIPLE_REGISTERS:
			au8Buffer[ NB_HI ]      = highByte(telegram.u16CoilsNo );
			au8Buffer[ NB_LO ]      = lowByte( telegram.u16CoilsNo );
			au8Buffer[ BYTE_CNT ]    = (uint8_t) ( telegram.u16CoilsNo * 2 );
			u8BufferSize = 7;

			for (uint16_t i=0; i< telegram.u16CoilsNo; i++)
			{
				au8Buffer[ u8BufferSize ] = highByte( au16regs[ i ] );
				u8BufferSize++;
				au8Buffer[ u8BufferSize ] = lowByte( au16regs[ i ] );
				u8BufferSize++;
			}
			break;
	}

	sendTxBuffer();
	u8state = MB_COM_WAITING;
	u8lastError = 0;
	return 0;
}

/**
 * @brief *** Only for ModbusRTU Master ***
 * This method checks if there is any incoming answer if pending.
 * If there is no answer, it would change Master state to MB_COM_IDLE.
 * This method must be called only at loop section.
 * Avoid any delay() function.
 *
 * Any incoming data would be redirected to au16regs pointer,
 * as defined in its modbus_msg_t query telegram.
 *
 * @params	nothing
 * @return errors counter
 * @ingroup loop
 */
int8_t ModbusRTU::poll()
{
	// check if there is any incoming frame
	uint8_t u8current = port->available();

	if ((unsigned long)(millis() -u32timeOut) > (unsigned long)u16timeOut)
	{
		u8state = MB_COM_IDLE;
		u8lastError = MB_NO_REPLY;
		u16errCnt++;
		return 0;
	}

	if (u8current == 0) return 0;

	// check T35 after frame end or still no frame end
	if (u8current != u8lastRec)
	{
		u8lastRec = u8current;
		u32time = millis();
		return 0;
	}
	if ((unsigned long)(millis() -u32time) < (unsigned long)T35) return 0;

	// transfer Serial buffer frame to auBuffer
	u8lastRec = 0;
	int8_t i8state = getRxBuffer();
	if (i8state < 6) //7 was incorrect for functions 1 and 2 the smallest frame could be 6 bytes long
	{
		u8state = MB_COM_IDLE;
		u16errCnt++;
		return i8state;
	}

	// validate message: id, CRC, FCT, exception
	uint8_t u8exception = validateAnswer();
	if (u8exception != 0)
	{
		u8state = MB_COM_IDLE;
		return u8exception;
	}

	// process answer
	switch( au8Buffer[ FUNC ] )
	{
		case MB_FC_READ_COILS:
		case MB_FC_READ_DISCRETE_INPUT:
			// call get_FC1 to transfer the incoming message to au16regs buffer
			get_FC1( );
			break;
		case MB_FC_READ_INPUT_REGISTER:
		case MB_FC_READ_REGISTERS :
			// call get_FC3 to transfer the incoming message to au16regs buffer
			get_FC3( );
			break;
		case MB_FC_WRITE_COIL:
		case MB_FC_WRITE_REGISTER :
		case MB_FC_WRITE_MULTIPLE_COILS:
		case MB_FC_WRITE_MULTIPLE_REGISTERS :
			// nothing to do
			break;
		default:
			break;
	}
	u8state = MB_COM_IDLE;
	return u8BufferSize;
}

/**
 * @brief
 * *** Only for ModbusRTU Slave ***
 * This method checks if there is any incoming query
 * Afterwards, it would shoot a validation routine plus a register query
 * Avoid any delay() function !!!!
 * After a successful frame between the Master and the Slave, the time-out timer is reset.
 *
 * @param *regs  register table for communication exchange
 * @param u8size  size of the register table
 * @return 0 if no query, 1..4 if communication error, >4 if correct query processed
 * @ingroup loop
 */
int8_t ModbusRTU::poll( uint16_t *regs, uint8_t u8size )
{

	au16regs = regs;
	u8regsize = u8size;
	uint8_t u8current;

	// check if there is any incoming frame
	u8current = port->available();

	if (u8current == 0) return 0;

	// check T35 after frame end or still no frame end
	if (u8current != u8lastRec)
	{
		u8lastRec = u8current;
		u32time = millis();
		return 0;
	}
	if ((unsigned long)(millis() -u32time) < (unsigned long)T35) return 0;

	u8lastRec = 0;
	int8_t i8state = getRxBuffer();
	u8lastError = i8state;
	if (i8state < 7) return i8state;

	// check slave id
	if (au8Buffer[ ID ] != u8id) return 0;

	// validate message: CRC, FCT, address and size
	uint8_t u8exception = validateRequest();
	if (u8exception > 0)
	{
		if (u8exception != MB_NO_REPLY)
		{
			buildException( u8exception );
			sendTxBuffer();
		}
		u8lastError = u8exception;
		return u8exception;
	}

	u32timeOut = millis();
	u8lastError = 0;

	// process message
	switch( au8Buffer[ FUNC ] )
	{
		case MB_FC_READ_COILS:
		case MB_FC_READ_DISCRETE_INPUT:
			return process_FC1( regs, u8size );
			break;
		case MB_FC_READ_INPUT_REGISTER:
		case MB_FC_READ_REGISTERS :
			return process_FC3( regs, u8size );
			break;
		case MB_FC_WRITE_COIL:
			return process_FC5( regs, u8size );
			break;
		case MB_FC_WRITE_REGISTER :
			return process_FC6( regs, u8size );
			break;
		case MB_FC_WRITE_MULTIPLE_COILS:
			return process_FC15( regs, u8size );
			break;
		case MB_FC_WRITE_MULTIPLE_REGISTERS :
			return process_FC16( regs, u8size );
			break;
		default:
			break;
	}
	return i8state;
}

/* _____PRIVATE FUNCTIONS_____________________________________________________ */

void ModbusRTU::init(uint8_t u8id)
{
	this->u8id = u8id;
	this->u16timeOut = 1000;
	this->u32overTime = 0;
}

/**
 * @brief
 * This method moves Serial buffer data to the ModbusRTU au8Buffer.
 *
 * @return buffer size if OK, MB_ERR_BUFF_OVERFLOW if u8BufferSize >= MB_MAX_BUFFER
 * @ingroup buffer
 */
int8_t ModbusRTU::getRxBuffer()
{
	boolean bBuffOverflow = false;

	RS485_CLEAR_DE;
	RS485_CLEAR_RE;

	u8BufferSize = 0;
	while ( port->available() )
	{
		au8Buffer[ u8BufferSize ] = port->read();
		u8BufferSize ++;

		if (u8BufferSize >= MB_MAX_BUFFER) bBuffOverflow = true;
	}
	u16InCnt++;

	if (bBuffOverflow)
	{
		u16errCnt++;
		return MB_ERR_BUFF_OVERFLOW;
	}
	return u8BufferSize;
}

/**
 * @brief
 * This method transmits au8Buffer to Serial line.
 * The CRC is appended to the buffer before starting to send it.
 *
 * @param nothing
 * @return nothing
 * @ingroup buffer
 */
void ModbusRTU::sendTxBuffer()
{
	// append CRC to message
	uint16_t u16crc = calcCRC( u8BufferSize );
	au8Buffer[ u8BufferSize ] = u16crc >> 8;
	u8BufferSize++;
	au8Buffer[ u8BufferSize ] = u16crc & 0x00ff;
	u8BufferSize++;

	// set RS485 transceiver to transmit mode
	RS485_SET_DE;
	RS485_SET_RE;

	UCSR3A=UCSR3A |(1 << TXC3);


	// transfer buffer to serial line
	port->write( au8Buffer, u8BufferSize );

	// must wait transmission end before changing pin state
	while (!(UCSR3A & (1 << TXC3)));

	volatile uint32_t u32overTimeCountDown = u32overTime;
	while ( u32overTimeCountDown-- > 0);

	// return RS485 transceiver to receive mode
	RS485_CLEAR_DE;
	RS485_CLEAR_RE;

	while(port->read() >= 0);

	u8BufferSize = 0;

	// set time-out for master
	u32timeOut = millis();

	// increase message counter
	u16OutCnt++;
}

/**
 * @brief
 * This method calculates CRC
 *
 * @return uint16_t calculated CRC value for the message
 * @ingroup buffer
 */
uint16_t ModbusRTU::calcCRC(uint8_t u8length)
{
	unsigned int temp, temp2, flag;
	temp = 0xFFFF;
	for (unsigned char i = 0; i < u8length; i++)
	{
		temp = temp ^ au8Buffer[i];
		for (unsigned char j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp >>=1;
			if (flag)
				temp ^= 0xA001;
		}
	}
	// Reverse byte order.
	temp2 = temp >> 8;
	temp = (temp << 8) | temp2;
	temp &= 0xFFFF;
	// the returned value is already swapped
	// crcLo byte is first & crcHi byte is last
	return temp;
}

/**
 * @brief
 * This method validates slave incoming messages
 *
 * @return 0 if OK, EXCEPTION if anything fails
 * @ingroup buffer
 */
uint8_t ModbusRTU::validateRequest()
{
	// check message crc vs calculated crc
	uint16_t u16MsgCRC =
		((au8Buffer[u8BufferSize - 2] << 8)
		 | au8Buffer[u8BufferSize - 1]); // combine the crc Low & High bytes
	if ( calcCRC( u8BufferSize-2 ) != u16MsgCRC )
	{
		u16errCnt ++;
		return MB_NO_REPLY;
	}

	// check fct code
	boolean isSupported = false;
	for (uint8_t i = 0; i< sizeof( fctsupported ); i++)
	{
		if (fctsupported[i] == au8Buffer[FUNC])
		{
			isSupported = 1;
			break;
		}
	}
	if (!isSupported)
	{
		u16errCnt ++;
		return MB_EXC_FUNC_CODE;
	}

	// check start address & nb range
	uint16_t u16regs = 0;
	uint8_t u8regs;
	switch ( au8Buffer[ FUNC ] )
	{
		case MB_FC_READ_COILS:
		case MB_FC_READ_DISCRETE_INPUT:
		case MB_FC_WRITE_MULTIPLE_COILS:
			u16regs = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ]) / 16;
			u16regs += word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ]) /16;
			u8regs = (uint8_t) u16regs;
			if (u8regs > u8regsize) return MB_EXC_ADDR_RANGE;
			break;
		case MB_FC_WRITE_COIL:
			u16regs = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ]) / 16;
			u8regs = (uint8_t) u16regs;
			if (u8regs > u8regsize) return MB_EXC_ADDR_RANGE;
			break;
		case MB_FC_WRITE_REGISTER :
			u16regs = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ]);
			u8regs = (uint8_t) u16regs;
			if (u8regs > u8regsize) return MB_EXC_ADDR_RANGE;
			break;
		case MB_FC_READ_REGISTERS :
		case MB_FC_READ_INPUT_REGISTER :
		case MB_FC_WRITE_MULTIPLE_REGISTERS :
			u16regs = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ]);
			u16regs += word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ]);
			u8regs = (uint8_t) u16regs;
			if (u8regs > u8regsize) return MB_EXC_ADDR_RANGE;
			break;
	}
	return 0; // OK, no exception code thrown
}

/**
 * @brief
 * This method validates master incoming messages
 *
 * @return 0 if OK, EXCEPTION if anything fails
 * @ingroup buffer
 */
uint8_t ModbusRTU::validateAnswer()
{
	// check message crc vs calculated crc
	uint16_t u16MsgCRC =
		((au8Buffer[u8BufferSize - 2] << 8)
		 | au8Buffer[u8BufferSize - 1]); // combine the crc Low & High bytes
	if ( calcCRC( u8BufferSize-2 ) != u16MsgCRC )
	{
		u16errCnt ++;
		return MB_NO_REPLY;
	}

	// check exception
	if ((au8Buffer[ FUNC ] & 0x80) != 0)
	{
		u16errCnt ++;
		return MB_ERR_EXCEPTION;
	}

	// check fct code
	boolean isSupported = false;
	for (uint8_t i = 0; i< sizeof( fctsupported ); i++)
	{
		if (fctsupported[i] == au8Buffer[FUNC])
		{
			isSupported = 1;
			break;
		}
	}
	if (!isSupported)
	{
		u16errCnt ++;
		return MB_EXC_FUNC_CODE;
	}

	return 0; // OK, no exception code thrown
}

/**
 * @brief
 * This method builds an exception message
 *
 * @ingroup buffer
 */
void ModbusRTU::buildException( uint8_t u8exception )
{
	uint8_t u8func = au8Buffer[ FUNC ];  // get the original FUNC code

	au8Buffer[ ID ]      = u8id;
	au8Buffer[ FUNC ]    = u8func + 0x80;
	au8Buffer[ 2 ]       = u8exception;
	u8BufferSize         = EXCEPTION_SIZE;
}

/**
 * This method processes functions 1 & 2 (for master)
 * This method puts the slave answer into master data buffer
 *
 * @ingroup register
 * TODO: finish its implementation
 */
void ModbusRTU::get_FC1()
{
	uint8_t u8byte, i;
	u8byte = 3;
	for (i=0; i< au8Buffer[2]; i++) {

		if(i%2)
		{
			au16regs[i/2]= word(au8Buffer[i+u8byte], lowByte(au16regs[i/2]));
		}
		else
		{
			au16regs[i/2]= word(highByte(au16regs[i/2]), au8Buffer[i+u8byte]);
		}
	}
}

/**
 * This method processes functions 3 & 4 (for master)
 * This method puts the slave answer into master data buffer
 *
 * @ingroup register
 */
void ModbusRTU::get_FC3()
{
	uint8_t u8byte, i;
	u8byte = 3;

	for (i=0; i< au8Buffer[ 2 ] /2; i++)
	{
		au16regs[ i ] = word(
				au8Buffer[ u8byte ],
				au8Buffer[ u8byte +1 ]);
		u8byte += 2;
	}
}

/**
 * @brief
 * This method processes functions 1 & 2
 * This method reads a bit array and transfers it to the master
 *
 * @return u8BufferSize Response to master length
 * @ingroup discrete
 */
int8_t ModbusRTU::process_FC1( uint16_t *regs, uint8_t /*u8size*/ )
{
	uint8_t u8currentRegister, u8currentBit, u8bytesno, u8bitsno;
	uint8_t u8CopyBufferSize;
	uint16_t u16currentCoil, u16coil;

	// get the first and last coil from the message
	uint16_t u16StartCoil = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ] );
	uint16_t u16Coilno = word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ] );

	// put the number of bytes in the outcoming message
	u8bytesno = (uint8_t) (u16Coilno / 8);
	if (u16Coilno % 8 != 0) u8bytesno ++;
	au8Buffer[ ADD_HI ]  = u8bytesno;
	u8BufferSize         = ADD_LO;
	au8Buffer[ u8BufferSize + u8bytesno - 1 ] = 0;

	// read each coil from the register map and put its value inside the outcoming message
	u8bitsno = 0;

	for (u16currentCoil = 0; u16currentCoil < u16Coilno; u16currentCoil++)
	{
		u16coil = u16StartCoil + u16currentCoil;
		u8currentRegister = (uint8_t) (u16coil / 16);
		u8currentBit = (uint8_t) (u16coil % 16);

		bitWrite(
				au8Buffer[ u8BufferSize ],
				u8bitsno,
				bitRead( regs[ u8currentRegister ], u8currentBit ) );
		u8bitsno ++;

		if (u8bitsno > 7)
		{
			u8bitsno = 0;
			u8BufferSize++;
		}
	}

	// send outcoming message
	if (u16Coilno % 8 != 0) u8BufferSize ++;
	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();
	return u8CopyBufferSize;
}

/**
 * @brief
 * This method processes functions 3 & 4
 * This method reads a word array and transfers it to the master
 *
 * @return u8BufferSize Response to master length
 * @ingroup register
 */
int8_t ModbusRTU::process_FC3( uint16_t *regs, uint8_t /*u8size*/ )
{

	uint8_t u8StartAdd = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ] );
	uint8_t u8regsno = word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ] );
	uint8_t u8CopyBufferSize;
	uint8_t i;

	au8Buffer[ 2 ]       = u8regsno * 2;
	u8BufferSize         = 3;

	for (i = u8StartAdd; i < u8StartAdd + u8regsno; i++)
	{
		au8Buffer[ u8BufferSize ] = highByte(regs[i]);
		u8BufferSize++;
		au8Buffer[ u8BufferSize ] = lowByte(regs[i]);
		u8BufferSize++;
	}
	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();

	return u8CopyBufferSize;
}

/**
 * @brief
 * This method processes function 5
 * This method writes a value assigned by the master to a single bit
 *
 * @return u8BufferSize Response to master length
 * @ingroup discrete
 */
int8_t ModbusRTU::process_FC5( uint16_t *regs, uint8_t /*u8size*/ )
{
	uint8_t u8currentRegister, u8currentBit;
	uint8_t u8CopyBufferSize;
	uint16_t u16coil = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ] );

	// point to the register and its bit
	u8currentRegister = (uint8_t) (u16coil / 16);
	u8currentBit = (uint8_t) (u16coil % 16);

	// write to coil
	bitWrite(
			regs[ u8currentRegister ],
			u8currentBit,
			au8Buffer[ NB_HI ] == 0xff );


	// send answer to master
	u8BufferSize = 6;
	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();

	return u8CopyBufferSize;
}

/**
 * @brief
 * This method processes function 6
 * This method writes a value assigned by the master to a single word
 *
 * @return u8BufferSize Response to master length
 * @ingroup register
 */
int8_t ModbusRTU::process_FC6( uint16_t *regs, uint8_t /*u8size*/ )
{

	uint8_t u8add = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ] );
	uint8_t u8CopyBufferSize;
	uint16_t u16val = word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ] );

	regs[ u8add ] = u16val;

	// keep the same header
	u8BufferSize         = RESPONSE_SIZE;

	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();

	return u8CopyBufferSize;
}

/**
 * @brief
 * This method processes function 15
 * This method writes a bit array assigned by the master
 *
 * @return u8BufferSize Response to master length
 * @ingroup discrete
 */
int8_t ModbusRTU::process_FC15( uint16_t *regs, uint8_t /*u8size*/ )
{
	uint8_t u8currentRegister, u8currentBit, u8frameByte, u8bitsno;
	uint8_t u8CopyBufferSize;
	uint16_t u16currentCoil, u16coil;
	boolean bTemp;

	// get the first and last coil from the message
	uint16_t u16StartCoil = word( au8Buffer[ ADD_HI ], au8Buffer[ ADD_LO ] );
	uint16_t u16Coilno = word( au8Buffer[ NB_HI ], au8Buffer[ NB_LO ] );


	// read each coil from the register map and put its value inside the outcoming message
	u8bitsno = 0;
	u8frameByte = 7;
	for (u16currentCoil = 0; u16currentCoil < u16Coilno; u16currentCoil++)
	{

		u16coil = u16StartCoil + u16currentCoil;
		u8currentRegister = (uint8_t) (u16coil / 16);
		u8currentBit = (uint8_t) (u16coil % 16);

		bTemp = bitRead(
				au8Buffer[ u8frameByte ],
				u8bitsno );

		bitWrite(
				regs[ u8currentRegister ],
				u8currentBit,
				bTemp );

		u8bitsno ++;

		if (u8bitsno > 7)
		{
			u8bitsno = 0;
			u8frameByte++;
		}
	}

	// send outcoming message
	// it's just a copy of the incomping frame until 6th byte
	u8BufferSize         = 6;
	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();
	return u8CopyBufferSize;
}

/**
 * @brief
 * This method processes function 16
 * This method writes a word array assigned by the master
 *
 * @return u8BufferSize Response to master length
 * @ingroup register
 */
int8_t ModbusRTU::process_FC16( uint16_t *regs, uint8_t /*u8size*/ )
{
	uint8_t u8StartAdd = au8Buffer[ ADD_HI ] << 8 | au8Buffer[ ADD_LO ];
	uint8_t u8regsno = au8Buffer[ NB_HI ] << 8 | au8Buffer[ NB_LO ];
	uint8_t u8CopyBufferSize;
	uint8_t i;
	uint16_t temp;

	// build header
	au8Buffer[ NB_HI ]   = 0;
	au8Buffer[ NB_LO ]   = u8regsno;
	u8BufferSize         = RESPONSE_SIZE;

	// write registers
	for (i = 0; i < u8regsno; i++)
	{
		temp = word(
				au8Buffer[ (BYTE_CNT + 1) + i * 2 ],
				au8Buffer[ (BYTE_CNT + 2) + i * 2 ]);

		regs[ u8StartAdd + i ] = temp;
	}
	u8CopyBufferSize = u8BufferSize +2;
	sendTxBuffer();

	return u8CopyBufferSize;
}
