%module mssim
%{ 
    #define SWIG_FILE_WITH_INIT
	#include <fstream>
    #include "mssim.h"
%}

%include "numpy.i"

%init %{
    import_array();
%}


%apply (double* IN_ARRAY2, int DIM1, int DIM2) {(double* m1, int nrows, int ncols),(double* m2, int nrows2, int ncols2)};

%include "mssim.h"
