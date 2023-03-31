#### 1. Canny edge detection consists of a multistage algorithm consisting of the following steps:  
    1. Smoothing image with Gaussian filter. 
    2. Computing gradient magnitude and orientation. 
    3. Applying Non-maxima suppression to the gradient magnitude image. 
    4. Using double-thresholding to detect and link images.
    5. Tracking edges by hysteresis(blob analysis). 

#### 2. The repository consists parallelized implementations of the Canny Edge Detection algorithm using 
- C++/OPENMP

To use, ensure the following

##### 1. Have an ASCII encoded PGM (P2) image file.

1. Use UNIX's `convert *.png|*.jpg binary.pgm` to convert from any image file (almost) to a binary PGM file.

2. Finally to convert from binary encoded PGM to ASCII encoded PGM (the one we want),
- Use the repo's `pgmb_to_pgma.cpp`. 
Build with `make pgmb_to_pgma`.
- Usage: `pgmb_to_pgma <input-binary-PGM>.pgm 01.pgm`
(where `01.pgm` is the required ASCII file for the program to work on. Save it as just that!)

4. Finally, run `make main.final` and run as `./main.final`




#### 3. Some modifications in the multi-stage algorithm.
Proposal: analyze if there are any improvements in edge detection with the following changes:  
    1. In Step 1, replacing gaussian filter with a different algorithm for smoothing and denoising  
    2. In Step 2, the comparison of different operators (Sobel, Schaar, etc) to yield different results  
    3. In step 4, using OTSU thresholding for automatic image thresholding. (Thresholds need to be specified in the normal algorithm)  
