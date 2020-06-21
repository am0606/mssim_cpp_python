#include <fstream>
#include <string>
#include <math.h>
#include "xtimer.h"
#include "mssim.h"

extern std::ostream* msg_stream;

int get_n(std::string s)
{
	std::istringstream issr(s);
	int nbands;
	issr >> nbands;
	return nbands;
}

int import(const char* filename, double*& matrix, int& nrows, int& ncols)
{
	std::ifstream input(filename);
	if (!input)
	{
		std::ostringstream err_str;
		err_str << "import: cannot open input file" << filename << std::endl;
		(*msg_stream) << err_str.str();
		std::cout << err_str.str();
		return 1;
	}
	std::string s;
	//first line
	getline(input,s);
	//nrows is number of bands
	nrows = get_n(s);
	//second line
	getline(input,s);
	//ncols is number of pixels
	ncols = get_n(s);
	matrix = new double[nrows*ncols];

	double num;
	for(int i=0;i<nrows;i++)
	{
		input >> num;
		for(int j=0;j<ncols;j++)
		{
			input >> num;
			matrix[i*ncols+j] = num;
		}
	}
	input.close();
	return 0;
}

void negToZeros(double*& alb, int nrows, int ncols)
{
	for(int i=0;i<nrows;i++)
		for(int j=0;j<ncols;j++)
		{
			int idx = i*ncols+j;
			double value = alb[idx];
			if (value < 0)
				alb[idx] = 0.;
		}
}

double* noise(double* m1, int nrows, int ncols)
{
	double* m2 = new double[nrows*ncols];
	for(int i=0;i<nrows;i++)
		for(int j=0;j<ncols;j++)
		{
			int idx = i*ncols+j;
			double value1 = m1[idx];
			double value2 = sqrt(value1)+1e-2;
			m2[idx] = value2*value2;
		}
	return m2;
}

void patch_image(double*& m1, double*& m2, int nrows, int ncols)
{
	for(int i=0;i<nrows;i++)
		for(int j=0;j<ncols;j++)
		{
			int idx = i*ncols+j;
			double m1_value = m1[idx];
			if (m1_value <= 0) m2[idx] = 0.;
		}
}

void test1(double*& m1, double*& m2, int& nrows, int& ncols)
{
	//multidimensional C array
	double m1_arr[7][9] = {{1.,	0.,	3.,	1.,	0.,	3.,	1.,	0.,	3.},
						   {4.,	5.,	6.,	4.,	5.,	6.,	4.,	5.,	6.},
						   {0.,	7.,	9.,	0.,	7.,	9.,	0.,	7.,	9.},
						   {1.,	0.,	3.,	1.,	0.,	3.,	1.,	0.,	3.},
						   {4.,	5.,	6.,	4.,	5.,	6.,	4.,	5.,	6.},
						   {0.,	7.,	9.,	0.,	7.,	9.,	0.1, 7.,9.},
						   {1.,	0.,	3.,	1.,	0.,	3.,	1.,	0.,	3.}};
	nrows = 7;
	ncols = 9;
	//It is important to note that in the C type system, a multidimensional array a[][] is NOT equivalent 
	//to a single pointer *a or a double pointer such as **a. Instead, a pointer to an array is used 
	//where the actual value of the pointer is the starting memory location of the array. 
	m1 = new double[nrows*ncols];
	for(int i=0;i<nrows;i++)
		for(int j=0;j<ncols;j++)
			m1[i*ncols+j] = m1_arr[i][j];
	double m2_arr[7][9] = {{1.,	 0.,	3.,	    1.,	     0.,	3.,	  1.,	0.,	 3.},
	   					   {4.,	 5.1,   6.2,    4,	     5.1,	6.2,  4.,	5.1, 6.2},
						   {0.3, 7.,	9.,	    0.3,     7.,	9.,	  0.3,	7.,	 9.},
						   {1.,	 0.,	3.,	    1,	     0.,	3.,   1.,	0.,	 3.},
						   {4.,	 5.1,   6.2,	4.,	     5.1,   6.2,  4.,	5.1, 6.2},
						   {0.3, 7.,	9,	    0.3,	 7.,	9.,	  0.3,	7.,	 9.},
						   {1.,	 0.,	3.,	    1.,	     0.,	3.,	  1.,	0.,	 3.}};
	m2 = new double[nrows*ncols];
	for(int i=0;i<nrows;i++)
		for(int j=0;j<ncols;j++)
			m2[i*ncols+j] = m2_arr[i][j];
}

int test2(double*& m1, double*& m2, int& nrows, int& ncols)
{
	const char* filename = "output_test_rad.h5.0.dat";
	int imp_retvalue = import(filename,m1,nrows,ncols);
	if (imp_retvalue == 0)
		m2 = noise(m1,nrows,ncols);
	return imp_retvalue;

}

int main(int argc, char* argv[])
{
	setPrefix();
	std::ofstream fout(setOut("stdout.txt"));
	msg_stream = &fout;
	(*msg_stream).precision(18);
	if (argc != 2)
	{
		std::ostringstream err_str;
		err_str << "Wrong number of parameters. Parameter value 'test1' or 'test2' should be set!\n";
		(*msg_stream) << err_str.str();
		std::cout << err_str.str();
		return 1;
	}

	double* m1;
	double* m2;
	int nrows, ncols;

	if (strcmp(argv[1],"test1")==0)
		test1(m1,m2,nrows,ncols);
	else if (strcmp(argv[1],"test2")==0)
	{
		int imp_retvalue = test2(m1,m2,nrows,ncols);
		if (imp_retvalue != 0)
		{
			std::ostringstream err_str;
			err_str << "Test2 imput file was not found!\n";
			(*msg_stream) << err_str.str();
			std::cout << err_str.str();
			return imp_retvalue;
		}
	}
	else
	{
		std::ostringstream err_str;
		err_str << "Wrong parameter value. Only 'test1' or 'test2' are available!";
		(*msg_stream) << err_str.str();
		std::cout << err_str.str();
		return 1;
	}
	
	negToZeros(m1,nrows,ncols);
	patch_image(m1,m2,nrows,ncols);
	xtimer tm;
	tm.start();
	int p=7;
	double my_ssim = mssim(m1,nrows,ncols,m2,nrows,ncols,p);
	tm.stop();
	std::ostringstream out_str;
	out_str.precision(18);
	out_str << "MSSIM = " << my_ssim << std::endl;
	out_str << "Elapsed time: " << tm.getTimeInterval() << std::endl;
	(*msg_stream) << out_str.str();
	std::cout << out_str.str();
	delete[] m1;
	delete[] m2;
	return 0;
}
