#include "ezGfx/ezGfx_sound_core.h"

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <alsa/asoundlib.h>




static volatile bool running;
static pthread_t thread;
static void* sfxThread(void* arg);
static snd_pcm_t *device;

static double globalTime;
static EZ_Sample note_mixer(double time, int channel); 

static struct {
  int sampleRate;
  int channels;
  int blockQueueLength;
  int blockSize;
} info;


EZ_Sample EZ_sfx_pcmNextSample(EZ_PCMArray* arr) {

  if (arr->data == NULL || arr->curr - arr->data >= arr->size) return 0;

  else return *++arr->curr;

}

void EZ_sfx_pcmFree(EZ_PCMArray array) {
  free(array.data);
}


void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize) {

    info.sampleRate = sampleRate;
    info.channels   = channels;
    info.blockSize  = blockSize;
    info.blockQueueLength = blockQueueLength;


    //init alsa  https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Sound.h
    int rc = snd_pcm_open(&device, "default", SND_PCM_STREAM_PLAYBACK, 0);
    ASSERTM(rc >= 0, "Couldn't init sound doesn't work");


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
  //!!\\ sample vs "frame" trickery

  pthread_detach(pthread_self());

  globalTime = 0.0;
  double dt = 1.0 / info.sampleRate;

  //init block
  EZ_Sample block[info.blockSize];
  snd_pcm_start(device);

  //main loop
  while (running) {

  //fill the block
  for (uint32_t i = 0; i < info.blockSize; i+= info.channels) {

    for (int chan = 0; chan < info.channels; chan++)
      block[i + chan] = note_mixer(globalTime, chan) + EZ_sfx_callback(globalTime, chan);
      //block[i + chan] = EZ_sfx_callback(globalTime, chan);

    globalTime += dt;
  }


   //render the block
   snd_pcm_uframes_t frames = info.blockSize / info.channels;
   EZ_Sample *current = block;

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




double EZ_sfx_sine(double time, double freq) {
  return sin(time*freq*PI*2.0);

}


double EZ_sfx_fastSine(double time, double freq) {

  double j = time*freq;                        // = time * 2 pi * f   / (2*pi)
  j -= (int)j;                                 //garder la partie décimale pour avoir la périodicité
  return 20.785 * j * (j - 0.5) * (j - 1.0f);  //approx de sin par x(x-pi)(x-2pi) en gros...

}


typedef LIST_OF(EZ_Note) node_t;
static node_t* note_list_head = NULL;


void EZ_sfx_play(EZ_Note* note) {

  node_t* new = (node_t*) malloc(sizeof(node_t));

  new->hd = *note;
  new->hd.startTime = globalTime;
  new->hd.endTime   = globalTime + note->length;
  
  new->tl = note_list_head; note_list_head = new;

}


static EZ_Sample note_mixer(double time, int channel) {

  double sf = 0;
  int length = 0;

  node_t* current = note_list_head;
  node_t* last = NULL;


  while (current != NULL) {

    sf += current->hd.oscillator(time, current->hd.pitch) * 0.25;
    length ++;


    //REMOVE ELEMENT FROM LIST
    if (time >= current->hd.endTime) {

      if (last == NULL) //IF CURRENT IS THE HEAD THEN CHANGE THE HEAD
        note_list_head = current->tl;

      else //OTHERWISE, CHANGE THE PREVIOUS LINK
        last->tl  = current->tl;
  

      free(current);

    }
    else {
      last = current;
    }

    current = current->tl;
  }



  return (EZ_Sample)(sf*SAMPLE_MAX);
}
