#include "ezErr.h"
#include "ezGfx_mesh.h"

#include <stdlib.h>
#include <stdio.h>


void EZ_mesh_free(EZ_Mesh_t* mesh) {
	free(mesh->triangles);
	free(mesh);
}




/* https://www.loc.gov/preservation/digital/formats/fdd/fdd000507.shtml
 * https://www.fileformat.info/format/wavefrontobj/egff.htm
 * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 */





EZ_Mesh_t* EZ_mesh_loadOBJ(const char* fname) {



	/* open file in text mode */
	FILE *fp = fopen(fname,"r");

	if (fp == NULL) {
		ERR_warning("Couldn't open file %s", fname);
		return NULL;
	}



	/* 1ST PASS : COUNT HOW MANY THINGS ARE TO PARSE */



	char c;
	int nPos = 0, nFaces = 0, nUV = 0, nNorm = 0;

	/* go to next line, stop if end_of_file encountered */
	#define NEXTLINE() while(c != '\n' && c != EOF) c = getc(fp);

	do {
		c = getc(fp);

		switch (c) {

			case 'v' :
				if ((c = getc(fp)) == 't') nUV++;
				else if (c == 'n') nNorm++;

				else {
					ungetc(c, fp);
					nPos++;
				}

			break;

			case 'f' :
				nFaces++;
			break;

			default : 

			break;
		}

		NEXTLINE();

	} while (c != EOF);



	fseek(fp, 0, SEEK_SET);




	/* 2ND PASS : PARSING	*/



	/* init mesh data */
	EZ_Mesh_t* mesh = malloc( sizeof(EZ_Mesh_t) );
	mesh->triangles = calloc(nFaces, sizeof(EZ_Tri_t));
	mesh->nPoly = nFaces;



	/* allocate room for parsing */
	EZ_Vec_t* v_buffer  = malloc( nPos  * sizeof(EZ_Vec_t) );
	EZ_Vec_t* vt_buffer = malloc( nUV   * sizeof(EZ_Vec_t) );
	EZ_Vec_t* vn_buffer = malloc( nNorm * sizeof(EZ_Vec_t) );

	int v_index  = 0, vt_index = 0, vn_index = 0, f_index = 0;


	do {

		c = getc(fp);

		switch (c) {

		case 'v' :

			/* UV */
			if ((c = getc(fp)) == 't') {

				fscanf(fp, "%g %g", 
					&vt_buffer[vt_index].x, 
					&vt_buffer[vt_index].y
				);

				vt_index++;
			}

			else if (c == 'n') {

				fscanf(fp, "%g %g %g", 
					&vn_buffer[v_index].x, 
					&vn_buffer[v_index].y, 
					&vn_buffer[v_index].z
				);

				vn_index++;
			}

			/* POSITION */
			else {
				ungetc(c, fp);

				fscanf(fp, "%g %g %g", 
					&v_buffer[v_index].x, 
					&v_buffer[v_index].y, 
					&v_buffer[v_index].z
				);

				v_buffer[v_index].w = 1.0;

				v_index++;
			}

		break;


		/* Write directly to the mesh structure */
		case 'f' : {
			int i, v = 0, vt = 0, vn = 0; 

			for (i = 0; i < 3; i++) {

				/* parse v */
				fscanf(fp, "%d", &v);

				if ((c = getc(fp)) == '/') {

					/* parse vt (can fail) */
					fscanf(fp, "%d", &vt);

					/* parse vn */
					if ((c = getc(fp)) == '/')
						fscanf(fp, "%d", &vn);
					
					else 
						ungetc(c, fp);

				}
				else ungetc(c, fp);


				/* write information */
				/* -1 means the last one */
				/* 1 means the first*/
				/* 0 means nothing (default to 0,0,0,0 : cf calloc) */
				if (v < 0) {
					mesh->triangles[f_index].vert[i].pos = v_buffer[v_index + v];	
				}
				else if (v > 0) {
					mesh->triangles[f_index].vert[i].pos = v_buffer[v - 1];
				}

				if (vt > 0) {
					mesh->triangles[f_index].vert[i].u = vt_buffer[vt - 1].x;
					mesh->triangles[f_index].vert[i].v = vt_buffer[vt - 1].y;
				}

				if (vn > 0) {
					mesh->triangles[f_index].vert[i].normal = vn_buffer[vn - 1];
				}

				mesh->triangles[f_index].col< = EZ_WHITE;	
			}

			f_index++;

		}
		break;



		default : 

		break;

		}
		

		NEXTLINE();

	} while (c != EOF);


	fclose(fp);



	free(v_buffer);
	free(vt_buffer);
	free(vn_buffer);


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