all :
	gcc main.c ezGfx_core_linux.c ezGfx_draw2D.c -o demo -lX11 -lGL -lpthread -lm
	./demo
