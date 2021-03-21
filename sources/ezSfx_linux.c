#include "ezSfx.h"

#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <alsa/asoundlib.h>


//Globals
bool running;
pthread_t thread;
void* sfxThread(void* arg);
EZ_sample(*sample_callback)(double time, int channel);
snd_pcm_t *device;

struct {
  int sampleRate;
  int channels;
  int blockQueueLength;
  int blockSize;
} info;


EZ_sample EZ_sfx_pcmNextSample(EZ_pcmArray* array) {

  if (array->data == NULL) return 0;

	EZ_sample sample = array->data[array->curr];

  if (array->curr < array->size)
    array->curr ++;

	return sample;

}

EZ_pcmArray EZ_sfx_pcmLoad(const char* fname) {

  //open file
  FILE *file = fopen(fname,"rb");

  //get the size
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  //init array
  EZ_pcmArray chunk = {0};
  chunk.curr = 0;
  chunk.data = malloc(fileSize);
  chunk.size = fileSize / sizeof(EZ_sample);

  //read samples
  if (chunk.size != 0 && chunk.data != NULL)
    fread(chunk.data, sizeof(EZ_sample), chunk.size, file);

  fclose(file);
  return chunk;
}

void EZ_sfx_pcmFree(EZ_pcmArray* array) {
  free(array->data);
}


void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize,
                 EZ_sample(*callback)(double time, int channel)) {

    info.sampleRate = sampleRate;
    info.channels   = channels;
    info.blockSize  = blockSize;
    info.blockQueueLength = blockQueueLength;

    if (callback)
      sample_callback = callback;
    else {
      printf("Null callback function, fool !");
      exit(EXIT_FAILURE);
    }




    //init alsa  https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Sound.h
    int rc = snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0){
      printf("Sound doesn't work");
      exit(EXIT_FAILURE);
    }


    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(device, params);

    snd_pcm_hw_params_set_access(device, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(device, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(device, params, sampleRate, 0);
    snd_pcm_hw_params_set_channels(device, params, channels);
    snd_pcm_hw_params_set_period_size(device, params, blockSize, 0);
    snd_pcm_hw_params_set_periods(device, params, blockQueueLength, 0);
    snd_pcm_hw_params(device, params);


}

void EZ_sfx_start() {
  running = true;
  pthread_create(&thread, NULL, &sfxThread, NULL);
}

void EZ_sfx_stop()  {
  running = false;
}

void EZ_sfx_join()  {
  pthread_join(thread, NULL);
}





void* sfxThread(void* arg) {
  //   //!!\\ sample vs "frame" trickery

  pthread_detach(pthread_self());

  double globalTime = 0;
  double dt = 1.0 / info.sampleRate;

  //init block
  EZ_sample block[info.blockSize];
  snd_pcm_start(device);

  //main loop
  while (running) {

   //fill the block
  for (uint32_t i = 0; i < info.blockSize; i+= info.channels) {

    for (int chan = 0; chan < info.channels; chan++)
      block[i + chan] = sample_callback(globalTime, chan);

    globalTime += dt;
  }


   //render the block
   snd_pcm_uframes_t frames = info.blockSize / info.channels;
   EZ_sample *current = block;

   while (frames > 0) {

      int rc = snd_pcm_writei(device, current, frames);
      if (rc > 0) {
      	current   += rc * info.channels;
      	frames -= rc;
      }

      if (rc == -EAGAIN) continue;
      if (rc == -EPIPE) // an underrun occured, prepare the device for more data
      	snd_pcm_prepare(device);

   }


  }

  snd_pcm_drain(device);
	snd_pcm_close(device);

  pthread_exit(NULL);
  return NULL;

}
