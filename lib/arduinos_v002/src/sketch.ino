#include "arduinos.h"


struct arduinos_semaphore_t sem;


int f1(void* arg)
{
	int n = (int)arg;
	//while (1) {
		Serial.print("arg = ");
		Serial.print((int)&arg);
		Serial.print(" ");
		Serial.println(n);
		
		arduinos_delay(10000);
		arduinos_semaphore_signal(&sem);

		arduinos_delay(10000);
		Serial.println("not dead");
	//}
	return 100;
}

int f2(void* arg)
{
	int n = (int)arg;
	int ret;
	Serial.println("waiting ... ");
	arduinos_delay(1000);

	arduinos_semaphore_wait(&sem);

	Serial.println("killing ... ");

	arduinos_kill(n);
	arduinos_join(n, &ret);
	Serial.print("got ");
	Serial.println(ret);
	while (1);
	return 0;
}


void setup()
{
	cid x;
	Serial.begin(9600);
	Serial.println("starting");
	arduinos_setup();
	arduinos_semaphore_init(&sem, 0);
	Serial.println("creating f1");
	x = arduinos_create(f1, (void*)8);
	arduinos_create(f2, (void*)x);

}

void loop()
{
	Serial.println("loop");
	arduinos_loop();
}



