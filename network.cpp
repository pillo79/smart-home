#include "network.h"
#include "shell.h"
#include "log.h"

#include <Ethernet.h>
#include <stdarg.h>

#define MAX_CLIENTS	3

EthernetServer server(23);
EthernetClient clients[MAX_CLIENTS];

char shell_cmd[128];

char logBuf[LOGSIZE];
int logWr, logEob, logRd;

void logSetup()
{
	logWr = logRd = 0;
	logEob = LOGSIZE;
}

void lprintf(const char *fmt, ...)
{
	va_list ap;
	int old_logWr = logWr;
	int wrap = 0;

	va_start(ap, fmt);
	int msglen = vsnprintf(logBuf+logWr, LOGSIZE-logWr, fmt, ap);
	va_end(ap);

	if (logWr+msglen > LOGSIZE) {
		logEob = old_logWr;
		logWr = 0;
		wrap = 1;

		va_start(ap, fmt);
		msglen = vsnprintf(logBuf+logWr, LOGSIZE-logWr, fmt, ap);
		va_end(ap);
	}

	logWr += msglen;
	if (logWr == LOGSIZE)
	{
		logEob = logWr;
		logWr = 0;
		wrap = 1;
	}

	if (!wrap) {
		if ((logRd > old_logWr) && (logRd <= logWr)) {
			logRd = (logWr+1) % LOGSIZE;
			logBuf[logRd] = '#';
		}
	} else {
		if ((logRd > old_logWr) || (logRd <= logWr)) {
			logRd = (logWr+1) % LOGSIZE;
			logBuf[logRd] = '#';
		}
	}

	if (logRd >= logEob)
		logEob = logRd;
}

void netSetup(void)
{
	static const byte mac[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB };
	static const IPAddress ip(192, 168, 1, 252);
	Ethernet.begin(mac, ip);
	server.begin();
}

void netLoop()
{
	int flush_log = 0;

	for (int i=0; i<MAX_CLIENTS; ++i) {
		if (!clients[i])
			clients[i] = server.accept();

		if (!clients[i])
			continue;

		if (logRd != logWr) {
			if (logRd > logWr)
				clients[i].write(logBuf+logRd, logEob-logRd);
			else
				clients[i].write(logBuf+logRd, logWr-logRd);
			flush_log = 1;
		}

		if (clients[i].available()) {
			int msglen = clients[i].read(shell_cmd, sizeof(shell_cmd));
			shell_cmd[msglen] = '\0';

			shell(shell_cmd);
		}

		if (!clients[i].connected())
			clients[i].stop();
	}

	if (flush_log) {
		if (logRd > logWr) {
			logRd = 0;
			logEob = LOGSIZE;
		} else {
			logRd = logWr;
		}
	}
}
