#include "ezGfx.h"

#include <stdio.h>
#include <stdlib.h>



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



