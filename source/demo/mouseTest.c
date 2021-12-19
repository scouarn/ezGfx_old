#include "ezGfx.h"


#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>	
#include <fcntl.h>		
#include <unistd.h>		

#define DEV_MOUSE   "/dev/input/event7"


int main() {

	int hMouse;


	/* Open mouse device */
	ERR_assert(-1 != (hMouse = open(DEV_MOUSE, O_RDONLY)), 
		"Couldn't open mouse device.");



	struct input_event event;

	while(0 != read(hMouse, &event, sizeof(struct input_event))  ) {

		printf("type %d\tcode %d\tvalue %d\n",
			event.type, event.code, event.value);

	}

	return 0;

}
