#include "ezGfx_pixel.h"
#include <stdlib.h>

EZ_Px_t EZ_randCol() {
	EZ_Px_t col;

	/* col.ref is larger than RAND_MAX ... */
	col.r = (uint8_t)rand();
	col.g = (uint8_t)rand();
	col.b = (uint8_t)rand();
	
	return col;
}


EZ_Px_t EZ_blend(EZ_Px_t fg, EZ_Px_t bg, EZ_BlendMode_t mode) {

	/* https://love2d.org/wiki/BlendMode_Formulas */
	EZ_Px_t result;

	switch (mode) {
		case ALPHA_IGNORE : result = fg; break;
		case ALPHA_FAST   : result = (fg.a == 0) ? bg : fg; break;

		default :
		case ALPHA_BLEND  : 
		
		/* fast integer linear interpolation */
		result.a = fg.a + (255 - fg.a) * bg.a;
		result.r = (fg.a * fg.r + (255 - fg.a) * bg.r) >> 8;
		result.g = (fg.a * fg.g + (255 - fg.a) * bg.g) >> 8;
		result.b = (fg.a * fg.b + (255 - fg.a) * bg.b) >> 8;

		break;
	}

	return result;
}


