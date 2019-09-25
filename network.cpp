#include "network.h"
#include "shell.h"

#include <Ethernet.h>

EthernetServer server(23);

struct ShellState st;

void netSetup(void)
{
	static const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	static const IPAddress ip(192, 168, 0, 177);
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
