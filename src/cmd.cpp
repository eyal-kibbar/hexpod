#include "cmd.h"
#include "srv.h"
#include "legs.h"
#include "hexpod.h"
#include <Arduino.h>
#include <arduinos.h>
#include <stdint.h>

union command_parsed_params_u {
	struct {
		enum leg_indx indx;
		enum leg_dir dir;
		uint8_t degree;
	} config_leg;

	struct {
		int millis;
	} set_sleep;
};

typedef int (*cmd_deserializer_func)(struct handler_params_t*, union command_parsed_params_u*);
typedef int (*cmd_executer_func)(union command_parsed_params_u*);

struct command_t {
	cmd_deserializer_func cmd_deserializer;
	cmd_executer_func     cmd_executer;
};

// static funcs
static int deserialize_config_leg(struct handler_params_t* params, union command_parsed_params_u* parsed);
static int deserialize_set_sleep(struct handler_params_t* params, union command_parsed_params_u* parsed);

static int execute_config_leg(union command_parsed_params_u* cmd_params);
static int execute_mv_forward(union command_parsed_params_u* cmd_params);
static int execute_mv_backward(union command_parsed_params_u* cmd_params);
static int execute_pivot_clockwise(union command_parsed_params_u* cmd_params);
static int execute_pivot_cc(union command_parsed_params_u* cmd_params);
static int execute_stop(union command_parsed_params_u* cmd_params);
static int execute_save_config(union command_parsed_params_u* cmd_params);
static int execute_set_hsleep(union command_parsed_params_u* cmd_params);
static int execute_set_vsleep(union command_parsed_params_u* cmd_params);


// consts
static const struct command_t commands[] = {
	{0},
	//[CMD_CONFIG_LEG]
	{
		.cmd_deserializer = deserialize_config_leg,
		.cmd_executer = execute_config_leg
	},
	//[CMD_MV_FORWARD]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_mv_forward
	},
	//[CMD_MV_BACKWARD]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_mv_backward
	},
	//[CMD_PIVOT_CLOCKWISE]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_pivot_clockwise
	},
	//[CMD_PIVOT_CC]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_pivot_cc
	},
	//[CMD_STOP]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_stop
	},
	//[CMD_SAVE_CONFIG]
	{
		.cmd_deserializer = NULL,
		.cmd_executer = execute_save_config
	},
	//[CMD_SET_HSLEEP]
	{
		.cmd_deserializer = deserialize_set_sleep,
		.cmd_executer = execute_set_hsleep
	},
	//[CMD_SET_VSLEEP]
	{
		.cmd_deserializer = deserialize_set_sleep,
		.cmd_executer = execute_set_vsleep
	}
};

int cmd_dispatch(struct handler_params_t* p)
{
	union command_parsed_params_u parsed;
	struct command_header_t* cmd_header;
	const struct command_t* cmd;

	// common command validate
	if (!p->params_raw_cmd->buff_count)
		goto err;

	cmd_header = (struct command_header_t*)p->params_raw_cmd->buff_arr;
	if (cmd_header->cmd_type == 0 || CMD_COUNT <= cmd_header->cmd_type) {
		Serial.println("invalid command type");
		goto err;
	}

	// deserialize
	cmd = &commands[cmd_header->cmd_type];
	if (cmd->cmd_deserializer && cmd->cmd_deserializer(p, &parsed)) {
		Serial.println("deserialize failed");
		goto err;
	}

	// flag that the buffer is ready for reuse
	arduinos_semaphore_signal(&p->params_done_reading_buff);
	// from now on, do not touch params

	// allow the buffer to be consumed and refilled
	//Serial.println("yielding and allowing buffer to be consumed");
	arduinos_yield();

	// execute
	return cmd->cmd_executer(&parsed);

err:
	// flag that the buffer is ready for reuse
	arduinos_semaphore_signal(&p->params_done_reading_buff);
	return -1;
}

int cmd_init()
{
}

// config leg
static int deserialize_config_leg(struct handler_params_t* p, union command_parsed_params_u* parsed)
{
	struct command_config_leg_t* cmd;

	// validate buffer specific for this command
	if (p->params_raw_cmd->buff_count != sizeof *cmd)
		return -1;
	// read buffer
	cmd = (struct command_config_leg_t*)p->params_raw_cmd->buff_arr;

	parsed->config_leg.indx = (enum leg_indx)cmd->indx;
	parsed->config_leg.dir = (enum leg_dir)cmd->dir;
	parsed->config_leg.degree = (uint8_t)cmd->degree;

	return 0;
}

static int deserialize_set_sleep(struct handler_params_t* p, union command_parsed_params_u* parsed)
{
	struct command_set_sleep_t* cmd;

	// validate buffer specific for this command
	if (p->params_raw_cmd->buff_count != sizeof *cmd)
		return -1;

	// read buffer
	cmd = (struct command_set_sleep_t*)p->params_raw_cmd->buff_arr;

	parsed->set_sleep.millis = (int)cmd->millis;

	Serial.print("set sleep ");
	Serial.println(parsed->set_sleep.millis);

	return 0;
}

static int execute_config_leg(union command_parsed_params_u* p)
{
	legs_config(p->config_leg.indx, p->config_leg.dir, p->config_leg.degree);
	return 0;
}

static int execute_mv_forward(union command_parsed_params_u* cmd_params)
{
	Serial.println("mv forward !");
	hexpod_forward();
	return 0;
}

static int execute_mv_backward(union command_parsed_params_u* cmd_params)
{
	hexpod_backward();
	return 0;
}

static int execute_pivot_clockwise(union command_parsed_params_u* cmd_params)
{
	hexpod_pivot_clockwise();
	return 0;
}

static int execute_pivot_cc(union command_parsed_params_u* cmd_params)
{
	hexpod_pivot_cc();
	return 0;
}

static int execute_stop(union command_parsed_params_u* cmd_params)
{
	hexpod_stop();
	return 0;
}

static int execute_save_config(union command_parsed_params_u* cmd_params)
{
	legs_save();
	return 0;
}

static int execute_set_hsleep(union command_parsed_params_u* cmd_params)
{
	hexpod_set_hsleep(cmd_params->set_sleep.millis);
	return 0;
}

static int execute_set_vsleep(union command_parsed_params_u* cmd_params)
{
	hexpod_set_vsleep(cmd_params->set_sleep.millis);
	return 0;
}

