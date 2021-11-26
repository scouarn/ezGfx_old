#include "ezGfx.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>



/*	INDEX :
**		
**		-PSFU FONTS
**		-BMP IMAGES
**		-OBJ MODELS
*/




/* https://wiki.osdev.org/PC_Screen_Font */

#define PSF_FONT_MAGIC 0x864ab572

struct __attribute__((__packed__)) PSF_font_header {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};


EZ_Font_t* EZ_load_PSF2(const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"rb");
	EZ_Font_t* font = malloc( sizeof(EZ_Font_t) );

	if (file == NULL) {
		EZ_throw("Couldn't load file", fname);
		font->data = NULL;
		return font;
	}


	/* read header */
	struct PSF_font_header header;
	fread(&header, sizeof(struct PSF_font_header), 1, file);

	if (header.magic != PSF_FONT_MAGIC) {
		EZ_throw("Couldn't load file (corrupted)", fname);
		fclose(file);
		font->data = NULL;
		return font;
	}

	/* read font */
	font = EZ_createFont(header.width, header.height);
	fread(font->data[0], header.bytesperglyph, 256, file);


	fclose(file);
	return font;

}



void EZ_save_PSF2(EZ_Font_t* font, const char* fname) {

	/* open file */
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	/* init header */
	struct PSF_font_header header;
	header.magic = PSF_FONT_MAGIC;
	header.headersize = 32;
	header.numglyph = 256;
	header.bytesperglyph = font->h * font->wb;
	header.width  = font->w;
	header.height = font->h;

	/* write */
	fwrite(&header, sizeof(struct PSF_font_header), 1, file);
	fwrite(font->data[0], header.bytesperglyph, 256, file);

	fclose(file);

}




EZ_Image_t* EZ_load_BMP(const char* fname) {

	EZ_Image_t* bitmap = malloc( sizeof(EZ_Image_t) );

	/* open file */
	FILE *file = fopen(fname,"rb");
	if (file == NULL) {
		EZ_throw("Couldn't load file", fname);
		bitmap->px = NULL;
		return bitmap;
	}


	/* skip n bytes */
	#define POP(n) for (int i = 0; i < n; i++) getc(file);


	uint32_t fsize, offset;

	/* read file header (fixed 14 bytes) */
	POP(2);
	fread(&fsize, 4, 1, file);
	POP(4);
	fread(&offset, 4, 1, file);


	
	/* read DIB header */
	uint32_t header_size, image_size;
	uint16_t depth;


	fread( &header_size, 4, 1, file);
	fread( &(bitmap->w), 4, 1, file);
	fread( &(bitmap->h), 4, 1, file);
	POP(2); /* planes */
	fread(&depth, 2, 1, file);
	int bytes_ppx = depth / 8;

	POP(4); /* compression */
	fread(&image_size, 4, 1, file);
	POP(16); /* resolution and color map info */


	uint32_t r_mask, g_mask, b_mask, a_mask;

	fread(&r_mask, 4, 1, file);
	fread(&g_mask, 4, 1, file);
	fread(&b_mask, 4, 1, file);
	fread(&a_mask, 4, 1, file);

	if (depth != 24 && depth != 32) EZ_error("Unsupported pixel format");
	if (a_mask == 0) EZ_throw("No alpha channel", fname);

	
	/* read pixels */

	bitmap->px = calloc( bitmap->w * bitmap->h, sizeof(EZ_Px_t) );
	fseek(file, offset, SEEK_SET); /* offset from begining */

	int bytes_per_line = image_size / bitmap->h;

	uint8_t* line_buffer = malloc( bytes_per_line );

	int x, y, c;
	for (y = bitmap->h-1; y >= 0; y--) { /* y axis is reversed in bitmap format (bottom to top) */
		fread(line_buffer, 1, bytes_per_line, file);

		for (x = 0; x < bitmap->w; x++) {
			EZ_Px_t* px = &(bitmap->px[x + y*bitmap->w]);

			/* test each color byte against each byte of each bitmask */
			for (c = 0; c < bytes_ppx; c++) {
				px->r |= line_buffer[x*bytes_ppx + c] & (r_mask >> 8*c);
				px->g |= line_buffer[x*bytes_ppx + c] & (g_mask >> 8*c);
				px->b |= line_buffer[x*bytes_ppx + c] & (b_mask >> 8*c);
				px->a |= line_buffer[x*bytes_ppx + c] & (a_mask >> 8*c);
			}

			/* set max transparency if it's not defined in the image */
			if (a_mask == 0) px->a = 255;

			}
	}

	fclose(file);
	free(line_buffer);

	return bitmap;


}

void EZ_save_BMP(EZ_Image_t* img, const char* fname) {


	/* open file */
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	/* write file header */
	const uint32_t zero = 0x0, one  = 0x1;
	const uint32_t img_size = img->w * img->h * 4;
	const uint32_t offset = 138;
	const uint32_t f_size = offset + img_size;

	fputs("BM", file);           /* signature */
	fwrite(&f_size, 4, 1, file); /* file size */
	fwrite(&zero, 4, 1, file);   /* reserved */
	fwrite(&offset, 4, 1, file); /* offset */



	/* write DIB header */

	const uint32_t h_size = 124;
	const uint16_t planes = 1;
	const uint16_t depth = 32;
	const uint32_t comp = 3;
	const uint32_t intent = 2;

	const uint32_t r_mask = 0xff000000; /* unusual masking ? */
	const uint32_t g_mask = 0x00ff0000;
	const uint32_t b_mask = 0x0000ff00;
	const uint32_t a_mask = 0x000000ff;

	fwrite(&h_size,   4, 1, file); /* header size */
	fwrite(&img->w,    4, 1, file); /* width */
	fwrite(&img->h,    4, 1, file); /* height */
	fwrite(&planes,   2, 1, file); /* planes */
	fwrite(&depth,    2, 1, file); /* depth */
	fwrite(&comp,     4, 1, file); /* compression */
	fwrite(&img_size, 4, 1, file); /* img size in bytes */
	fwrite(&one,      4, 1, file); /* res */
	fwrite(&one,      4, 1, file); /* res */
	fwrite(&zero,     4, 1, file); /* palete */
	fwrite(&zero,     4, 1, file); /* palete */
	fwrite(&r_mask,   4, 1, file); /* colormask */
	fwrite(&g_mask,   4, 1, file); /* colormask */
	fwrite(&b_mask,   4, 1, file); /* colormask */
	fwrite(&a_mask,   4, 1, file); /* colormask */

	fputs("BGRs", file); /* color space type */

	int i;

	for (i = 0; i < 36; i++) /* color space endpoints (36 zero bytes) */
		fputc('\0', file); 

	for (i = 0; i < 3; i++) /* gamma (int 0 for rgb) */
		fwrite(&zero, 4, 1,  file); 

	fwrite(&intent, 4, 1, file); /* intent */

	for (i = 0; i < 3; i++) /* ICC */
		fwrite(&zero, 4, 1,  file); 


	/* px data */
	/* line per line */
	/* no padding, RGBA already 32bits 4bytes-aligned */

	for (i = img->h - 1; i >= 0; i--) //y axis mirroring
		fwrite(img->px + i*img->w, sizeof(EZ_Px_t), img->w,  file); 


	//close
	fclose(file);


}






#ifdef EZ_DRAW3D

EZ_Mesh EZ_load_OBJ(const char* fname) {

	//open file
	FILE *file = fopen(fname,"r");
	EZ_Mesh mesh;

	if (file == NULL) {
		WARNING("Couldn't load file %s\n", fname);
		mesh.triangles = NULL;
		mesh.texture = NULL;
		return mesh;
	}


	//char beeing read
	char c;

	//go to next line, stop if end_of_file encountered
	#define NEXT() while(c != '\n') if (c == EOF) break; else c = getc(file);



	/*		1ST PASS		*/

	//number of entries in the file
	int n_vertices = 0;
	int n_faces = 0;


	//count number of things to parse
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			n_vertices++;
		break;

		case 'f' :
			n_faces++;
		break;

		default : break;

		}
		NEXT();
	}


	//return to the begining of the file
	fseek(file, 0, SEEK_SET);




	/*		2ND PASS		*/


	//allocate room for parsing vertices and triangles
	vec3f* vertex_buffer = calloc(n_vertices, sizeof(vec3f)); //3 float coords
	vec3i* face_buffer   = calloc(n_faces, sizeof(vec3i));    //3 integer indices (starts at 1!!)

	//vertex and triangle beeing parsed
	vec3f* v = vertex_buffer;
	vec3i* t = face_buffer;

	//char read, to detect error
	int read;
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			read = fscanf(file, "%f %f %f", &v->x, &v->y, &v->z);
			if (read == 0) {
				WARNING("Error during vertex parsing in mesh %s\n", fname);
				fclose(file);
				mesh.triangles = NULL;
				mesh.texture = NULL;
				return mesh;
			}
			v++;
		break;

		case 'f' : 
			read = fscanf(file, "%d %d %d", &t->x, &t->y, &t->z);
			if (read == 0) {
				WARNING("Error during face parsing in mesh %s\n", fname);
				fclose(file);
				mesh.triangles = NULL;
				mesh.texture = NULL;
				return mesh;
			}
			t++;
		break;

		default : break;

		}
		NEXT();
	}


	fclose(file);


	//init mesh data
	mesh.nPoly = n_faces;
	mesh.triangles = calloc(n_faces, sizeof(EZ_Tri));

	if (mesh.triangles == NULL) {
		WARNING("Couldn't allocate memory for mesh %s\n", fname);
		mesh.triangles = NULL;
		mesh.texture = NULL;
		return mesh;
	}


	//copy parsed data to mesh
	for (int i = 0; i < n_faces; i++) {

		vec3i indices = face_buffer[i];

		//!!\\ indices start at 1
		mesh.triangles[i].points[0].pos = vertex_buffer[indices.x - 1];
		mesh.triangles[i].points[1].pos = vertex_buffer[indices.y - 1];
		mesh.triangles[i].points[2].pos = vertex_buffer[indices.z - 1];

		mesh.triangles[i].col = EZ_WHITE;
	}


	//debug coordinates

	// for (int i = 0; i < n_faces; i++) {
	// 	printf("triangle #%d :\n", i);

	// 	for (int j = 0; j < 3; j++) {
	// 		vec3f pos = mesh.triangles[i].points[j].pos;
	// 		printf("%f %f %f\n", pos.x, pos.y, pos.z);
	// 	}
	// 	printf("\n");

	// }

	// printf("%d vertices, %d triangles\n", n_vertices, n_faces);

	free(vertex_buffer);
	free(face_buffer);


	return mesh;


}


void EZ_save_OBJ(const EZ_Mesh mesh, const char* fname) {


	//open file
	FILE *file = fopen(fname,"w"); //TEXT MODE

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	WARNING("Not implemented");

	fclose(file);

}


#endif



