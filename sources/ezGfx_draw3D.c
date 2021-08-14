#include "ezGfx_draw3D.h"
#include "ezGfx_draw2D.h"



void EZ_draw3D_wireframe(EZ_Image target, const EZ_Mesh mesh, const mat4x4* projection, const mat4x4* transform) {

  for (int i = 0; i < mesh.nPoly; i++) {
    EZ_draw3D_tri(target, mesh.triangles[i], projection, transform);
  }

}


void EZ_draw3D_tri(EZ_Image target, const EZ_Poly tri, const mat4x4* projection, const mat4x4* transform) {

  vec4f proj[3]; //holds the coordinates in screen space of the three vertices


  //for each vertex
  for (int j = 0; j < 3; j++) {

    //apply transform matrix
    proj[j] = MMUL4V3V4(*transform, tri.points[j].pos);

    //apply projection matrix
    proj[j] = MMUL4V3V4(*projection, proj[j]);

    //divide by w  //DONT DIVIDE BY ZERO !!
    if (proj[j].w != 0.0f) proj[j] = VSCA4(proj[j], 1.0f/proj[j].w);

    //convert to screen space
    proj[j].x += 1.0f; 
    proj[j].x *= 0.5f*target.w;

    proj[j].y += -1.0f; 
    proj[j].y *= -0.5f*target.h;



    //printf("%.2f,  %.2f,  %.2f\n", proj[j].x, proj[j].y, proj[j].z);

  }

  //draw the triangle
  EZ_draw2D_tri(target, EZ_WHITE, proj[0].x, proj[0].y, proj[1].x, proj[1].y, proj[2].x, proj[2].y);

}



static const vec3f cube_data[12][3] = {

    //south
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } },
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },

    //east
    { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
    { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },

    //north
    { { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },

    //west
    { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },

    //top
    { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
    { { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },

    //bot
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
    { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },


};



EZ_Mesh EZ_draw3D_unitCube() {

  EZ_Mesh mesh;
  mesh.triangles = malloc(12 * sizeof(EZ_Poly));
  mesh.nPoly = 12;

  for (int i = 0; i < 12; i++) {
    mesh.triangles[i].col = EZ_WHITE;

    for (int j = 0; j < 3; j++)
      mesh.triangles[i].points[j].pos = cube_data[i][j];
  }

  return mesh;

}

void EZ_draw3D_freeMesh(EZ_Mesh mesh) {
  if (mesh.triangles != NULL) free(mesh.triangles);
}