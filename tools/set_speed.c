#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include "../src/cmd.h"

static const uint8_t delimiter[] = { 'a', 'b', 'c' };

int main(int argc, char* argv[])
{
	struct command_set_sleep_t cmd;
	char* dev;
	int fd;

	if (argc != 4) {
		printf("invalid # params\n");
		return -1;
	}
	

	dev = argv[1];
	switch (*argv[2]) {
	case 'h': cmd.hdr.cmd_type = CMD_SET_HSLEEP; break;
	case 'v': cmd.hdr.cmd_type = CMD_SET_VSLEEP; break;
	default : printf("invalid dir\n"); return -1;
	}
	cmd.millis = atoi(argv[3]);

	if (!(fd = open(dev, O_WRONLY))) {
		printf("cannot open dev %s\n", dev);
		return -1;
	}

	write(fd, &cmd, sizeof cmd);
	write(fd, delimiter, sizeof delimiter);
	return 0;
}


