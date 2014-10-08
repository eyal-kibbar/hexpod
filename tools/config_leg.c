#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include "../src/cmd.h"

static const uint8_t delimiter[] = { 'a', 'b', 'c' };

int main(int argc, char* argv[])
{
	struct command_config_leg_t cmd;
	char* dev;
	int fd;

	if (argc != 5) {
		printf("invalid # params\n");
		return -1;
	}
	
	cmd.hdr.cmd_type = 0x1;

	dev = argv[1];
	cmd.indx = atoi(argv[2]);
	switch (*argv[3]) {
	case 'u': cmd.dir = 0x0; break;
	case 'd': cmd.dir = 0x1; break;
	case 'f': cmd.dir = 0x2; break;
	case 'b': cmd.dir = 0x3; break;
	default : printf("invalid dir\n"); return -1;
	}
	cmd.degree = atoi(argv[4]);

	if (!(fd = open(dev, O_WRONLY))) {
		printf("cannot open dev %s\n", dev);
		return -1;
	}

	write(fd, &cmd, sizeof cmd);
	write(fd, delimiter, sizeof delimiter);
	return 0;
}


