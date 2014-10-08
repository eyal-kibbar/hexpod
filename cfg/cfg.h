#ifndef CFG_H_
#define CFG_H_

#define SERVO_COUNT 12

struct servo_cfg_t {
	int  cfg_pin;
	int  cfg_leg_indx;
	char cfg_dir;
	int  cfg_min;
	int  cfg_max;
};


#endif

