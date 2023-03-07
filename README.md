#### 1. Canny edge detection consists of a multistage algorithm consisting of the following steps:  
    1. Smoothing image with Gaussian filter. 
    2. Computing gradient magnitude and orientation. 
    3. Applying Non-maxima suppression to the gradient magnitude image. 
    4. Using double-thresholding to detect and link images.
    5. Tracking edges by hysteresis(blob analysis). 

#### 2. The repository consists parallelized implementations of the Canny Edge Detection algorithm using 
- C++/OPENMP
- CUDA Framework

#### 3. Some modifications in the multi-stage algorithm.
Proposal: analyze if there are any improvements in edge detection with the following changes:  
    1. In Step 1, replacing gaussian filter with a different algorithm for smoothing and denoising  
    2. In Step 2, the comparison of different operators (Sobel, Schaar, etc) to yield different results  
    3. In step 4, using OTSU thresholding for automatic image thresholding. (Thresholds need to be specified in the normal algorithm)  
