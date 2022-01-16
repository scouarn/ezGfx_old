#include "ezGfx_shader.h"

#include <stdlib.h>


void EZ_shader_textured(EZ_3DRenderParam_t* p) {



	/* depth buffering */
	if (*p->zloc > p->z) {
		return;
	}
	else {
		*p->zloc = p->z;
	}

	if (p->tri->mat->tex == NULL) {
		*p->px = EZ_MAGENTA;
		return;	
	}
	
	EZ_Px_t sample = *EZ_image_samplef(p->tri->mat->tex, p->u, p->v);

	if (sample.a == 0) return;


	/* apply shading */
	p->px->r = sample.r * p->tri->illum;
	p->px->g = sample.g * p->tri->illum;
	p->px->b = sample.b * p->tri->illum;

}



void EZ_shader_flat(EZ_3DRenderParam_t* p) {

	if (p->tri->mat->col.a == 0) return;

	/* depth buffering */
	if (*p->zloc > p->z) {
		return;
	}
	else {
		*p->zloc = p->z;
	}

	/* apply shading */
	p->px->r = p->tri->mat->col.r * p->tri->illum;
	p->px->g = p->tri->mat->col.g * p->tri->illum;
	p->px->b = p->tri->mat->col.b * p->tri->illum;

}

