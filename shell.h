#ifndef __SHELL_H__
#define __SHELL_H__

struct ShellState {
	char cmd[128];
	char reply[1024];
};

void shell(struct ShellState *s);

#endif // __SHELL_H__
