#include "ModbusRTU.h"

#include <Arduino.h>
#include <Print.h>

#include "log.h"

#define RS485_PIN_DE      0b01000000
#define RS485_PIN_RE      0b00100000
#define RS485_DIR_REG     DDRJ
#define RS485_PORT_REG    PORTJ
#define RS485_CLEAR_DE    RS485_PORT_REG &= ~RS485_PIN_DE
#define RS485_CLEAR_RE    RS485_PORT_REG &= ~RS485_PIN_RE
#define RS485_SET_DE      RS485_PORT_REG |= RS485_PIN_DE
#define RS485_SET_RE      RS485_PORT_REG |= RS485_PIN_RE

/**
 * @enum MESSAGE
 * @brief
 * Indexes to telegram frame positions
 */
enum MESSAGE
{
	ID = 0, //!< ID field
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
 * @ingroup setup
 */
ModbusRTU::ModbusRTU()
{
	this->u16timeOut = 1000;
	this->u32overTime = 0;
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
 * @return TRUE if timeout elapsed
 * @ingroup loop
 */
bool ModbusRTU::getTimeOutState()
{
	return ((unsigned long)(millis() -u32xferStartTime) > (unsigned long)u16timeOut);
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

	if ((unsigned long)(millis() - u32xferStartTime) > (unsigned long)u16timeOut)
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
		u32lastCharTime = millis();
		return 0;
	}
	if ((unsigned long)(millis() - u32lastCharTime) < (unsigned long)T35) return 0;

	// transfer Serial buffer frame to auBuffer
	u8lastRec = 0;
	int8_t i8state = getRxBuffer();
#ifdef DEBUG_UART
	for (int i=0; i<i8state; ++i)
		lprintf(" %02x", au8Buffer[i]);
	lprintf("\n");
#endif

	if (i8state < 6)
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

/* _____PRIVATE FUNCTIONS_____________________________________________________ */

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
	u32xferStartTime = millis();

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
	uint16_t u16CalcCRC = calcCRC( u8BufferSize-2 );
#ifdef DEBUG_UART
	lprintf("len %i, crc_msg %04x, crc_calc %04x\n", u8BufferSize, u16MsgCRC, u16CalcCRC);
#endif
	if ( u16CalcCRC != u16MsgCRC )
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
