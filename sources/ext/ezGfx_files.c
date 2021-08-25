#include "ezGfx.h"

#include <stdio.h>
#include <stdlib.h>



/*	INDEX :
**		
**		-PSFU FONTS
**		-WAVE AUDIO
**		-BMP IMAGES
**		-OBJ MODELS
*/




//https://wiki.osdev.org/PC_Screen_Font

#define PSF_FONT_MAGIC 0x864ab572

struct PSF_font_header {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
};


EZ_Font EZ_load_PSF2(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_Font font;

	if (file == NULL) {
		WARNING("Couldn't load file %s\n", fname);
		return font;
	}


	//read header
	struct PSF_font_header header;
	fread(&header, sizeof(struct PSF_font_header), 1, file);

	if (header.magic != PSF_FONT_MAGIC) {
		WARNING("Wrong magic number in %s", fname);
		fclose(file);
		return font;
	}


	//setup font
	font = EZ_createFont(header.width, header.height);


	if (font.data[0] == NULL) {
		WARNING("Couldn't allocate memory for font %s", fname);
		fclose(file);
		return font;
	}

	//read font data
	fread(font.data[0], header.bytesperglyph, 256, file);

	//close
	fclose(file);
	return font;
}



void EZ_save_PSF2(const EZ_Font font, const char* fname) {

	//open file
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	//init header
	struct PSF_font_header header;
	header.magic = PSF_FONT_MAGIC;
	header.headersize = 32;
	header.numglyph = 256;
	header.bytesperglyph = font.h_px * font.w_bytes;
	header.height = font.h_px;
	header.width  = font.w_px;

	//write header
	fwrite(&header, sizeof(struct PSF_font_header), 1, file);


	//write font data
	int written = fwrite(font.data[0], header.bytesperglyph, 256, file);

	//error check
	ASSERTW(written > 0, "Nothing was written to %s", fname);

	//close
	fclose(file);

}








#ifdef EZSFX_CORE


//http://soundfile.sapp.org/doc/WaveFormat/
//https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f

//Wave file header

struct riff_header {

		//RIFF chunk descriptor
		int8_t  ChunkID[4];  //"RIFF" -> 0x52494646
		int32_t ChunkSize;   //Size of the rest of the file
		int8_t  Format[4];   //"WAVE" -> 0x57415645
};

struct fmt_subchunk {

		int8_t  SubchunkID[4]; // "fmt " -> 0x666d7420
		int32_t SubchunkSize;  // Should be 16 for PCM
		int16_t AudioFormat;   // 1 -> PCM
		int16_t NumChannels;
		int32_t SampleRate;
		int32_t ByteRate;      // SampleRate * NumChannels * sizeof(sample)
		int16_t BlockAlign;    // NumChannels * sizeof(sample) -> bytes per alsa "frame"
		int16_t BitsPerSample; // 8 * sizeof(sample)
};

struct data_subchunk {

		int8_t  SubchunkID[4]; // "data" -> 0x64617461
		int32_t SubchunkSize;  //Number of bytes in data. 
		// -> samples * num_channels * sizeof(sample)
};




EZ_PCMArray EZ_load_WAV(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_PCMArray arr;

	if (file == NULL) {
		WARNING("Couldn't open file %s\n", fname);
		return arr;
	}


	//read riff header
	struct riff_header riff;
	fread(&riff, sizeof(struct riff_header), 1, file);

	if (*(uint32_t*)riff.ChunkID != 0x46464952) {
		WARNING("Wrong magic number ('RIFF') in %s",fname);
		fclose(file);
		return arr;
	}

	if (*(uint32_t*)riff.Format != 0x45564157) {
		WARNING("Wrong magic number ('WAVE') in %s",fname);
		fclose(file);
		return arr;
	}


	//read ftm chunk
	struct fmt_subchunk fmt;
	fread(&fmt, sizeof(struct fmt_subchunk), 1, file);

	if (*(uint32_t*)fmt.SubchunkID != 0x20746d66) {
		WARNING("Wrong magic number ('fmt ') in %s",fname);
		fclose(file);
		return arr;
	}

	if (fmt.SubchunkSize != 16 || fmt.AudioFormat != 1) {
		WARNING("Only PCM wave file are supported, %s", fname);
		fclose(file);
		return arr;
	}

	if (fmt.NumChannels == 0) {
		WARNING("This audio file has 0 channels %s", fname);
		fclose(file);
		return arr;
	}
	else arr.channels = fmt.NumChannels;

	if (fmt.SampleRate == 0 || fmt.ByteRate == 0) {
		WARNING("This audio file has a sample rate of 0... %s", fname);
		fclose(file);
		return arr;
	}
	else arr.sampleRate = fmt.SampleRate;

	if (fmt.BitsPerSample != 16) {
		WARNING("Only 16bits signed integer audio is supported, %s", fname);
		fclose(file);
		return arr;
	}


	//read data chunk
	struct data_subchunk data;
	fread(&data, sizeof(struct data_subchunk), 1, file);

	if (*(uint32_t*)data.SubchunkID != 0x61746164) {
		WARNING("Wrong magic number ('data') in %s",fname);
		fclose(file);
		return arr;
	}

	if (data.SubchunkSize == 0) {
		WARNING("This audio file has no samples, %s", fname);
		fclose(file);
		return arr;
	}
	else arr.size = data.SubchunkSize / fmt.NumChannels;


	//read samples
	arr.data = malloc(data.SubchunkSize);

	if (arr.data == NULL) {
		WARNING("Couldn't allocate memory for file, %s", fname);
		fclose(file);
		return arr;
	}
	else arr.curr = arr.data;
	
	fread(arr.data, 1, data.SubchunkSize, file);


	return arr;

}


void EZ_save_WAV(const EZ_PCMArray arr, const char* fname) {

	//open file
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	//write headers
	struct riff_header riff = {
		.ChunkID = "RIFF",
		.ChunkSize = 36 + arr.size * sizeof(EZ_Sample),
		.Format  = "WAVE"
	};

	fwrite(&riff, sizeof(struct riff_header), 1, file);


	struct fmt_subchunk fmt = {
		.SubchunkID = "fmt ",
		.SubchunkSize = 16,
		.AudioFormat = 1,
		.NumChannels = arr.channels,
		.SampleRate  = arr.sampleRate,
		.ByteRate = arr.channels * arr.sampleRate * sizeof(EZ_Sample),
		.BlockAlign = arr.channels * sizeof(EZ_Sample),
		.BitsPerSample = 8 * sizeof(EZ_Sample)
	};

	fwrite(&fmt, sizeof(struct fmt_subchunk), 1, file);


	struct data_subchunk data = {
		.SubchunkID = "data",
		.SubchunkSize = arr.size * sizeof(EZ_Sample)
	};

	fwrite(&data, sizeof(struct data_subchunk), 1, file);


	//write pcm data
	fwrite(arr.data, sizeof(EZ_Sample), arr.size, file);

}



/* DEPRECATED
EZ_PCMArray EZ_load_PCM(const char* fname) {

	//open file
	FILE *file = fopen(fname,"rb");
	EZ_PCMArray arr;

	if (file == NULL) {
		WARNING("Couldn't open file %s\n", fname);
		return arr;
	}

	//get the size
	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (fileSize == 0) {
		WARNING("File %s was of size 0", fname);
		return arr;
	}


	//init data
	arr.data = malloc(fileSize);

	if (arr.data == NULL) {
		WARNING("Couldn't allocate memory for PCM %s", fname);
		fclose(file);
		return arr;
	}

	//set array parameters
	arr.curr = arr.data;
	arr.size = fileSize / sizeof(EZ_Sample);
	arr.sampleRate = CD44KHZ; //CANT TELL
	arr.channels = 2; //CANT TELL

	//read samples
	fread(arr.data, sizeof(EZ_Sample), arr.size, file);

	//return
	fclose(file);
	return arr;
}


void EZ_save_PCM(const EZ_PCMArray arr, const char* fname) {}
*/


#endif











//variable header size ?

EZ_Image EZ_load_BMP(const char* fname) {

	EZ_Image bitmap;

	//open file
	FILE *file = fopen(fname,"rb");

	//check if opened
	if (file == NULL) {
		WARNING("Couldn't load file %s", fname);
		bitmap.px = NULL;
		return bitmap;
	}


	//Skip n bytes
	#define POP(n) for (int i = 0; i < n; i++) getc(file);


	uint32_t fsize, offset;

	//read file header (fixed 14 bytes)
	POP(2);
	fread(&fsize, 4, 1, file);
	POP(4);
	fread(&offset, 4, 1, file);


	
	//read DIB header
	uint32_t header_size, image_size;
	uint16_t depth;


	fread(&header_size, 4, 1, file);
	fread(&bitmap.w, 4, 1, file);
	fread(&bitmap.h, 4, 1, file);
	POP(2); //planes
	fread(&depth, 2, 1, file);
	int bytes_ppx = depth / 8;

	POP(4); //compression
	fread(&image_size, 4, 1, file);
	POP(16); //resolution and color map info


	uint32_t r_mask, g_mask, b_mask, a_mask;

	fread(&r_mask, 4, 1, file);
	fread(&g_mask, 4, 1, file);
	fread(&b_mask, 4, 1, file);
	fread(&a_mask, 4, 1, file);

	if (depth != 24 && depth != 32) ERROR("Unsupported %dbits pixel format", depth);
	if (a_mask == 0) WARNING("No alpha channel in file %s", fname);

	
	//read pixels

	bitmap.px = calloc(bitmap.w*bitmap.h, sizeof(EZ_Px));

	if (bitmap.px == NULL) {
		WARNING("Couldn't allocate memory for image %s\n", fname);
		fclose(file);
		return bitmap;
	}


	fseek(file, offset, SEEK_SET); //offset from begining

	int bytes_per_line = image_size / bitmap.h;

	uint8_t line[bytes_per_line]; //line buffer

	for (int y = bitmap.h-1; y >= 0; y--) { //y axis is reversed in bitmap format (bottom to top)
		fread(line, 1, bytes_per_line, file);

		for (int x = 0; x < bitmap.w; x++) {

			EZ_Px* px = &bitmap.px[x+y*bitmap.w];

			//test each color byte against each byte of each bitmask
			for (int c = 0; c < bytes_ppx; c++) {
				px->col.r |= line[x*bytes_ppx + c] & (r_mask >> 8*c);
				px->col.g |= line[x*bytes_ppx + c] & (g_mask >> 8*c);
				px->col.b |= line[x*bytes_ppx + c] & (b_mask >> 8*c);
				px->col.a |= line[x*bytes_ppx + c] & (a_mask >> 8*c);
			}

			//set max transparency if it's not defined in the image
			if (a_mask == 0) px->col.a = 255;

			}
	}


	//close
	fclose(file);

	#undef POP
	return bitmap;


}

void EZ_save_BMP(const EZ_Image img, const char* fname) {


	//open file
	FILE *file = fopen(fname,"wb");

	if (file == NULL) {
		WARNING("Couldn't save file %s\n", fname);
		return;
	}


	//write file header
	const uint32_t zero = 0x0, one  = 0x1;
	const uint32_t img_size = img.w*img.h * 4;
	const uint32_t offset = 138;
	const uint32_t f_size = offset + img_size;

	fputs("BM", file); //signature
	fwrite(&f_size, 4, 1, file); //file size
	fwrite(&zero, 4, 1, file); //reserved
	fwrite(&offset, 4, 1, file); //offset



	//write DIB header

	const uint32_t h_size = 124;
	const uint16_t planes = 1;
	const uint16_t depth = 32;
	const uint32_t comp = 3;
	const uint32_t intent = 2;

	const uint32_t r_mask = 0xff000000; //unusual masking ?
	const uint32_t g_mask = 0x00ff0000;
	const uint32_t b_mask = 0x0000ff00;
	const uint32_t a_mask = 0x000000ff;

	fwrite(&h_size,   4, 1, file); //header size
	fwrite(&img.w,    4, 1, file); //width
	fwrite(&img.h,    4, 1, file); //height
	fwrite(&planes,   2, 1, file); //planes
	fwrite(&depth,    2, 1, file); //depth
	fwrite(&comp,     4, 1, file); //compression
	fwrite(&img_size, 4, 1, file); //img size in bytes
	fwrite(&one,      4, 1, file); //res
	fwrite(&one,      4, 1, file); //res
	fwrite(&zero,     4, 1, file); //palete
	fwrite(&zero,     4, 1, file); //palete
	fwrite(&r_mask,   4, 1, file); //colormask
	fwrite(&g_mask,   4, 1, file); //colormask
	fwrite(&b_mask,   4, 1, file); //colormask
	fwrite(&a_mask,   4, 1, file); //colormask

	fputs("BGRs", file); //color space type

	for (int i = 0; i < 36; i++) //color space endpoints (36 zero bytes)
		fputc('\0', file); 

	for (int i = 0; i < 3; i++) //gamma	(int 0 for rgb)
		fwrite(&zero, 4, 1,  file); 

	fwrite(&intent, 4, 1, file); //intent

	for (int i = 0; i < 3; i++) //ICC
		fwrite(&zero, 4, 1,  file); 


	//px data
	
	//line per line
	//no padding, RGBA already 32bits 4bytes-aligned 

	for (int y = img.h-1; y >= 0; y--) //y axis mirroring
		fwrite(img.px + y*img.w, sizeof(EZ_Px), img.w,  file); 


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
	vec3f vertex_buffer[n_vertices]; //3 float coords
	vec3i face_buffer[n_faces];  //3 integer indices (1 indexed!!)

	//vertex and triangle beeing parsed
	vec3f *v = vertex_buffer;
	vec3i *t = face_buffer;

	//char read, to detect error
	int read;
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			read = fscanf(file, "%f %f %f", &v->x, &v->y, &v->z);
			if (read == 0) {
				WARNING("Error during vertex parsing in mesh %s\n", fname);
				fclose(file);
				return mesh;
			}
			v++;
		break;

		case 'f' : 
			read = fscanf(file, "%d %d %d", &t->x, &t->y, &t->z);
			if (read == 0) {
				WARNING("Error during face parsing in mesh %s\n", fname);
				fclose(file);
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



