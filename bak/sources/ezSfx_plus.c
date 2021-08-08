#include "ezSfx_plus.h"
#include <math.h>

#include <stdio.h>

static double globalTime;

typedef struct node {
  EZ_sfx_note note;
  struct node *next;

} EZ_sfx_listNode;
static EZ_sfx_listNode* listHead = NULL;


EZ_sample EZ_sfx_play(EZ_sfx_note note) {

	EZ_sfx_listNode* new = (EZ_sfx_listNode*) malloc(sizeof(EZ_sfx_listNode));

	new->note = note;
	new->note.startTime = globalTime;
	new->note.endTime = new->note.startTime + new->note.length;
	new->next = listHead;

	listHead = new;

}



EZ_sample EZ_sfx_plusCallback(double time, int channel) {

	globalTime = time;

  double sf = 0;
	int length = 0;

	EZ_sfx_listNode* current = listHead;
	EZ_sfx_listNode* last = NULL;


	while (current != NULL) {

		sf += current->note.callback(time, current->note.pitch) * 0.25;
		length ++;


		//REMOVE ELEMENT FROM LIST
		if (time >= current->note.endTime) {

			if (last == NULL) { //IF CURRENT IS THE HEAD THEN CHANGE THE HEAD
				listHead = current->next;
			}
			else { //OTHERWISE, CHANGE THE PREVIOUS LINK
				last->next  = current->next;
			}

      free(current);

		}
		else {
			last = current;
		}

		current = current->next;
	}



  return (EZ_sample)(sf*SAMPLE_MAX);
}




double EZ_sfx_sine(double time, double freq) {
	return sin(time*freq*PI*2.0);

}


double EZ_sfx_fastSine(double time, double freq) {

	double j = time*freq;                        // = time * 2 pi * f   / (2*pi)
	j -= (int)j;                                 //garder la partie décimale pour avoir la périodicité
  return 20.785 * j * (j - 0.5) * (j - 1.0f);  //approx de sin par x(x-pi)(x-2pi) en gros...

}
