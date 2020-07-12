#include "modbus.h"
#include "modbusdevice.h"

#include <Arduino.h>

#define MB_MSG_DELAY 20

static ModbusRTU mb;

static enum {
	MB_INIT,
	MB_GET_MSGS,
	MB_SEND_MSG,
	MB_WAIT_REPLY,
} cur_state;
static int cur_dev_idx;
static int cur_msg_idx;
static int last_millis;
static const modbus_msg_t *cur_msgs;

void modbusSetup()
{
	mb.begin(19200, SERIAL_8N2);
	mb.setTimeOut(50);
	cur_state = MB_INIT;
}

void modbusPoll()
{
	switch(cur_state) {
	case MB_INIT:
		cur_dev_idx = 0;
		cur_msg_idx = 0;

		// fall-through is intentional

	case MB_GET_MSGS:
		cur_msgs = MODBUS_DEV_TABLE[cur_dev_idx]->getMessages();
		cur_msg_idx = 0;
		if (cur_msgs) {
			cur_state = MB_SEND_MSG;
		} else {
			if (!MODBUS_DEV_TABLE[++cur_dev_idx]) cur_dev_idx = 0;
		}
		break;

	case MB_SEND_MSG:
		if (millis()-last_millis > MB_MSG_DELAY) {
			// waited enough, send message
			mb.query(cur_msgs[cur_msg_idx]);
			cur_state = MB_WAIT_REPLY;
		}
		break;

	case MB_WAIT_REPLY:
		mb.poll();
		if (mb.getState() == MB_COM_IDLE) {
			// reply received
			last_millis = millis();

			// find next message
			if (!cur_msgs[++cur_msg_idx].u8fct) {
				if (!MODBUS_DEV_TABLE[++cur_dev_idx]) cur_dev_idx = 0;
				cur_state = MB_GET_MSGS;
			} else {
				cur_state = MB_SEND_MSG;
			}
		}
		break;
	}
}
