#ifndef CMD_H_
#define CMD_H_

#include <stdint.h>
struct command_header_t {
	uint8_t cmd_type;
} __attribute((packed));


struct command_config_leg_t {
	struct command_header_t hdr;
	uint8_t indx;
	uint8_t dir;
	uint8_t degree;
} __attribute__((packed));

struct command_set_sleep_t {
	struct command_header_t hdr;
	uint16_t millis;
}  __attribute__((packed));

enum command_type {
	CMD_NULL = 0,
	CMD_CONFIG_LEG = 1,
	CMD_MV_FORWARD = 2,
	CMD_MV_BACKWARD = 3,
	CMD_PIVOT_CLOCKWISE = 4,
	CMD_PIVOT_CC = 5,
	CMD_STOP = 6,
	CMD_SAVE_CONFIG = 7,
	CMD_SET_HSLEEP = 8,
	CMD_SET_VSLEEP = 9,
	
	CMD_COUNT // must be last
};

#ifdef ARDUINO
#include <arduinos.h>

struct buffer_t {
	int buff_count;
	uint8_t buff_arr[64];
};

struct handler_params_t {
	struct arduinos_semaphore_t params_done_reading_buff;
	struct buffer_t* params_raw_cmd;
};

int cmd_init();
int cmd_dispatch(struct handler_params_t*);
#endif /* ARDUINO */


#endif /* CMD_H_ */

