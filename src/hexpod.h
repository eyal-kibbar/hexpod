#ifndef HEXPOD_H_
#define HEXPOD_H_


void hexpod_init();
void hexpod_forward();
void hexpod_backward();
void hexpod_stop();
void hexpod_pivot_clockwise();
void hexpod_pivot_cc();

void hexpod_set_hsleep(int millis);
void hexpod_set_vsleep(int millis);

#endif

