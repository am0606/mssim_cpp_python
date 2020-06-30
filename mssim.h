#ifndef __MSSIM__
#define __MSSIM__

#include <sstream>
#include <string.h>

const int eps = 1e-12;

char* getPrefix();
void setPrefix(const char* postprefix = 0);
char* setOut(const char* name);
void print_matrix(const char* filename, double* matrix, int nrows, int ncols);
void xnz(double* m1, int ncols, double*& nz_matrix, int& num_z_matr, int n_bl_r, int n_bl_c, int p);
void mus(double* m, int ncols, double* nz_matrix, double*& mus_matrix, int n_bl_r, int n_bl_c, int p);
void sigmas2Pre(double* m, int ncols, double* mus_matrix, double* nz_matrix, double*& sigma2_matrix, int n_bl_r, int n_bl_c, int p);
void covPre(double* m1, double* m2, int ncols, double* mus_matrix1, double* mus_matrix2, double* nz_matrix, double*& covpre_matrix, int n_bl_r, int n_bl_c, int p);
double ab(double* mus_matrix1, double* mus_matrix2, double* sigma2_matrix1, double* sigma2_matrix2, double* covpre_matrix, int n_bl_r, int n_bl_c, int num_z_matr, double*& ab_matrix);
double mssim(double* m1, int nrows, int ncols, double* m2, int nrows2, int ncols2, int p);

#endif