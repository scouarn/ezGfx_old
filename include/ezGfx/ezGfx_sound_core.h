
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

#include <ezGfx/ezGfx_utils.h>

#define CD44KHZ 44100
#define SAMPLE_MAX 32767


typedef int16_t EZ_Sample; //signed 16bits iteger samples

typedef struct {
	EZ_Sample* data;	//array of samples
	EZ_Sample* curr;	//pointer to element of array (->sample to be played)
	uint32_t   size;		//array length
	uint32_t   channels;	//number of channels
	uint32_t   sampleRate;  //samples per seconds * channels

} EZ_PCMArray;


EZ_Sample EZ_sfx_pcmNextSample(EZ_PCMArray* array); //return and increment current
void EZ_sfx_pcmFree(EZ_PCMArray array);				 //free array


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
//define SFX_CLIENT_CALLBACK in ezGfx.h to enable it
extern EZ_Sample EZ_sfx_callback(double time, int channel);


	
#endif
