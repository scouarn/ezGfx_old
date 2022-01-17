#include "ezErr.h"
#include "ezGfx_mesh.h"
#include "ezGfx_shader.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFF_LEN 1024

/* https://www.loc.gov/preservation/digital/formats/fdd/fdd000507.shtml
 * https://www.fileformat.info/format/wavefrontobj/egff.htm
 * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */


static int startwith(char* haystack, char* needle) {

	char* location = strstr(haystack, needle);

	return location == haystack; /* test if the substring found is at the begining */

}


void EZ_mesh_free(EZ_Mesh_t* mesh) {

	if (mesh == NULL) return;

	EZ_Tri_t *tri = mesh->faces;

	while (tri) {

		EZ_Tri_t* next = tri->next;
		free(tri);
		tri = next;
	}

	EZ_mesh_freeTextures(mesh);

	free(mesh);
}

void EZ_mesh_freeTextures(EZ_Mesh_t* mesh) {

	if (mesh == NULL) return;

	for (int i = 0; i < MESH_MAT_COUNT; i++) {
		if (mesh->materials[i].tex)
			EZ_image_free(mesh->materials[i].tex);

		mesh->materials[i].tex = NULL;
	}
}

void EZ_mesh_loadSingleTexture(EZ_Mesh_t* mesh, const char* fname) {

	EZ_mesh_freeTextures(mesh);

	EZ_Image_t* texture = EZ_image_loadBMP(fname);

	mesh->materials[0].shad = EZ_shader_textured;
	mesh->materials[0].tex  = texture;
	mesh->materials[0].col  = EZ_WHITE;

}

void EZ_mesh_loadMTL(EZ_Mesh_t* mesh, const char* fname) {

	EZ_mesh_freeTextures(mesh);
	
	mesh->materials[0].shad = EZ_shader_flat;
	mesh->materials[0].tex  = NULL;
	mesh->materials[0].col  = EZ_WHITE;


	/* open file in text mode */
	FILE *fp = fopen(fname,"r");

	if (fp == NULL) {
		ERR_warning("Couldn't open file %s", fname);
	}


	/* SINGLE PASS PARSING	*/
	char line[LINE_BUFF_LEN];
	int current_mat = 0;


	while ( fgets(line, LINE_BUFF_LEN, fp) != NULL ) {

		if (startwith(line, "newmtl ")) {
			
			int parsed = sscanf(line, "newmtl Material.%d", &current_mat);

			if (parsed != 1 || current_mat >= MESH_MAT_COUNT) {
				current_mat = 0;
			}

		}

		else if (startwith(line, "map_Kd ")) {

			/* just in case to avoid leeks if a material is redefined */
			EZ_image_free(mesh->materials[current_mat].tex);

			char texfile[256];
			sscanf(line, "map_Kd %256s", texfile);

			EZ_Image_t* texture = EZ_image_loadBMP(texfile);
			mesh->materials[current_mat].tex  = texture;
			mesh->materials[current_mat].shad = EZ_shader_textured;


		}


	}


	fclose(fp);

}





EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname) {


	/* open file in text mode */
	FILE *fp = fopen(fname,"r");

	if (fp == NULL) {
		ERR_warning("Couldn't open file %s", fname);
		return NULL;
	}



	/* 1ST PASS : COUNT HOW MANY THINGS ARE TO PARSE */

	char line[LINE_BUFF_LEN];
	int v_count  = 0, vt_count = 0, vn_count = 0, f_count = 0;
	
	while ( fgets(line, LINE_BUFF_LEN, fp) != NULL ) {

		if (startwith(line, "v "))  v_count++;
		else if (startwith(line, "vt ")) vt_count++;
		else if (startwith(line, "vn ")) vn_count++;
		else if (startwith(line, "f "))  f_count++;

	}

	if (v_count == 0 || f_count == 0) {
		ERR_warning("No vertices or faces in %s", fname);
		fclose(fp);

		return NULL;
	}


	/* 2ND PASS : PARSING	*/

	fseek(fp, 0, SEEK_SET); /* begining of file */


	/* init mesh data */
	EZ_Mesh_t* mesh = calloc( 1, sizeof(EZ_Mesh_t) );

	mesh->f_count = f_count;
	mesh->faces = calloc( f_count, sizeof(EZ_Tri_t) ); /* allocating the triangle in a contiguous chunk is simpler */

	for (int i = 0; i < f_count-1; i++) {
		mesh->faces[i].next = &mesh->faces[i+1];
	}
	
	EZ_Tri_t* tri = mesh->faces; /* current triangle beeing parse */
	int current_mat = 0; /* index of the current material beeing used */

	/* allocate room for parsing */
	EZ_Vec_t* v_buffer  = calloc( v_count , sizeof(EZ_Vec_t) );
	EZ_Vec_t* vt_buffer = calloc( vt_count, sizeof(EZ_Vec_t) );
	EZ_Vec_t* vn_buffer = calloc( vn_count, sizeof(EZ_Vec_t) );

	/* allow negative / relative indexing */
	int v_index = 0, vt_index = 0, vn_index = 0, f_index = 0;


	while ( fgets(line, LINE_BUFF_LEN, fp) != NULL ) {


		if (startwith(line, "mtllib ")) {
			char matfile[256];
			sscanf(line, "mtllib %256s", matfile);
			EZ_mesh_loadMTL(mesh, matfile);
		}

		else if (startwith(line, "usemtl ")) {
			
			if (sscanf(line, "usemtl Material.%d", &current_mat) == 1) {
				/* OK */
			}
			else {
				current_mat = 0;
			}
		}

		else if (startwith(line, "vt ")) {

			sscanf(line, "vt %g %g", 
					&vt_buffer[vt_index].x,
					&vt_buffer[vt_index].y
				);

			vt_buffer[vt_index].z = 1.0;
			vt_buffer[vt_index].w = 1.0;

			vt_index++;
		}

		else if (startwith(line, "vn ")) {

			sscanf(line, "vn %g %g %g", 
					&vn_buffer[vn_index].x, 
					&vn_buffer[vn_index].y, 
					&vn_buffer[vn_index].z
				);

			vn_buffer[vn_index].w = 1.0;

			vn_index++;
		}

		else if (startwith(line, "v ")) {
			
			sscanf(line, "v %g %g %g", 
					&v_buffer[v_index].x, 
					&v_buffer[v_index].y, 
					&v_buffer[v_index].z
				);

			v_buffer[v_index].w = 1.0;

			v_index++;
		}

		else if (startwith(line, "f ")) {

			int i;
			char* token = strtok(line+2, " ");  /* skip "f " */ /* the tokens are separated by spaces */ 

			for (i = 0; i < 3; i++) {
				int v, vt, vn; 

				if (sscanf(token, "%d/%d/%d",  &v, &vt, &vn) == 3) {
					/* nothing */
				}
				else if (sscanf(token, "%d//%d", &v, &vn) == 2) {
					vt = 0;
				}
				else if (sscanf(token, "%d/%d", &v, &vt) == 2) {
					vn = 0;
				}
				else if (sscanf(token, "%d", &v) == 1) {
					vt = 0; vn = 0;
				}
				else {
					ERR_warning("Error while parsing face in %s", fname);
					v = 0; vt = 0; vn = 0;
				}

				token = strtok(NULL, " "); /* next token */ 

				/* write information */
				/* -1 means the last one */
				/* 1 means the first*/
				/* 0 means nothing (default to 0,0,0,0 : cf calloc) */

				if (v < 0) {
					tri->vert[i].pos = v_buffer[v_index + v];
				}
				else if (v > 0) {
					tri->vert[i].pos = v_buffer[v - 1];
				}

				if (vt > 0) {
					tri->vert[i].uv = vt_buffer[vt - 1];
				}

				if (vn > 0) {
					tri->vert[i].normal = vn_buffer[vn - 1];
				}

				tri->vert[i].col = EZ_WHITE;

			}

			tri->mat = mesh->materials + current_mat;

			tri++;
			f_index++;

		}

	}

	if (f_index != f_count)
		ERR_warning("Unparsed faces in %s", fname);
	if (v_index != v_count)
		ERR_warning("Unused vertex in %s", fname);
	if (vt_index != vt_count)
		ERR_warning("Unused UV in %s", fname);
	if (vn_index != vn_count)
		ERR_warning("Unused normal in %s", fname);


	free(v_buffer);
	free(vt_buffer);
	free(vn_buffer);
	
	fclose(fp);


	return mesh;


}


