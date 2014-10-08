#include "legs.h"
#include <arduinos.h>
#include <Servo.h>
#include <Arduino.h>
#include <stdint.h>
#include <EEPROM.h>
#include "../cfg/cfg.h"

#define LEG_MAGIC 0xDEAD

struct leg_t {
	Servo leg_hservo;
	Servo leg_vservo;

	uint8_t leg_dir_degree[LEG_DIR_COUNT];
	float leg_vcurr;
	float leg_hcurr;

	float leg_step;
	Servo* leg_curr_servo;
	float* leg_curr;
};

struct legs_config_t {
	uint16_t magic;
	uint8_t dir[LEG_DIR_COUNT][LEG_COUNT];
};


// globals
static struct leg_t legs[LEG_COUNT];

// static


// API

int legs_init()
{
	int i, j;
	struct legs_config_t legs_cfg;
	struct servo_cfg_t servos_cfg[] = {
		#include "../cfg/servos.cfg"
	};

	for (i=0; i < SERVO_COUNT; ++i) {
		pinMode(servos_cfg[i].cfg_pin, OUTPUT);
		switch (servos_cfg[i].cfg_dir) {
		case 'v':
			legs[servos_cfg[i].cfg_leg_indx].leg_vservo.attach(
				servos_cfg[i].cfg_pin,
				servos_cfg[i].cfg_min, servos_cfg[i].cfg_max);
			break;
		case 'h':
			legs[servos_cfg[i].cfg_leg_indx].leg_hservo.attach(
				servos_cfg[i].cfg_pin,
				servos_cfg[i].cfg_min, servos_cfg[i].cfg_max);
			break;
		default:
			return -1;
	}
	}


	// Load saved configuration
	for (i=0; i < sizeof(legs_cfg); ++i) {
		(((uint8_t*)&legs_cfg)[i]) = (uint8_t)EEPROM.read(i);
	}
	
	if (legs_cfg.magic != LEG_MAGIC) {
		//Serial.print("BAD MAGIC: ");
		//Serial.println(cfg.magic);
		return -1;
	}

	for (i=0; i < LEG_COUNT; ++i) {
		for (j=0; j < LEG_DIR_COUNT; ++j) {
			legs[i].leg_dir_degree[j] = legs_cfg.dir[j][i];
			/*
			Serial.print("leg ");
			Serial.print(i);
			Serial.print(" dir ");
			Serial.print(j);
			Serial.print(" degree ");
			Serial.println(legs[i].leg_dir_degree[j]);
			*/
		}
	}
	
	return 0;
}

void legs_config(enum leg_indx indx, enum leg_dir dir, uint8_t degree)
{
	/*
	Serial.print("legs_config");
	Serial.print(" indx = ");
	Serial.print(indx);
	Serial.print(" dir = ");
	Serial.print((int)dir);
	Serial.print(" degree = ");
	Serial.println(degree);
	*/

	legs[indx].leg_dir_degree[dir] = degree;
	switch (dir) {
	case LEG_DIR_UP: case LEG_DIR_DOWN:
		legs[indx].leg_vservo.write(degree);
		legs[indx].leg_vcurr = degree;
		break;
	case LEG_DIR_FORWARD: case LEG_DIR_BACKWARD:
		legs[indx].leg_hservo.write(degree);
		legs[indx].leg_hcurr = degree;
		break;
	}
}

void legs_save()
{
	int i, j;
	struct legs_config_t cfg;

	cfg.magic = LEG_MAGIC;
	for (i=0; i < LEG_COUNT; ++i) {
		for (j=0; j < LEG_DIR_COUNT; ++j) {
			cfg.dir[j][i] = legs[i].leg_dir_degree[j];
			/*
			Serial.print("leg ");
			Serial.print(i);
			Serial.print(" dir ");
			Serial.print(j);
			Serial.print(" degree ");
			Serial.println(cfg.dir[j][i]);
			*/
		}
	}

	for (i = 0; i < sizeof(cfg); ++i) {
		EEPROM.write(i, (int)(((uint8_t*)&cfg)[i]));
	}
}

void legs_setdir(enum leg_indx indx, enum leg_dir dir, int steps)
{
	switch (dir) {
	case LEG_DIR_UP: case LEG_DIR_DOWN:
		legs[indx].leg_curr_servo = &legs[indx].leg_vservo;
		legs[indx].leg_curr = &legs[indx].leg_vcurr;
		break;
	case LEG_DIR_FORWARD: case LEG_DIR_BACKWARD:
		legs[indx].leg_curr_servo = &legs[indx].leg_hservo;
		legs[indx].leg_curr = &legs[indx].leg_hcurr;
		break;
	}
	legs[indx].leg_step = (legs[indx].leg_dir_degree[dir] - *(legs[indx].leg_curr)) / (float)steps;
	
	/*
	Serial.print("setdir indx = ");
	Serial.print(indx);
	Serial.print(" dir = ");
	Serial.print((int)dir);
	Serial.print(" steps = ");
	Serial.print(steps);
	Serial.print(" leg_step = ");
	Serial.println(legs[indx].leg_step);
	*/
}

void legs_setdir_group(enum leg_indx* group, enum leg_dir dir, int steps)
{
	int i;
	for (i=0; group[i] != LEG_INVALID; legs_setdir(group[i++], dir, steps));
}

int  legs_move(enum leg_indx indx)
{
	/*
	Serial.print("move leg indx = ");
	Serial.print(indx);
	Serial.print(" curr = ");
	Serial.print(*legs[indx].leg_curr);
	Serial.print(" step = ");
	Serial.print(legs[indx].leg_step);
	Serial.print(" u = ");
	Serial.print(legs[indx].leg_dir_degree[LEG_DIR_UP]);
	Serial.print(" d = ");
	Serial.print(legs[indx].leg_dir_degree[LEG_DIR_DOWN]);
	Serial.print(" f = ");
	Serial.print(legs[indx].leg_dir_degree[LEG_DIR_FORWARD]);
	Serial.print(" b = ");
	Serial.print(legs[indx].leg_dir_degree[LEG_DIR_BACKWARD]);
	Serial.print(" servo = ");
	Serial.println(legs[indx].leg_curr_servo == &legs[indx].leg_hservo ? "h" : "v");
	*/

	*legs[indx].leg_curr += legs[indx].leg_step;
	legs[indx].leg_curr_servo->write((int)(*legs[indx].leg_curr));
	
	return 0;
}

int  legs_move_group(enum leg_indx* group)
{
	int i;
	for (i=0; group[i] != LEG_INVALID; legs_move(group[i++]));
	return 0;
}

int  legs_move_all()
{
	int i;
	for (i=0; i < LEG_COUNT; legs_move((enum leg_indx)i++));
	return 0;
}

