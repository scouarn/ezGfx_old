#include "ezGfx/ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void EZ_callback_init() {}
void EZ_callback_draw(double dt) {}
void EZ_callback_keyPressed(EZ_Key key) {}
void EZ_callback_keyReleased(EZ_Key key) {}
void EZ_callback_mouseMoved(EZ_Mouse mouse) {}
void EZ_callback_kill() {}
EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}


int main (int argc, char **argv) {

	EZ_start();
	EZ_join();

	return 0;
}
