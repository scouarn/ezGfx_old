#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void EZ_mesh_free(EZ_Mesh_t* mesh) {

	EZ_Tri_t *tri = mesh->faces;

	while (tri) {

		EZ_Tri_t* next = tri->next;
		free(tri);
		tri = next;
	}

	if (mesh->m_count > 0)
		free(mesh->materials);
	
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

	if (v_count == 0 || f_count == 0) {
		ERR_warning("No vertices or faces in %s", fname);
		fclose(fp);

		return NULL;
	}


	/* 2ND PASS : PARSING	*/

	fseek(fp, 0, SEEK_SET); /* begining of file */


	/* init mesh data */
	EZ_Mesh_t* mesh = calloc( 1, sizeof(EZ_Mesh_t) );
	mesh->faces = calloc( f_count, sizeof(EZ_Tri_t) ); /* allocating the triangle in a contiguous chunk is simpler */
	mesh->f_count = f_count;

	mesh->materials = malloc( sizeof(EZ_Material_t) );
	mesh->materials[0] = (EZ_Material_t) { 0, NULL, EZ_WHITE };
	mesh->m_count = 1;

	EZ_Tri_t* tri = mesh->faces; /* current triangle beeing parse */

	for (int i = 0; i < f_count-1; i++) {
		mesh->faces[i].next = &mesh->faces[i+1];
	}
	


	/* allocate room for parsing */
	EZ_Vec_t* v_buffer  = calloc( v_count , sizeof(EZ_Vec_t) );
	EZ_Vec_t* vt_buffer = calloc( vt_count, sizeof(EZ_Vec_t) );
	EZ_Vec_t* vn_buffer = calloc( vn_count, sizeof(EZ_Vec_t) );

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
			char* token = strtok(line+2, " ");  /* skip "f " */ /* the tokens are separated by spaces */ 

			for (i = 0; i < 3; i++) {
				int v, vt, vn; 

				if (vn_count == 0 && vt_count == 0) {
					sscanf(token, "%d", &v);
					vt = 0; vn = 0;
				}

				else if (vn_count == 0) {
					sscanf(token, "%d/%d", &v, &vt);
					vn = 0;
				}

				else if (vt_count == 0) {
					sscanf(token, "%d//%d", &v, &vn);
					vt = 0;
				}

				else {
					sscanf(token, "%d/%d/%d",  &v, &vt, &vn);
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

			tri->mat = mesh->materials;

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



// EZ_Mesh_t* EZ_mesh_loadOFF(const char* fname) {

// 	#define LINE_BUFF_LEN 1024


// 	/* open file in text mode */
// 	FILE *fp = fopen(fname,"r");

// 	if (fp == NULL) {
// 		ERR_warning("Couldn't open file %s", fname);
// 		return NULL;
// 	}


// 	char line[LINE_BUFF_LEN];


// 	/* read the number of things to parse */
// 	int v_count, f_count, e_count; /* edge count is irrelevent and can be ignored */

// 	while ( fgets(line, LINE_BUFF_LEN, fp) != NULL
// 	    && 3 != sscanf(line, "%d %d %d", &v_count, &f_count, &e_count)
// 		);


// 	if (v_count == 0 || f_count == 0) {
// 		ERR_warning("No vertices or faces in %s", fname);
// 		fclose(fp);
// 		return NULL;
// 	}

	

// 	/* allocate for parsing vertices */
// 	EZ_Vec_t* v_buffer = calloc( v_count, sizeof(EZ_Vec_t) );


// 	/* parse vertices */
// 	int i = 0;

// 	while (i < v_count) {

// 		if (fgets(line, LINE_BUFF_LEN, fp) == NULL) {
// 			ERR_warning("Unexpected end of file during vertex parsing in %s", fname);
// 			break;
// 		}
	
// 		int parsed = sscanf(line, "%g %g %g", 
// 						&v_buffer[i].x,
// 						&v_buffer[i].y,
// 						&v_buffer[i].z
// 					);

// 		v_buffer[i].w = 1.0;

// 		if (parsed == 3) {
// 			i++;
// 		}
// 		else if (parsed > 0) { 
// 			ERR_warning("Error while parsing a vertex in %s", fname);
// 			break;
// 		}
// 		/* else : zero --> empty line or comment */

// 	}


// 	/* alloc for face parsing */
// 	EZ_Mesh_t* mesh = calloc( 1, sizeof(EZ_Mesh_t) );
// 	mesh->faces = calloc( f_count, sizeof(EZ_Tri_t) ); /* allocating the triangle in a contiguous chunk is simpler */
// 	mesh->f_count = f_count;


// 	/* set up the linked list */
// 	for (i = 0; i < f_count-1; i++) {
// 		mesh->faces[i].next = &mesh->faces[i+1];
// 	}


// 	/* parse faces */
// 	EZ_Tri_t* tri = mesh->faces;
// 	i = 0;

// 	while (i < f_count) {

// 		if (fgets(line, LINE_BUFF_LEN, fp) == NULL) {
// 			ERR_warning("Unexpected end of file during face parsing in %s", fname);
// 			break;
// 		}
		
// 		tri->col = EZ_WHITE; /* set default alpha */

// 		int parsed, nb_edges, v1, v2, v3;
// 		parsed = sscanf(line, "%d %d %d %d %hhd %hhd %hhd", 
// 				&nb_edges, 
// 				&v1, &v2, &v3, 
// 				&tri->col.r, &tri->col.g, &tri->col.b
// 			);


// 		if (nb_edges != 3) {
// 			ERR_warning("Unsupported non-triangular face in %s", fname);
// 		}	

// 		if (parsed != 4 && parsed != 7) {
// 			ERR_warning("Error while parsing a face in %s", fname);
// 		}


// 		tri->vert[0].pos = v_buffer[v1];
// 		tri->vert[1].pos = v_buffer[v2];
// 		tri->vert[2].pos = v_buffer[v3];


// 		tri++;
// 		i++;
// 	}

// 	free(v_buffer);
// 	fclose(fp);

// 	return mesh;
// }


