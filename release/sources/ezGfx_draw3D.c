#include "ezGfx_draw3D.h"
#include "ezGfx_draw2D.h"
#include <math.h>



void EZ_draw3D_wireframe(EZ_Image target, EZ_poly mesh[], int nPoly, EZ_mat projection[], EZ_mat transform[]) {

  for (int i = 0; i < nPoly; i++) {

    EZ_mat trns[4];    // temporary transformed vector
    EZ_mat proj[3][4]; // holds the coordinates in screen space of the three vertices

    //for each vertex
    for (int j = 0; j < 3; j++) {

      //transform the vertex
      EZ_mat_vecAdd(trns, transform, mesh[i].vertices[j].pos, 3); // _matXvec -> look about 3D transform matrices

      //multiply by the projection matrix
      EZ_mat_spe4dMatMul(proj[j], projection, trns);

      //divide by z
      EZ_mat_vecScale(proj[j], proj[j], 1.0f/proj[j][3], 3);

      //scale to screen space
      EZ_mat_vecAdd  (proj[j], proj[j], (EZ_mat[]){ 1.0f, 1.0f }, 3);
      proj[j][0] *= target.w * 0.5f;
      proj[j][1] *= target.h * 0.5f;

    }

    //draw the triangle
    EZ_draw2D_tri(target, mesh[i].col, proj[0][0], proj[0][1], proj[1][0], proj[1][1], proj[2][0], proj[2][1]);

  }



}

void EZ_draw3D_projTransform(EZ_mat matrix[], float fov, float aRatio, float zNear, float zFar) {

  float f = 1.0f / tan(fov / 2.0f);

  float q = zFar / (zFar - zNear);

  for (int i = 0; i < 15; i++) matrix[i] = 0.0f;

  matrix[0] = aRatio*f;
  matrix[5] = f;
  matrix[10] = q;
  matrix[11] = -zNear*q;
  matrix[14] = 1.0f;


}


void EZ_draw3D_unitCube(EZ_poly mesh[], EZ_px col) {

  EZ_mat data[12][3][3] = {

    //south
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f } },
    { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },

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

  for (int p = 0; p < 12; p ++) {
    mesh[p].col = col;

    for (int v = 0; v < 3; v++)
      for (int i = 0; i < 3; i++)
        mesh[p].vertices[v].pos[i] = data[p][v][i];
  }



}
