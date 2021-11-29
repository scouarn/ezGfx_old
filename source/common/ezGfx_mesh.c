#include "ezGfx_utils.h"
#include "ezGfx_mesh.h"

#include <stdlib.h>
#include <stdio.h>


static const vec3f cube_data[12][3] = {

		/* south */
		{ { -1.0f, -1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } },

		/* east */
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f } },
		{ {  1.0f, -1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f } },

		/* north */
		{ {  1.0f, -1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f } },
		{ {  1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f } },

		/* west */
		{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f, -1.0f } },
		{ { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },

		/* top */
		{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f,  1.0f } },
		{ { -1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f,  1.0f }, {  1.0f,  1.0f, -1.0f } },

		/* bot */
		{ { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f } },
		{ { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f,  1.0f } },

};


/* rubik's cube colors */
static const EZ_Px_t colors[] = {EZ_RED, EZ_YELLOW, EZ_ORANGE, EZ_WHITE, EZ_BLUE, EZ_GREEN};


EZ_Mesh_t* EZ_draw3D_unitCube() {
	int i, j;

	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = malloc(12 * sizeof(EZ_Tri_t));
	mesh->nPoly = 12;

	for (i = 0; i < mesh->nPoly; i++) {
		mesh->triangles[i].col = colors[i/2];

		for (j = 0; j < 3; j++)
			mesh->triangles[i].points[j].pos = cube_data[i][j];
	}

	return mesh;

}

void EZ_draw3D_freeMesh(EZ_Mesh_t* mesh) {
	free(mesh->triangles);
	free(mesh);
}




EZ_Mesh_t* EZ_load_OBJ(const char* fname) {

	char c;
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );

	int n_vertices = 0;
	int n_faces = 0;

	FILE *file;


	/* go to next line, stop if end_of_file encountered */
	#define NEXT() while(c != '\n') if (c == EOF) break; else c = getc(file);



	/* open file in text mode */
	file = fopen(fname,"r");

	if (file == NULL) {
		EZ_throw("Couldn't load file", fname);
		return NULL;
	}




	



	/*		1ST PASS : count number of things to parse	*/

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


	fseek(file, 0, SEEK_SET);

	/*		2ND PASS : parse	*/


	/* allocate room for parsing vertices and triangles */
	vec3f* vertex_buffer = calloc(n_vertices, sizeof(vec3f)); /* 3 float coords */
	vec3i* face_buffer   = calloc(n_faces, sizeof(vec3i));    /* 3 integer indices (starts at 1!!) */

	/* vertex and triangle beeing parsed */
	vec3f* v = vertex_buffer;
	vec3i* t = face_buffer;

	/* char read, to detect error */
	int read;
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			read = fscanf(file, "%f %f %f", &v->x, &v->y, &v->z);
			if (read == 0) {
				EZ_throw("Error during vertex parsing in mesh", fname);
				fclose(file);
				return NULL;
			}
			v++;
		break;

		case 'f' : 
			read = fscanf(file, "%d %d %d", &t->x, &t->y, &t->z);
			if (read == 0) {
				EZ_throw("Error during face parsing in mesh", fname);
				fclose(file);
				return NULL;
			}
			t++;
		break;

		default : break;

		}
		NEXT();
	}


	fclose(file);


	/* init mesh data */
	mesh->nPoly = n_faces;
	mesh->triangles = calloc(n_faces, sizeof(EZ_Tri_t));


	/* copy parsed data to mesh */
	for (int i = 0; i < n_faces; i++) {

		vec3i indices = face_buffer[i];

		//!!\\ indices start at 1
		mesh->triangles[i].points[0].pos = vertex_buffer[indices.x - 1];
		mesh->triangles[i].points[1].pos = vertex_buffer[indices.y - 1];
		mesh->triangles[i].points[2].pos = vertex_buffer[indices.z - 1];

		mesh->triangles[i].col = EZ_WHITE;
	}


	free(vertex_buffer);
	free(face_buffer);


	return mesh;


}


void EZ_save_OBJ(EZ_Mesh_t* mesh, const char* fname) {


	/* open file */
	FILE *file = fopen(fname,"w"); /* TEXT MODE */

	if (file == NULL) {
		EZ_throw("Couldn't save file", fname);
		return;
	}


	EZ_warning("Not implemented");

	fclose(file);

}

