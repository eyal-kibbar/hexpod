#include "srv.h"
#include "cmd.h"
#include <arduinos.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <Arduino.h>


// static
static int srv_loop(void* reserved);


// consts
static uint8_t delimiter[] = {'a', 'b', 'c'};



int srv_init()
{
	arduinos_create(srv_loop, NULL);
	return 0;
}


static void buff_reset(struct buffer_t* buff, int* delimiter_index)
{
	buff->buff_count = 0;
	*delimiter_index = 0;
}

static int buff_fill(struct buffer_t* buff, int* delimiter_index)
{
	int n;
	while ((n = Serial.read()) != -1) {
		if (buff->buff_count == sizeof buff->buff_arr)
			buff_reset(buff, delimiter_index);
		
		buff->buff_arr[buff->buff_count++] = (uint8_t)n;

		/*
		Serial.print("read: ");
		Serial.println((int)n);
		*/

		*delimiter_index = delimiter[*delimiter_index] == (uint8_t)n ?
			*delimiter_index + 1 :
			!!(delimiter[0] == (uint8_t)n);

		// check if we saw the delimiter
		if (*delimiter_index == sizeof delimiter) {
			buff->buff_count -= sizeof delimiter;
			return 1;
		}
	}
	return 0;
}


static void cmd_execute(struct buffer_t* buff)
{
	int i;
	struct handler_params_t params;

	/*
	Serial.print("got buffer: '");
	for (i=0; i < buff->buff_count; ++i) {
		Serial.print((int)buff->buff[i]);
		Serial.print(" ");
	}
	Serial.println("'");
	*/

	// init params
	arduinos_semaphore_init(&params.params_done_reading_buff, 0);
	params.params_raw_cmd = buff;

	// create a new command handler

	if (arduinos_create((context_start_func)cmd_dispatch, &params) < 0) {
		Serial.println("error creating the handler");
		return;
	}


	// wait for handler to read the buffer
	//Serial.println("waiting for buffer");
	arduinos_semaphore_wait(&params.params_done_reading_buff);
	//Serial.println("done waiting");
}

int srv_loop(void* reserved)
{
	struct buffer_t buff;
	int delimiter_index = 0;
	buff.buff_count = 0;

	while (1) {
		if (buff_fill(&buff, &delimiter_index)) {
			cmd_execute(&buff);
			buff_reset(&buff, &delimiter_index);
		} else {
			arduinos_yield();
		}
	}
}

