#include "modbus.h"
#include "modbusdevice.h"
#include "log.h"

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
	int now = millis();
	int ret;

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
		if ((now-last_millis) > MB_MSG_DELAY) {
			// waited enough, send message
			ret = mb.query(cur_msgs[cur_msg_idx]);
			if (ret)
				lprintf("MB %i[%i]: query(%i,%i) %i @%08x after %i\n", cur_dev_idx, cur_msg_idx, cur_msgs[cur_msg_idx].u8id, cur_msgs[cur_msg_idx].u8fct, ret, now, now-last_millis);

			last_millis = now;
			cur_state = MB_WAIT_REPLY;
		}
		break;

	case MB_WAIT_REPLY:
		ret = mb.poll();
		if (mb.getState() == MB_COM_IDLE) {
			// reply received

			if (mb.getLastError()) {
				lprintf("MB %i[%i]: %i err %i @%08x after %i\n", cur_dev_idx, cur_msg_idx, ret, (int) mb.getLastError(), now, now-last_millis);
//			} else {
//				lprintf("MB %i[%i]: %i ack @%08x after %i\n", cur_dev_idx, cur_msg_idx, ret, now, now-last_millis);
			}
			last_millis = now;

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
