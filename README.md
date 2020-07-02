# MSSIM Index C/C++ implementation with Python interface

## Synopsis

This is `C/C++` implementation of Structural Similarity Index (SSIM) with option to obtain `Python` module.

The key idea of SSIM Index is to introduce quality assessment based on the degradation of structural information and it was first described in paper [1]. SSIM Index is widely used to assess the quality of image and give such a metric to compare images. 

One of the applications of MSSIM (mean SSIM) is quality measure of compressed images. It can be used to compare images after compression. Here I have non-standart implementation of MSSIM. For two images (2D matrices) we take into account only positive values of the first matrix. So we compute MSSIM for those elements of both matrices if the corresponding elements of the first matrix are positive. As for `C/C++` this is simplest serial implementation of MMSIM. 

The major point of this project is Python interface. The corresponding Python module can be made by SWIG. In this case I have to convert two `numpy` arrays to `C/C++` dynamic memory arrays. Here I consider 1D arrays for the matrixes. To pass **two** numpy array to `C/C++` program one should use something like the following interface file:
```C
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
```

## Data format and return values
The program is organized as two tests. 

Test 1 - compare two 7x9 matrices.

Test 2 - compare large first matrix with its noised variant. 

Test 3 - compare two 14x9 matrices with 3 zero submatrices.

Test 4 - compare two 14x9 matrices with 1 zero submatrices and submatrices with zero variance (both of compared matrices variances are zero).

For `C/C++` programThe return value is zero if everything is OK and 1
if there are the following errors:
1. Wrong number of input parameters (should one parameter).
2. Wrong parameter value(should be `test1` or `test2` or `test3` or `test4`).
3. The file `output_test_rad.h5.0.dat` cannot be found for `test2`.
4. MSSIM function takes two input arrays of the same sizes.
If sizes parameters are not equal the error occurs.

## How to build

SWIG package is required with additional interface file `numpy.i` supplied with `numpy` package to convert `numpy` arrays to `C/C++` array. `output` subdir should be created to run `C/C++` program. `__DEBUG_OUTPUT` should be defined (in `DEFINES` section of the `Makefile` you should add `-D__DEBUG_OUTPUT`) to print intermediate results and can be used for debugging.

To build Python module set `Python` path in `PYPATH` variable in `Makefile` and/or `-I...` section of `build_python` file. Then run script `build_python` or run command `make pymodule`. 

## Results
`C/C++` tests:

$ ./mssim test1 \
MSSIM = 0.999556194103456641 \
Elapsed time: 1.40666961669921875e-05 \
$ ./mssim test2 \
MSSIM = 0.999996954461945364 \
Elapsed time: 0.05572509765625 \
$ ./mssim test3 \
MSSIM = 0.999658338206103614 \
Elapsed time: 1.9073486328125e-05 \
$ ./mssim test4 \
MSSIM = 0.999534126785326826 \
Elapsed time: 1.9073486328125e-05

`Python` tests:

$ python test_mssim.py -t test1 \
my_ssim_val_nz = 0.999556194103457
my_ssim_nz time:  1.5497207641601562e-05 [sec]
$ python test_mssim.py -t test2\
alb: \
224 channels x 1120 pixels \
my_ssim_val_nz = 0.999996954461945 \
my_ssim_nz time:  0.20418858528137207 [sec] \
$ python test_mssim.py -t test3 \
my_ssim_val_nz = 0.999658338206104 \
my_ssim_nz time:  2.6464462280273438e-05 [sec] \
$ python test_mssim.py -t test4 \
my_ssim_val_nz = 0.999534126785327 \
my_ssim_nz time:  2.7894973754882812e-05 [sec]

It can be seen that for small matrices 7x9 and 14x9 the timings are almost the same but there is some overhead in case of using `Python` interface for larger 224 x 1120 matrices.
## References

1. Wang, Zhou & Bovik, Alan & Sheikh, Hamid & Simoncelli, Eero. (2004). Image Quality Assessment: From Error Visibility to Structural Similarity. Image Processing, IEEE Transactions on. 13. 600 - 612. 10.1109/TIP.2003.819861.

## License

This code is distributed under GNU General Public License v3.0.
