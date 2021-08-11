#include "ezGfx_matrix.h"

/*

void EZ_mat_add(EZ_mat C[], EZ_mat A[], EZ_mat B[], int rows, int cols) {

  for (int i = 0; i < cols*rows; i++)
      C[i] = A[i] + B[i];

}

void EZ_mat_scale(EZ_mat C[], EZ_mat A[], EZ_mat scale, int rows, int cols) {

  for (int i = 0; i < cols*rows; i++)
      C[i] = A[i] * scale;

}

void EZ_mat_mul(EZ_mat C[], EZ_mat A[], EZ_mat B[], int rowsA, int colsA, int rowsB, int colsB) {
  if (colsA != rowsB) return;

  for (int i = 0; i < rowsA; i++)
    for(int j = 0; j < colsB; j++) {

      C[j + i*colsB] = 0.0f;

      for (int k = 0; k < colsA; k++)
        C[j + i*colsB] += A[k + i*colsB] * B[j + k*colsB];

    }


}


void EZ_mat_spe4dMatMul(EZ_mat C[], EZ_mat A[], EZ_mat B[]) {

  C[0] = A[0] * B[0]  +  A[1] * B[1]  +  A[2] * B[2]  +  A[3];
  C[1] = A[4] * B[0]  +  A[5] * B[1]  +  A[6] * B[2]  +  A[7];
  C[2] = A[8] * B[0]  +  A[9] * B[1]  +  A[10]* B[2]  +  A[11];
  C[3] = A[12]* B[0]  +  A[13]* B[1]  +  A[14]* B[2]  +  A[15];

}

void EZ_mat_spe3dMatMul(EZ_mat C[], EZ_mat A[], EZ_mat B[]) {

  C[0] = A[0] * B[0]  +  A[1] * B[1]  +  A[2];
  C[1] = A[3] * B[0]  +  A[4] * B[1]  +  A[5];
  C[2] = A[6] * B[0]  +  A[7] * B[1]  +  A[8];

}



void   EZ_mat_vecCross(EZ_mat *C, EZ_mat A[], EZ_mat B[]) {

    C[0] = A[1]*B[2] - A[2]*B[1];
    C[1] = A[2]*B[0] - A[0]*B[2];
    C[2] = A[0]*B[1] - A[1]*B[0];

}

EZ_mat EZ_mat_vecDot(EZ_mat A[], EZ_mat B[], int dim) {

  EZ_mat s = 0.0f;
  for (int i = 0; i < dim; i++) {
    s += A[i] * B[i];
  }

  return s;

}

void EZ_mat_vecAdd  (EZ_mat C[], EZ_mat A[], EZ_mat B[], int dim) {
  for (int i = 0; i < dim; i++)
    C[i] = A[i] + B[i];
}

void EZ_mat_vecScale(EZ_mat C[], EZ_mat A[], EZ_mat scale, int dim) {
  for (int i = 0; i < dim; i++)
    C[i] = A[i] * scale;
}


void EZ_mat_print(EZ_mat *A, int rows, int cols) {

  for (int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++)
      printf("| %*f |", 16, A[j + i*cols]);

    printf("\n");
  }

  printf("\n");


}
*/