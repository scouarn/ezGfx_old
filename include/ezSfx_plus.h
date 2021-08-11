#include "ezSfx_core.h"

#ifndef EZSFX_PLUS
#define EZSFX_PLUS


typedef struct {
	double(*callback)(double time, double pitch);
	double startTime;
	double endTime;
	double length;

	double pitch;

	double vibrato;
	double vibratoAmp;

	double tremolo;
	double tremoloAmp;

	double panning;
	double panningAmp;

	double attack;
	double decay;
	double sustain;
	double release;

} EZ_sfx_note;


double EZ_sfx_fastSine(double time, double freq);
double EZ_sfx_sine(double time, double freq);

void EZ_sfx_play(EZ_sfx_note note); //add note on the list...



#endif
