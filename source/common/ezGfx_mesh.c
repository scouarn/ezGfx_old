#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdlib.h>
#include <stdio.h>


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

		for (j = 0; j < 3; j++)
			mesh->triangles[i].pos[j] = cube_data[i][j];
	}

	return mesh;

}

void EZ_draw3D_freeMesh(EZ_Mesh_t* mesh) {
	free(mesh->triangles);
	free(mesh);
}


typedef struct {int x,y,z;} vec3i;


EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname) {

	char c;
	int i;

	/* go to next line, stop if end_of_file encountered */
	#define NEXTLINE() while(c != '\n' && c != EOF) c = getc(file);


	/* open file in text mode */
	FILE *file = fopen(fname,"r");

	if (file == NULL) {
		ERR_warning("Couldn't open file %s", fname);
		return NULL;
	}


	/*		1ST PASS : count number of things to parse	*/
	int n_vertices = 0;
	int n_faces = 0;
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
		NEXTLINE();
	}

	fseek(file, 0, SEEK_SET);


	/*		2ND PASS : parsing	*/


	/* allocate room for parsing vertices and triangles */
	EZ_Vec_t* vertex_buffer = calloc(n_vertices, sizeof(EZ_Vec_t)); /* 3 float coords */
	vec3i*    face_buffer   = calloc(n_faces,    sizeof(vec3i));    /* 3 integer indices (starts at 1!!) */

	/* vertex and triangle beeing parsed */
	EZ_Vec_t* v = vertex_buffer;
	vec3i*    t = face_buffer;

	/* char read, to detect error */
	int read;
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			read = fscanf(file, "%lg %lg %lg", &v->x, &v->y, &v->z);
			v++;

			if (read == 0) {
				ERR_warning("Error during vertex parsing in %s", fname);
				fclose(file);
				return NULL;
			}
		break;

		case 'f' : 
			read = fscanf(file, "%d %d %d", &t->x, &t->y, &t->z);
			t++;

			if (read == 0) {
				ERR_warning("Error during face parsing in %s", fname);
				fclose(file);

				return NULL;
			}
		break;

		default : break;

		}
		NEXTLINE();
	}


	fclose(file);


	/* init mesh data */
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->nPoly = n_faces;
	mesh->triangles = calloc(n_faces, sizeof(EZ_Tri_t));


	/* copy parsed data to mesh */
	for (i = 0; i < n_faces; i++) {
		vec3i indices = face_buffer[i];

		/* indices start at 1 */
		mesh->triangles[i].pos[0] = vertex_buffer[indices.x - 1];
		mesh->triangles[i].pos[1] = vertex_buffer[indices.y - 1];
		mesh->triangles[i].pos[2] = vertex_buffer[indices.z - 1];

		mesh->triangles[i].col = EZ_WHITE;
	}


	free(vertex_buffer);
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

