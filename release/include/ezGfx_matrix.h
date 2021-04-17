#include "ezGfx_core.h"


/*
/
/	EZGFX ALGEBRA EXTENSION
/
/
/ SCOUARN, 2021
/
/	For 2D and 3D transforms.
/
/
/
*/


#ifndef EZ_MATRIX
#define EZ_MATRIX

typedef float EZ_mat;

void EZ_mat_print(EZ_mat *A, int rows, int cols);

//general stuff
void EZ_mat_add(EZ_mat C[], EZ_mat A[], EZ_mat B[], int rows, int cols);
void EZ_mat_scale(EZ_mat C[], EZ_mat A[], EZ_mat scale, int rows, int cols);
void EZ_mat_mul(EZ_mat C[], EZ_mat A[], EZ_mat B[], int rowsA, int colsA, int rowsB, int colsB);

//macros for 1*n and n*1 matices aka vectors
void EZ_mat_vecAdd  (EZ_mat C[], EZ_mat A[], EZ_mat B[], int dim);
void EZ_mat_vecScale(EZ_mat C[], EZ_mat A[], EZ_mat scale, int dim);

// "gfx transform" version of the matrix product with the extra dimension
void EZ_mat_spe4dMatMul(EZ_mat C[], EZ_mat A[], EZ_mat B[]);
void EZ_mat_spe3dMatMul(EZ_mat C[], EZ_mat A[], EZ_mat B[]);

//useful vector functions
void   EZ_mat_vecCross(EZ_mat *C, EZ_mat A[], EZ_mat B[]);
EZ_mat EZ_mat_vecDot(EZ_mat A[], EZ_mat B[], int dim);

#endif
