#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int compare(void* a, void* b)
{
	return (int)a - (int)b;
}


void print_num(void* x)
{
	printf("%d", (int)x);
}

void pushs(struct heap_t* h, int n)
{
	for (;n > 0; --n) {
		heap_push(h, (void*)rand());
	}
}

void pops(struct heap_t* h, int n)
{
	for (;n > 0; --n) {
		heap_pop(h);
	}
}


void test(struct heap_t* h)
{
	int n_trx;
	int i,j;
	void* x;
	int nr_elmt = 0;
	int nr_push = 0;
	int nr_pop = 0;

	n_trx = rand() % 100;
	//printf("n_trx = %d\n", n_trx);
	for (; n_trx > 0; --n_trx) {
		if (h->size < 10) {
			i = rand() % (11 - h->size);
			nr_elmt += i;
			nr_push += i;
			pushs(h, i);
		}
		if (h->size > 0) {
			i = rand() % (h->size+1);
			nr_elmt -= i;
			nr_pop += i;
			pops(h, i);
		}
	}

	//printf("nr_elmt = %d push = %d pop = %d\n", nr_elmt, nr_push, nr_pop);
	if (h->size > 0) {
		printf("[");
		while (h->size > 0) {
			heap_peek(h, &x);
			print_num(x);
			//heap_foreach(h, print_num);
			heap_pop(h);
			if (h->size)
				printf(", ");
		}
		printf("]\n");
	}
}
// ./a.out | tee a | xargs  -l1 -I{} python -c "print(str(sorted({})))" > b
int main()
{
	int i;
	void* x;
	struct heap_t h;

	srand(10);

	heap_init(&h, compare);
	for (i=0; i < 1000; ++i) {
		test(&h);
	}
	return 0;
}

