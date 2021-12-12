#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdlib.h>
#include <stdio.h>


void EZ_mesh_free(EZ_Mesh_t* mesh) {
	free(mesh->triangles);
	free(mesh);
}


typedef struct {int pos[3], uv[3];} _face;
typedef struct {float x,y,z;} _pos;
typedef struct {float u,v;} _uv;


static char _peak(FILE* fp) {

	char c = getc(fp);
	ungetc(c, fp);

	return c;
}


EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname) {



	/* open file in text mode */
	FILE *file = fopen(fname,"r");

	if (file == NULL) {
		ERR_warning("Couldn't open file %s", fname);
		return NULL;
	}



	/* 1ST PASS : COUNT HOW MANY THINGS ARE TO PARSE */



	char c;
	int nPos = 0, nFaces = 0, nUV = 0;

	/* go to next line, stop if end_of_file encountered */
	#define NEXTLINE() while(c != '\n' && c != EOF) c = getc(file);

	do {
		c = getc(file);

		switch (c) {

			case 'v' :
				if (_peak(file) == 't') nUV++;
				else nPos++;
			break;

			case 'f' :
				nFaces++;
			break;

			default : 

			break;
		}

		NEXTLINE();

	} while (c != EOF);



	fseek(file, 0, SEEK_SET);






	/* 2ND PASS : PARSING	*/


	/* allocate room for parsing */
	_pos* pos_buffer = malloc( nPos * sizeof(_pos) );
	_pos* pos = pos_buffer;

	_uv* uv_buffer = malloc( nUV * sizeof(_uv) );
	_uv* uv  = uv_buffer;

	_face* face_buffer = malloc( nFaces * sizeof(_face) );
	_face* face  = face_buffer;


	/* char read, to detect error */
	int read;

	do {

		c = getc(file);

		switch (c) {

		case 'v' :

			/* UV */
			if (_peak(file) == 't') {
				read = fscanf(file, "t %g %g", &uv->u, &uv->v);
				uv++;
			}

			/* POSITION */
			else {
				read = fscanf(file, "%g %g %g", &pos->x, &pos->y, &pos->z);
				pos++;
			}

		break;


		case 'f' : 

			for (int i = 0; i < 3; i++) {

				read = fscanf(file, "%d", &face->pos[i]);

				if (_peak(file) == '/')
					read += fscanf(file, "/%d", &face->uv[i]);					

			}

			face++;		

		break;



		default : 

		break;

		}
		

		NEXTLINE();

	} while (c != EOF);


	fclose(file);



	/* WRITE TO A MESH STRUCTURE */


	/* init mesh data */
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = calloc(nFaces, sizeof(EZ_Tri_t));
	mesh->nPoly = nFaces;

	int i,j;

	/* copy parsed data to mesh */
	for (i = 0; i < nFaces; i++) {
		_face indices = face_buffer[i];
		
		mesh->triangles[i].col = EZ_WHITE;

		for (j = 0; j < 3; j++) {

			/* indices start at 1 */
			if (nPos > 0){
				mesh->triangles[i].vert[j].pos.x = pos_buffer[indices.pos[j] - 1].x;
				mesh->triangles[i].vert[j].pos.y = pos_buffer[indices.pos[j] - 1].y;
				mesh->triangles[i].vert[j].pos.z = pos_buffer[indices.pos[j] - 1].z;
				mesh->triangles[i].vert[j].pos.w = 1.0;
			}

			if (nUV > 0) {
				mesh->triangles[i].vert[j].u     =  uv_buffer[indices.uv[j] - 1].u;
				mesh->triangles[i].vert[j].v     =  uv_buffer[indices.uv[j] - 1].v;
			}
		}

	}


	free(pos_buffer);
	free(uv_buffer);
	free(face_buffer);


	return mesh;


}


void EZ_mesh_saveOBJ(EZ_Mesh_t* mesh, const char* fname) {


	/* open file */
	FILE *file = fopen(fname,"w"); /* TEXT MODE */

	if (file == NULL) {
		ERR_warning("Couldn't save file %s", fname);
		return;
	}


	ERR_warning("Not implemented");

	fclose(file);

}





static const EZ_Vec_t cube_data[12][3] = {

		/* south */
		{ {{ -1.0f, -1.0f, -1.0f }}, {{ -1.0f,  1.0f, -1.0f }}, {{  1.0f,  1.0f, -1.0f }} },
		{ {{ -1.0f, -1.0f, -1.0f }}, {{  1.0f,  1.0f, -1.0f }}, {{  1.0f, -1.0f, -1.0f }} },

		/* east */
		{ {{  1.0f, -1.0f, -1.0f }}, {{  1.0f,  1.0f, -1.0f }}, {{  1.0f,  1.0f,  1.0f }} },
		{ {{  1.0f, -1.0f, -1.0f }}, {{  1.0f,  1.0f,  1.0f }}, {{  1.0f, -1.0f,  1.0f }} },

		/* north */
		{ {{  1.0f, -1.0f,  1.0f }}, {{  1.0f,  1.0f,  1.0f }}, {{ -1.0f,  1.0f,  1.0f }} },
		{ {{  1.0f, -1.0f,  1.0f }}, {{ -1.0f,  1.0f,  1.0f }}, {{ -1.0f, -1.0f,  1.0f }} },

		/* west */
		{ {{ -1.0f, -1.0f,  1.0f }}, {{ -1.0f,  1.0f,  1.0f }}, {{ -1.0f,  1.0f, -1.0f }} },
		{ {{ -1.0f, -1.0f,  1.0f }}, {{ -1.0f,  1.0f, -1.0f }}, {{ -1.0f, -1.0f, -1.0f }} },

		/* top */
		{ {{ -1.0f,  1.0f, -1.0f }}, {{ -1.0f,  1.0f,  1.0f }}, {{  1.0f,  1.0f,  1.0f }} },
		{ {{ -1.0f,  1.0f, -1.0f }}, {{  1.0f,  1.0f,  1.0f }}, {{  1.0f,  1.0f, -1.0f }} },

		/* bot */
		{ {{ -1.0f, -1.0f, -1.0f }}, {{  1.0f, -1.0f,  1.0f }}, {{ -1.0f, -1.0f,  1.0f }} },
		{ {{ -1.0f, -1.0f, -1.0f }}, {{  1.0f, -1.0f, -1.0f }}, {{  1.0f, -1.0f,  1.0f }} },

};


/* rubik's cube colors */
static const EZ_Px_t colors[] = {EZ_RED, EZ_YELLOW, EZ_ORANGE, EZ_WHITE, EZ_BLUE, EZ_GREEN};


EZ_Mesh_t* EZ_mesh_unitCube() {
	int i, j;

	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = malloc(12 * sizeof(EZ_Tri_t));
	mesh->nPoly = 12;

	for (i = 0; i < mesh->nPoly; i++) {
		mesh->triangles[i].col = colors[i/2];

		for (j = 0; j < 3; j++) {
			mesh->triangles[i].vert[j].pos = cube_data[i][j];
			mesh->triangles[i].vert[j].pos.w = 1.0;
		}
	}

	return mesh;

}