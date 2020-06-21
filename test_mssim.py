import argparse
import numpy as np
from math import sqrt
from time import time
from mssim import mssim

def negToZeros(alb):
    (nr,nc) = alb.shape
    for row in range(nr):
        for col in range(nc):
            if alb[row,col] < 0:
                alb[row,col] = 0
    return alb

def z_indices2(image):
    (nr,nc) = image.shape
    return [(i,j) for i in range(nr) for j in range(nc) if image[i,j] <= 0]

def patch_image(image,zidxs):
    newimage = image.copy()
    for idx_pair in zidxs:
        newimage[idx_pair[0],idx_pair[1]] = 0
    return newimage

def noise(elem):
    elem = sqrt(elem) + 1e-2
    return elem*elem

def test1():
    image0 = np.array([[1,	0,	3,	1,	0,	3,	1,	0,	3],\
                       [4,	5,	6,	4,	5,	6,	4,	5,	6],\
                       [0,	7,	9,	0,	7,	9,	0,	7,	9],\
                       [1,	0,	3,	1,	0,	3,	1,	0,	3],\
                       [4,	5,	6,	4,	5,	6,	4,	5,	6],\
                       [0,	7,	9,	0,	7,	9,	0.1,	7,	9],\
                       [1,	0,	3,	1,	0,	3,	1,	0,	3]])
    image1 = np.array([[1,	0,	3,	1,	0,	3,	1,	0,	3],\
                       [4,	5.1, 6.2,	4,	5.1,	6.2,	4,	5.1,	6.2],\
                       [0.3,	7,	9,	0.3,	7,	9,	0.3,	7,	9],\
                       [1,	0,	3,	1,	0,	3,	1,	0,	3],\
                       [4,	5.1,	6.2,	4,	5.1,	6.2,	4,	5.1,	6.2],\
                       [0.3,	7,	9,	0.3,	7,	9,	0.3,	7,	9],\
                       [1,	0,	3,	1,	0,	3,	1,	0,	3]])
    zidxs = z_indices2(image0)
    image1 = patch_image(image1,zidxs)

    return image0,image1

def test2():
    filename = 'output_test_rad.h5.0.dat'
    a = np.loadtxt(filename, skiprows=2)
    image00 = a[:,1:]
    print("alb:")
    (nr,nc) = image00.shape
    print(nr, "channels x", nc, "pixels")

    image0 = negToZeros(image00)
    image0_noise = np.array([[noise(image0[i,j]) for j in range(nc)] for i in range(nr)])
    zidxs = z_indices2(image0)
    image1 = patch_image(image0_noise,zidxs)
    return image0,image1

ap = argparse.ArgumentParser()
ap.add_argument("-t", "--testnm", required=True,	help="test case (test1 or test2)")
args = vars(ap.parse_args())

testname = args["testnm"]
if testname == 'test1':
    [image0,image1] = test1()
elif testname == 'test2':
    [image0,image1] = test2()
else:
    raise argparse.ArgumentError('testnm=(test1 || test2) expected.')

my_ssim_cpp_time_start = time()
my_ssim_cpp_val = mssim(image0,image1,p=7)
my_ssim_cpp_time_end = time()
print('my_ssim_val_nz = %.15f' % my_ssim_cpp_val)
print("my_ssim_nz time: ", my_ssim_cpp_time_end - my_ssim_cpp_time_start, "[sec]")
