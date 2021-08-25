# ezGfx
Semi-private repo on a simple cross-platform graphics library project. It also include some related tools like a font editor (Windows is not yet supported). Detailed documentation is not yet available, read comments inside header files. 


## Configuration and building

Inside _config.mk_ choose the used implementation and its dependencies based on the target platform. Only the "core" source files are plateform dependent. You can change the compiler and flags as you desire.

| Platform | Implementation 				| Dependencies  |		 Comment 				|
|--------|----------------------------------|---------------|-------------------------------|
| Linux	 | `ezGfx_core_linux_xorg`	    	|			 	| Uses a Xorg server		    |
| Linux	 | `ezGfx_core_linux_xorgGL`	    | `-lX11`		| Uses openGL					|
| Linux	 | `ezGfx_core_linux_void`	    	| `-lX11 -lGL`	| Uses the Linux framebuffer	|
| Linux	 | `ezGfx_sound_core_linux_alsa`	| `-lasound`	| Uses Alsa for sound	        |


From this directory (./) :

* To build the library and the tools call `make all`.
* To only rebuild the object file call `make lib`.
* To only rebuild the tools call `make tools`.
* To clean everything call `make clean`.



## Usage as a library

When compiling, link the compliled objects located in the _bin_ directory and include the _include_ directory.


## Usage as a framework

Make a project subdirectory inside the _tools_ directory, following the _template_ example. By calling `make all` and/or `make clean` from the main directory (where this readme file is located ./), it is called in each subdirectory of the _tool_ directory (with the same targets). In this way you can tweak the library and everything is recompiled as needed (the tools and your project as well). You can copy the generic Makefile from the template.


## General usage

Include the library's headers with `#include <ezGfx.h>`. The library expects you to define **all** of the following functions inside your project (or you will have a troubles when linking...) :
```C
void EZ_callback_init() {}
void EZ_callback_draw(double dt) {}
void EZ_callback_keyPressed(EZ_Key key) {}
void EZ_callback_keyReleased(EZ_Key key) {}
void EZ_callback_mouseMoved(EZ_Mouse mouse) {}
void EZ_callback_kill() {}
EZ_Sample EZ_sfx_callback(double time, int channel) {}
```

Nothe : they are _declared_ as `extern`. 



The graphics engine thread can be started with `EZ_start();`, call `EZ_join();` to wait for it to stop and `EZ_stop()` to force it to stop.

To start the sound engine thread you first have to configure it with `void EZ_sfx_init(int sampleRate, int channels, int blockQueueLength, int blockSize);` then use `EZ_sfx_start();`, `EZ_sfx_join();` and `EZ_sfx_stop();`.






