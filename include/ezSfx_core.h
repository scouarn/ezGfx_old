
/*
/
/	EZSFX (standalone from EZGFX).
/
/
/ SCOUARN, 2021
/
/	Sound output.
/
/
/
*/


#ifndef EZSFX_CORE
#define EZSFX_CORE

#include <stdlib.h>
#include <utils.h>

#define CD41KHZ 44100
#define SAMPLE_MAX 32767
typedef i16 EZ_Sample; //signed 16bits iteger samples

typedef struct {
	EZ_Sample* data;	//array of samples
	u32   size;			//array length
	EZ_Sample* curr;	//pointer to element of array (->sample to be played)
} EZ_pcmArray;


EZ_Sample   EZ_sfx_pcmNextSample(EZ_pcmArray array); //return and increment current
EZ_pcmArray EZ_sfx_pcmLoad(const char* filename);	 //load array
void EZ_sfx_pcmFree(EZ_pcmArray array);				 //free array


//init audio
void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize);


//interact with the audio thread
void EZ_sfx_start();
void EZ_sfx_stop();
void EZ_sfx_join();



typedef struct {
	double(*oscillator)(double time, double pitch);
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

} EZ_Note;

void EZ_sfx_play(EZ_Note* note); //add note on the list...

//oscillators
double EZ_sfx_fastSine(double time, double freq);
double EZ_sfx_sine(double time, double freq);


//callback returning sample to play given the time and the channel
//if the client does not define it, define it anyway
#ifdef SFX_CLIENT_CALLBACK
	extern EZ_Sample EZ_sfx_callback(double time, int channel);
#else
	EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}
#endif

	
#endif
