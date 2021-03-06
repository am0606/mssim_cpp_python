#include "mssim.h"
#include <iostream>
#include <fstream>

std::ostream* msg_stream;
char outprefix[10];
char outstr[100];

char* getPrefix()
{
	strcpy(outstr, outprefix);
	return (char *)outstr;
}

void setPrefix(const char* postprefix)
{
	
	if (postprefix == 0)
		strcpy(outprefix,"output");
	else
		strcpy(outprefix,postprefix);
}

char* setOut(const char* name)
{
	strcpy(outstr, getPrefix());
	strcat(outstr, "/");
	strcat(outstr, name);
	return (char *)outstr;
}

void print_matrix(const char* filename, double* matrix, int nrows, int ncols)
{
	std::ostream* output;
	if (filename)
	{
		output = new std::ofstream(filename);
	}
	else
	{
		output = &(std::cout);
	}

	(*output).precision(15);
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++)
			(*output) << matrix[i*ncols+j] << '\t';
		(*output) << '\n';
	}
	(*output).flush();
}

void xnz(double* m1, int ncols, double*& nz_matrix, int n_bl_r, int n_bl_c, int p)
{
	//number of totally zero p x p submatrices
	for(int i=0;i<n_bl_r;i++)
		for(int j=0;j<n_bl_c;j++)
		{
			int nz_ij = 0;
			for(int ii=i;ii<i+p;ii++)
				for(int jj=j;jj<j+p;jj++)
				{
					double val = m1[ii*ncols+jj];
					if (val > 0) nz_ij++;
				}
				nz_matrix[i*n_bl_c+j] = nz_ij;
		}
}

void mus(double* m, int ncols, double* nz_matrix, double*& mus_matrix, int n_bl_r, int n_bl_c, int p)
{
	for(int i=0;i<n_bl_r;i++)
		for(int j=0;j<n_bl_c;j++)
		{
			int idx = i*n_bl_c+j;
			double nz = nz_matrix[idx];
			if (nz > 0)
			{
				double sum_ij = 0;
				for(int ii=i;ii<i+p;ii++)
					for(int jj=j;jj<j+p;jj++)
					{
						double val = m[ii*ncols+jj];
						sum_ij += val;
					}
					double mean_ij = sum_ij / nz;
					mus_matrix[idx] = mean_ij;
			}
			else mus_matrix[idx] = 0.;
		}

}

void sigmas2Pre(double* m, int ncols, double* mus_matrix, double* nz_matrix, double*& sigma2pre_matrix, int n_bl_r, int n_bl_c, int p)
{
	for(int i=0;i<n_bl_r;i++)
		for(int j=0;j<n_bl_c;j++)
		{
			int idx = i*n_bl_c+j;
			double nz = nz_matrix[idx];
			if (nz > 0)
			{
				double sum2_ij = 0;
				for(int ii=i;ii<i+p;ii++)
					for(int jj=j;jj<j+p;jj++)
					{
						double val = m[ii*ncols+jj];
						sum2_ij += val*val;
					}
					double mu = mus_matrix[idx];
					double sigma2_ij = sum2_ij - mu*mu*nz;
					sigma2pre_matrix[idx] = sigma2_ij;
			}
			else sigma2pre_matrix[idx] = 0.;
		}

}

void covPre(double* m1, double* m2, int ncols, double* mus_matrix1, double* mus_matrix2, double* nz_matrix, double*& covpre_matrix, int n_bl_r, int n_bl_c, int p)
{
	for(int i=0; i<n_bl_r; i++)
		for(int j=0; j<n_bl_c; j++)
		{
			int idx = i*n_bl_c+j;
			double nz = nz_matrix[idx];
			if (nz > 0)
			{
				double sum_ij = 0;
				for(int ii=i;ii<i+p;ii++)
					for(int jj=j;jj<j+p;jj++)
					{
						int iijj = ii*ncols+jj;
						double m1_val = m1[iijj];
						double m2_val = m2[iijj];
						sum_ij += m1_val*m2_val;
					}				
					double mu1 = mus_matrix1[idx];
					double mu2 = mus_matrix2[idx];
					double cov_ij = sum_ij - mu1*mu2*nz;
					covpre_matrix[idx] = cov_ij;
			}
			else covpre_matrix[idx] = 0.;
		}
}

double ab(double* mus_matrix1, double* mus_matrix2, double* sigma2_matrix1, double* sigma2_matrix2, double* covpre_matrix, int n_bl_r, int n_bl_c, double*& ab_matrix)
{
	double my_ssim_a_sum = 0;
	//number of blocks with zero contribution (zero input matrices and zero variances)
	int num_z_blocks = 0;
	for(int i=0; i<n_bl_r; i++)
		for(int j=0; j<n_bl_c; j++)
		{
			int idx = i*n_bl_c+j;
			double mu_ij_1 = mus_matrix1[idx];
			double mu_ij_2 = mus_matrix2[idx];
			double a_value;
			if (mu_ij_1 == 0 && mu_ij_2 == 0) a_value = 0.;
			else a_value = 2*mu_ij_1*mu_ij_2/(mu_ij_1*mu_ij_1 + mu_ij_2*mu_ij_2);
			if (a_value > 0.)
			{
				double b_value;
				double sigma2pre_ij_1 = sigma2_matrix1[idx];
				double sigma2pre_ij_2 = sigma2_matrix2[idx];
				if (sigma2pre_ij_1 == 0 && sigma2pre_ij_2 == 0)
				{
					b_value = 0.;
					num_z_blocks++;
				}
				else b_value = 2*covpre_matrix[idx]/(sigma2pre_ij_1+sigma2pre_ij_2);
				double ab_value = a_value * b_value;
				ab_matrix[idx] = ab_value;
				my_ssim_a_sum += ab_value;
			}
			else 
			{
				ab_matrix[idx] = 0.;
				num_z_blocks++;
			}

		}
	int nnz = n_bl_r*n_bl_c - num_z_blocks;
	////DEBUG
	// std::cout.precision(17);
	// std::cout << "num_z_blocks = " << num_z_blocks << ", nnz = " << nnz << ", my_ssim_a_sum = " <<  my_ssim_a_sum << std::endl;
	return my_ssim_a_sum/nnz;
}

double mssim(double* m1, int nrows, int ncols, double* m2, int nrows2, int ncols2, int p)
{
	if (nrows != nrows2)
	{
		std::ostringstream err_str;
		err_str << "mssim: nrows != nrows2!\n";
		(*msg_stream) << err_str.str();
		std::cout << err_str.str();
		exit(1);
	}
	if (ncols != ncols2)
	{
		std::ostringstream err_str;
		err_str << "mssim: ncols != ncols2!\n";
		(*msg_stream) << err_str.str();
		std::cout << err_str.str();
		exit(1);
	}
	unsigned n_bl_r = nrows - p + 1;
	unsigned n_bl_c = ncols - p + 1;

	double* nz_matrix = new double[n_bl_r * n_bl_c];
	xnz(m1,ncols,nz_matrix,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("nz_matrix.txt"), nz_matrix, n_bl_r, n_bl_c);
#endif
	////DEBUG
	//print_matrix(0, nz_matrix, n_bl_r, n_bl_c);

	double* mus_matrix1 = new double[n_bl_r * n_bl_c];
	mus(m1,ncols,nz_matrix,mus_matrix1,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("mus_matrix1.txt"),mus_matrix1, n_bl_r, n_bl_c);
#endif
	////DEBUG
	//print_matrix(0,mus_matrix1, n_bl_r, n_bl_c);

	double* mus_matrix2 = new double[n_bl_r * n_bl_c];
	mus(m2,ncols,nz_matrix,mus_matrix2,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("mus_matrix2.txt"),mus_matrix2, n_bl_r, n_bl_c);
#endif
	////DEBUG
	//print_matrix(0,m2,nrows, ncols);
	//print_matrix(0,mus_matrix2, n_bl_r, n_bl_c);

	double* sigma2_pre_matrix1 = new double[n_bl_r * n_bl_c];
	sigmas2Pre(m1,ncols,mus_matrix1,nz_matrix,sigma2_pre_matrix1,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("sigma2_pre_matrix1.txt"), sigma2_pre_matrix1, n_bl_r, n_bl_c);
#endif

	double* sigma2_pre_matrix2 = new double[n_bl_r * n_bl_c];
	sigmas2Pre(m2,ncols,mus_matrix2,nz_matrix,sigma2_pre_matrix2,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("sigma2_pre_matrix2.txt"), sigma2_pre_matrix2, n_bl_r, n_bl_c);
#endif

	double* cov_pre_matrix = new double[n_bl_r * n_bl_c];
	covPre(m1,m2,ncols,mus_matrix1,mus_matrix2,nz_matrix,cov_pre_matrix,n_bl_r,n_bl_c,p);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("cov_pre_matrix.txt"), cov_pre_matrix, n_bl_r, n_bl_c);
#endif

	double* ab_matrix = new double[n_bl_r * n_bl_c];
	double my_ssim = ab(mus_matrix1,mus_matrix2,sigma2_pre_matrix1,sigma2_pre_matrix2,cov_pre_matrix,n_bl_r,n_bl_c,ab_matrix);
#ifdef __DEBUG_OUTPUT
	print_matrix(setOut("ab_matrix.txt"), ab_matrix, n_bl_r, n_bl_c);
#endif
	delete[] nz_matrix;
	delete[] mus_matrix1;
	delete[] mus_matrix2;
	delete[] sigma2_pre_matrix1;
	delete[] sigma2_pre_matrix2;
	delete[] cov_pre_matrix;
	delete[] ab_matrix;
	return my_ssim;
}
