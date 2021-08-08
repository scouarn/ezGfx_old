

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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define CD41KHZ 44100
#define SAMPLE_MAX 32767
typedef int16_t EZ_sample;

typedef struct {
	EZ_sample* data;
	uint32_t   size;
	uint32_t   curr;
} EZ_pcmArray;

EZ_sample   EZ_sfx_pcmNextSample(EZ_pcmArray* array);
EZ_pcmArray EZ_sfx_pcmLoad(const char* filename);
void EZ_sfx_pcmFree(EZ_pcmArray* array);

void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize,
								 EZ_sample(*callback)(double time, int channel)
);



void EZ_sfx_start();
void EZ_sfx_stop();
void EZ_sfx_join();


#endif
