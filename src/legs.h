#ifndef LEGS_H_
#define LEGS_H_

#include <stdint.h>

enum leg_dir {
	LEG_DIR_UP        = 0,
	LEG_DIR_DOWN      = 1,
	LEG_DIR_FORWARD   = 2,
	LEG_DIR_BACKWARD  = 3,

	LEG_DIR_COUNT     = 4
};

enum leg_indx {
	LEG_INVALID       = -1,
	LEG_TOP_LEFT      = 0,
	LEG_TOP_RIGHT     = 1,
	LEG_MIDDLE_LEFT   = 2,
	LEG_MIDDLE_RIGHT  = 3,
	LEG_BOTTOM_LEFT   = 4,
	LEG_BOTTOM_RIGHT  = 5,

	LEG_COUNT = 6
};


int  legs_init();
void legs_config(enum leg_indx indx, enum leg_dir dir, uint8_t degree);
void legs_save();

void legs_setdir(enum leg_indx indx, enum leg_dir dir, int steps);
void legs_setdir_group(enum leg_indx* group, enum leg_dir dir, int steps);
int  legs_move(enum leg_indx indx);
int  legs_move_group(enum leg_indx* group);
int  legs_move_all();


#endif

