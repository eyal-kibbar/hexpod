#include <arduinos.h>
#include "legs.h"
#include "srv.h"
#include "hexpod.h"
#include "cmd.h"



void setup()
{
	Serial.begin(9600);
	//Serial.println("starting");
	arduinos_setup();
	legs_init();
	hexpod_init();
	srv_init();
	cmd_init();
}

void loop()
{
	arduinos_loop();
}



