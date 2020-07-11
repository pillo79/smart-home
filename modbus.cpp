#include "modbus.h"
#include "modbusdevice.h"

#include <Arduino.h>

#define MB_MSG_DELAY 20

static ModbusRTU mb;

static enum {
	MB_INIT,
	MB_SEND_MSG,
	MB_WAIT_REPLY,
} cur_state;
static int cur_dev_idx;
static int cur_msg_idx;
static int last_millis;
static const modbus_msg_t *cur_msgs;

void modbusSetup()
{
	mb.begin(19200);
	mb.setTimeOut(100);
	cur_state = MB_INIT;
}

void modbusPoll()
{
	switch(cur_state) {
	case MB_INIT:
		cur_dev_idx = 0;
		cur_msg_idx = 0;
		cur_msgs = MODBUS_DEV_TABLE[cur_dev_idx]->getMessages();

		// fall-through is intentional

	case MB_SEND_MSG:
		if (millis()-last_millis > MB_MSG_DELAY) {
			// waited enough, send message
			mb.query(cur_msgs[cur_msg_idx]);
			cur_state = MB_WAIT_REPLY;
			last_millis = millis();
		}
		break;

	case MB_WAIT_REPLY:
		mb.poll();
		if (mb.getState() == MB_COM_IDLE) {
			// reply received, find next message
			// at least one message per device must be present
			if (!cur_msgs[++cur_msg_idx].u8fct) {
				cur_msg_idx = 0;
				if (!MODBUS_DEV_TABLE[++cur_dev_idx]) cur_dev_idx = 0;
				cur_msgs = MODBUS_DEV_TABLE[cur_dev_idx]->getMessages();
			}
			last_millis = millis();
			cur_state = MB_SEND_MSG;
		}
		break;
	}
}
