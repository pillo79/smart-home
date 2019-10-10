#include "network.h"
#include "shell.h"

#include <Ethernet.h>

EthernetServer server(23);

struct ShellState st;

void netSetup(void)
{
	static const byte mac[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
	static const IPAddress ip(192, 168, 1, 252);
	Ethernet.begin(mac, ip);
	server.begin();
}

void netLoop()
{
	EthernetClient client = server.available();
	if (client) {
		int msglen = client.read(st.cmd, sizeof(st.cmd));
		st.cmd[msglen] = '\0';
		Serial.print(st.cmd);

		shell(&st);
		int len = strlen(st.reply);
		if (len) {
			client.write(st.reply, len);
			Serial.print(st.reply);
		}
	}
}
