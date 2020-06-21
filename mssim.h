#ifndef __MSSIM__
#define __MSSIM__

#include <sstream>
#include <string.h>

char* getPrefix();
void setPrefix(const char* postprefix = 0);
char* setOut(const char* name);
void print_matrix(const char* filename, double* matrix, int nrows, int ncols);
void xnz(double* m1, int ncols, double*& nz_matrix, int n_bl_r, int n_bl_c, int p);
void mus(double* m, int ncols, double* nz_matrix, double*& mus_matrix, int n_bl_r, int n_bl_c, int p);
void sigmas2(double* m, int ncols, double* mus_matrix, double* nz_matrix, double*& sigma2_matrix, int n_bl_r, int n_bl_c, int p);
void cov(double* m1, double* m2, int ncols, double* mus_matrix1, double* mus_matrix2, double* nz_matrix, double*& cov_matrix, int n_bl_r, int n_bl_c, int p);
double ab(double* mus_matrix1, double* mus_matrix2, double* sigma2_matrix1, double* sigma2_matrix2, double* cov_matrix, double*& ab_matrix, int n_bl_r, int n_bl_c);
double mssim(double* m1, int nrows, int ncols, double* m2, int nrows2, int ncols2, int p);

#endif