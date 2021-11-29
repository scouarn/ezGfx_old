#ifndef _EZGFX_PIXEL_H_
#define _EZGFX_PIXEL_H_

#include <stdint.h>

typedef union {
	uint32_t ref;  /* the 32bits hex value */
	struct {uint8_t a, b, g, r;};
} EZ_Px_t;


typedef enum { ALPHA_BLEND,  /* standart alpha blending */
               ALPHA_IGNORE, /* no blending at all */
               ALPHA_FAST	 /* ignore alpha if bigger than 0 */
} EZ_BlendMode_t;



EZ_Px_t EZ_blend(EZ_Px_t fg, EZ_Px_t bg, EZ_BlendMode_t mode);
EZ_Px_t EZ_randCol();	/* random white noise RGB color */

#define EZ_BLACK   ((EZ_Px_t)0x000000FFU) 
#define EZ_WHITE   ((EZ_Px_t)0xFFFFFFFFU)
#define EZ_BLUE    ((EZ_Px_t)0x0000FFFFU)
#define EZ_GREEN   ((EZ_Px_t)0x00FF00FFU)
#define EZ_CYAN    ((EZ_Px_t)0x00FFFFFFU)
#define EZ_RED     ((EZ_Px_t)0xFF0000FFU)
#define EZ_MAGENTA ((EZ_Px_t)0xFF00FFFFU)
#define EZ_YELLOW  ((EZ_Px_t)0xFFFF00FFU)
#define EZ_ORANGE  ((EZ_Px_t)0xFF7F00FFU)
#define EZ_LIME    ((EZ_Px_t)0x7FFF00FFU)
#define EZ_PURPLE  ((EZ_Px_t)0x7F00FFFFU)
#define EZ_GRAY	   ((EZ_Px_t)0x7F7F7FFFU)


#define EZ_RGB(r,g,b) 	 ({.a = 255, .b = b, .g = g, .r = r})
#define EZ_RGBA(r,g,b,a) ({.a =   a, .b = b, .g = g, .r = r})
#define EZ_BW(c) 		 ({.a = 255, .b = c, .g = c, .r = c})




#endif /* ezGfx_pixel_h */