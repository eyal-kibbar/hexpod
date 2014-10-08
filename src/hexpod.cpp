#include "hexpod.h"
#include <arduinos.h>
#include "legs.h"
#include <Arduino.h>


enum hexpod_op {
	HEXPOD_OP_INVALID,
	HEXPOD_OP_FORWARD,
	HEXPOD_OP_BACKWARD,
	HEXPOD_OP_PIVOT_CLOCKWISE,
	HEXPOD_OP_PIVOT_CC
};

// globals
static cid curr_op_cid;
static enum hexpod_op curr_op;
static int hsleep_ms = 200;
static int vsleep_ms = 400;
static int steps = 4;

// consts
static enum leg_indx legs_group_2_1[] = {
	LEG_TOP_LEFT,
	LEG_MIDDLE_RIGHT,
	LEG_BOTTOM_LEFT,
	LEG_INVALID
};
static enum leg_indx legs_group_1_2[] = {
	LEG_TOP_RIGHT,
	LEG_MIDDLE_LEFT,
	LEG_BOTTOM_RIGHT,
	LEG_INVALID
};

static enum leg_indx legs_group_outer[] = {
	LEG_TOP_LEFT,
	LEG_TOP_RIGHT,
	LEG_BOTTOM_LEFT,
	LEG_BOTTOM_RIGHT,
	LEG_INVALID
};

static enum leg_indx legs_group_inner[] = {
	LEG_MIDDLE_LEFT,
	LEG_MIDDLE_RIGHT,
	LEG_INVALID
};

static enum leg_indx legs_group_2_0[] = {
	LEG_TOP_LEFT,
	LEG_BOTTOM_LEFT,
	LEG_INVALID
};

static enum leg_indx legs_group_0_2[] = {
	LEG_TOP_RIGHT,
	LEG_BOTTOM_RIGHT,
	LEG_INVALID
};

static enum leg_indx legs_group_3_0[] = {
	LEG_TOP_LEFT,
	LEG_MIDDLE_LEFT,
	LEG_BOTTOM_LEFT,
	LEG_INVALID
};

static enum leg_indx legs_group_0_3[] = {
	LEG_TOP_RIGHT,
	LEG_MIDDLE_RIGHT,
	LEG_BOTTOM_RIGHT,
	LEG_INVALID
};

static void swap(void** a, void** b)
{
	void* t;
	t = *a;
	*a = *b;
	*b = t;
}

static int hexpod_change_op(enum hexpod_op newop)
{
	// no need to change op
	if (curr_op == newop)
		return 1;

	hexpod_stop();
	curr_op_cid = arduinos_self();
	curr_op = newop;

	return 0;
}

void hexpod_init()
{
	curr_op_cid = -1;
	curr_op = HEXPOD_OP_INVALID;
}

static void hexpod_move(enum leg_dir d1, enum leg_dir d2)
{
	enum leg_indx *g1 = legs_group_2_1;
	enum leg_indx *g2 = legs_group_1_2;
	int i;

	// init legs

	// move group 1 to d1
	legs_setdir_group(g1, LEG_DIR_UP, 1);
	legs_setdir_group(g2, LEG_DIR_DOWN, 1);

	legs_move_group(g2);
	arduinos_delay(vsleep_ms);
	legs_move_group(g1);
	arduinos_delay(vsleep_ms);


	legs_setdir_group(g1, d1, 1);
	legs_move_group(g1);
	arduinos_delay(hsleep_ms);

	// move group 2 to d2
	legs_setdir_group(g1, LEG_DIR_DOWN, 1);
	legs_setdir_group(g2, LEG_DIR_UP, 1);

	legs_move_group(g1);
	arduinos_delay(vsleep_ms);
	legs_move_group(g2);
	arduinos_delay(vsleep_ms);

	legs_setdir_group(g2, d2, 1);
	legs_move_group(g2);
	arduinos_delay(hsleep_ms);
	
	// move legs
	while (1) {
		// move
		legs_setdir_group(g1, d2, steps);
		legs_setdir_group(g2, d1, steps);
		for (i=0; i < steps; ++i) {
			legs_move_all();
			arduinos_delay(hsleep_ms / steps);
		}

		// switch up and down
		legs_setdir_group(g1, LEG_DIR_UP, 1);
		legs_setdir_group(g2, LEG_DIR_DOWN, 1);

		legs_move_group(g2);
		arduinos_delay(vsleep_ms);
		legs_move_group(g1);
		arduinos_delay(vsleep_ms);

		// swap groups
		swap((void**)&g1, (void**)&g2);
	}
}


static void hexpod_pivot(enum leg_dir d1, enum leg_dir d2)
{
	int i;
	enum leg_indx* g1 = legs_group_2_1;
	enum leg_indx* g2 = legs_group_1_2;
	enum leg_indx* s1 = legs_group_outer;
	enum leg_indx* s2 = legs_group_inner;

	// init legs

	legs_setdir_group(g2, LEG_DIR_DOWN, 1);
	legs_setdir_group(g1, LEG_DIR_UP, 1);
	
	legs_move_group(g2);
	arduinos_delay(vsleep_ms);
	legs_move_group(g1);
	arduinos_delay(vsleep_ms);

	legs_setdir_group(s1, d1, 1);
	legs_setdir_group(s2, d2, 1);
	legs_move_group(g1);
	arduinos_delay(hsleep_ms);

	// switch legs
	legs_setdir_group(g1, LEG_DIR_DOWN, 1);
	legs_setdir_group(g2, LEG_DIR_UP, 1);
	
	legs_move_group(g1);
	arduinos_delay(vsleep_ms);
	legs_move_group(g2);
	arduinos_delay(vsleep_ms);

	legs_setdir_group(s1, d1, 1);
	legs_setdir_group(s2, d2, 1);
	legs_move_group(g2);
	arduinos_delay(hsleep_ms);

	// move loop
	while (1) {
		legs_setdir_group(s1, d2, steps);
		legs_setdir_group(s2, d1, steps);

		for (i=0; i < steps; ++i) {
			legs_move_all();
			arduinos_delay(hsleep_ms / steps);
		}

		// switch up and down
		legs_setdir_group(g1, LEG_DIR_UP, 1);
		legs_setdir_group(g2, LEG_DIR_DOWN, 1);

		legs_move_group(g2);
		arduinos_delay(vsleep_ms);
		legs_move_group(g1);
		arduinos_delay(vsleep_ms);


		swap((void**)&g1, (void**)&g2);
		swap((void**)&s1, (void**)&s2);
	}
}

void hexpod_forward()
{
	// check if we are already moving forward
	if (hexpod_change_op(HEXPOD_OP_FORWARD))
		return;

	hexpod_move(LEG_DIR_FORWARD, LEG_DIR_BACKWARD);

}

void hexpod_backward()
{
	// check if we are already moving backward
	if (hexpod_change_op(HEXPOD_OP_BACKWARD))
		return;

	hexpod_move(LEG_DIR_BACKWARD, LEG_DIR_FORWARD);
}

void hexpod_stop()
{
	if (curr_op_cid != -1)
		arduinos_kill(curr_op_cid);
	curr_op_cid = -1;
	curr_op = HEXPOD_OP_INVALID;
}

void hexpod_pivot_clockwise()
{
	// check if we are already moving clockwise
	if (hexpod_change_op(HEXPOD_OP_PIVOT_CLOCKWISE))
		return;

	hexpod_pivot(LEG_DIR_FORWARD, LEG_DIR_BACKWARD);
}

void hexpod_pivot_cc()
{
	// check if we are already moving clockwise
	if (hexpod_change_op(HEXPOD_OP_PIVOT_CC))
		return;

	hexpod_pivot(LEG_DIR_BACKWARD, LEG_DIR_FORWARD);
}

void hexpod_set_hsleep(int millis)
{
	hsleep_ms = millis;
}
void hexpod_set_vsleep(int millis)
{
	vsleep_ms = millis;
}

