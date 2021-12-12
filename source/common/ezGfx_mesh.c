#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdlib.h>
#include <stdio.h>


void EZ_mesh_free(EZ_Mesh_t* mesh) {
	free(mesh->triangles);
	free(mesh);
}


typedef struct {int p1,p2,p3, u1,u2,u3;} face_index;
typedef struct {float x,y,z;} vec3f;
typedef struct {float x,y;}   vec2f;


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
	int nPos = 0, nFaces = 0, nUV = 0;

	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :
			if (getc(file) == 't') nUV++;
			else nPos++;
		break;

		case 'f' :
			nFaces++;
		break;

		default : break;

		}
		NEXTLINE();
	}

	fseek(file, 0, SEEK_SET);


	/*		2ND PASS : parsing	*/


	/* allocate room for parsing */
	vec3f* pos_buffer = calloc(nPos, sizeof(vec3f) );
	vec3f* pos = pos_buffer;

	vec2f* uv_buffer = calloc(nUV, sizeof(vec2f) );
	vec2f* uv  = uv_buffer;

	face_index* face_buffer = calloc(nFaces, sizeof(face_index) );
	face_index* face  = face_buffer;


	/* char read, to detect error */
	int read;
	while ((c = getc(file)) != EOF) {

		switch (c) {

		case 'v' :


			if ((c = getc(file)) == 't') {
				read = fscanf(file, "%g %g", &uv->x, &uv->y);
				uv++;


				if (read < 2) {
					ERR_warning("Error during uv parsing in %s", fname);
					fclose(file);
					return NULL;
				}
			}
			else {
				ungetc(c, file);
				read = fscanf(file, "%g %g %g", &pos->x, &pos->y, &pos->z);
				pos++;

				if (read < 3) {
					ERR_warning("Error during uv parsing in %s", fname);
					fclose(file);
					return NULL;
				}
			}

		break;


		case 'f' : 

			read = fscanf(file, "%d", &face->p1);

			if ((c = getc(file)) == '/') {
				read += fscanf(file, "%d %d/%d %d/%d", 
						&face->u1,
						&face->p2, &face->u2, 
						&face->p3, &face->u3
					);



				if (read < 6) {
					ERR_warning("Error during (uv mapped) face parsing in %s", fname);
					fclose(file);

					return NULL;
				}

			}
			else {

				ungetc(c, file);
				read += fscanf(file, "%d %d", &face->p2, &face->p3);

				if (read < 3) {
					ERR_warning("Error during face parsing in %s", fname);
					fclose(file);

					return NULL;
				}

			}

			face++;		

			break;

		default : break;

		}
		NEXTLINE();
	}


	fclose(file);


	/* init mesh data */
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = calloc(nFaces, sizeof(EZ_Tri_t));
	mesh->nPoly = nFaces;


	/* copy parsed data to mesh */
	for (i = 0; i < nFaces; i++) {
		face_index indices = face_buffer[i];

		/* indices start at 1 */
		mesh->triangles[i].vert[0].pos.x = pos_buffer[indices.p1 - 1].x;
		mesh->triangles[i].vert[1].pos.x = pos_buffer[indices.p2 - 1].x;
		mesh->triangles[i].vert[2].pos.x = pos_buffer[indices.p3 - 1].x;

		mesh->triangles[i].vert[0].pos.y = pos_buffer[indices.p1 - 1].y;
		mesh->triangles[i].vert[1].pos.y = pos_buffer[indices.p2 - 1].y;
		mesh->triangles[i].vert[2].pos.y = pos_buffer[indices.p3 - 1].y;

		mesh->triangles[i].vert[0].pos.z = pos_buffer[indices.p1 - 1].z;
		mesh->triangles[i].vert[1].pos.z = pos_buffer[indices.p2 - 1].z;
		mesh->triangles[i].vert[2].pos.z = pos_buffer[indices.p3 - 1].z;

		mesh->triangles[i].vert[0].pos.w = 1.0;
		mesh->triangles[i].vert[1].pos.w = 1.0;
		mesh->triangles[i].vert[2].pos.w = 1.0;

		mesh->triangles[i].vert[0].u     =  uv_buffer[indices.u1 - 1].x;
		mesh->triangles[i].vert[1].u     =  uv_buffer[indices.u2 - 1].x;
		mesh->triangles[i].vert[2].u     =  uv_buffer[indices.u3 - 1].x;

		mesh->triangles[i].vert[0].v     =  uv_buffer[indices.u1 - 1].y;
		mesh->triangles[i].vert[1].v     =  uv_buffer[indices.u2 - 1].y;
		mesh->triangles[i].vert[2].v     =  uv_buffer[indices.u3 - 1].y;

		mesh->triangles[i].col = EZ_WHITE;
	}

/*

	printf("Found %d pos  %d uv  %d faces\n", nPos, nUV, nFaces);

	for (i = 0; i < nPos; i++) {
		printf("%g %g %g\n",
		 	pos_buffer[i].x, pos_buffer[i].y, pos_buffer[i].z 
		);
	}

	for (i = 0; i < nUV; i++) {
		printf("%g %g\n",
		 	uv_buffer[i].x, uv_buffer[i].y
		);
	}

	for (i = 0; i < nFaces; i++) {
		printf("%d/%d %d/%d %d/%d\n",
		 	face_buffer[i].p1, face_buffer[i].u1, 
		 	face_buffer[i].p2, face_buffer[i].u2, 
			face_buffer[i].p3, face_buffer[i].u3
		);
	}
*/
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