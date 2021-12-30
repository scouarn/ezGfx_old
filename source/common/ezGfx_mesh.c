#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void EZ_mesh_free(EZ_Mesh_t* mesh) {

	EZ_Tri_t *tri = mesh->triangles;

	while (tri) {

		EZ_Tri_t* next = tri->next;
		free(tri);
		tri = next;
	}

	free(mesh);
}




/* https://www.loc.gov/preservation/digital/formats/fdd/fdd000507.shtml
 * https://www.fileformat.info/format/wavefrontobj/egff.htm
 * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */



static int startwith(char* haystack, char* needle) {

	char* location = strstr(haystack, needle);

	return location == haystack; /* test if the substring found is at the begining */

}



EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname) {

	#define LINE_BUFF_LEN 1024


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


		if      (startwith(line, "v "))  v_count++;
		else if (startwith(line, "vt ")) vt_count++;
		else if (startwith(line, "vn ")) vn_count++;
		else if (startwith(line, "f "))  f_count++;

	}

	/* 2ND PASS : PARSING	*/

	fseek(fp, 0, SEEK_SET); /* begining of file */


	/* init mesh data */
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = calloc( f_count, sizeof(EZ_Tri_t) ); /* allocating the triangle in a contiguous chunk is simpler */
	mesh->nPoly = f_count;

	EZ_Tri_t* tri = mesh->triangles; /* current triangle beeing parse */


	/* allocate room for parsing */
	EZ_Vec_t* v_buffer  = malloc( v_count  * sizeof(EZ_Vec_t) );
	EZ_Vec_t* vt_buffer = malloc( vt_count * sizeof(EZ_Vec_t) );
	EZ_Vec_t* vn_buffer = malloc( vn_count * sizeof(EZ_Vec_t) );

	/* allow negative / relative indexing */
	int v_index = 0, vt_index = 0, vn_index = 0, f_index = 0;


	while ( fgets(line, LINE_BUFF_LEN, fp) != NULL ) {

		if (startwith(line, "vt ")) {

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
			char* token = strtok(line+2, " ");  /* skip "f " */

			for (i = 0; i < 3; i++) {
				int v, vt, vn; 

				if (3 == sscanf(token, "%d/%d/%d",  &v, &vt, &vn) ) {

				}
				else if (2 == sscanf(token, "%d/%d", &v, &vt) ) {
					vn = 0;
				}
				else if (2 == sscanf(token, "%d//%d", &v, &vn) ) {
					vt = 0;
				}
				else if (1 == sscanf(token, "%d", &v) ) {
					vt = 0; vn = 0;
				}
				else {
					ERR_warning("Couldn't parse a face in %s", fname);
					v = 0; vt = 0; vn = 0;
				}

				token = strtok(NULL, " ");


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

			tri->col = EZ_WHITE;

			if (f_index < f_count-1) {

				tri->next = tri+1;
				tri++;
				f_index++;
			}

		}

	}


	free(v_buffer);
	free(vt_buffer);
	free(vn_buffer);
	
	fclose(fp);


	return mesh;


}



