#include "ezGfx.h"
#include <stdio.h>


void keydown(EZ_Key_t* k) {

	if (k->code == K_ESCAPE) EZ_stop();

	printf("key %d, typed %d\n", k->code, k->typed);
}


int main() {

	EZ_setCallback_keyPressed(keydown);

	EZ_start();
	EZ_join();

	return 0;
}