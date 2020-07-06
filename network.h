#ifndef __NETWORK_H__
#define __NETWORK_H__

#define LOGSIZE 1024

void logSetup();
void lprintf(const char *fmt, ...);

void netSetup(void);
void netLoop(void);

#endif
