# KNN

## Usage

``` 
project.exe -ll [-fp pointsFilePath] [-fr resultsFilePath]      Linear run
project.exe -lp [-fp pointsFilePath] [-fr resultsFilePath]      CUDA + OMP
project.exe [-fp filePath] [-fr filePath]                       Defalt run(All options)
project.exe -g [-fp filePath] [-fr filePath] [-n NUM] [-k NUM]  Generate points

    Parameters:
        -fp filePath    Set points file to filePath. Default: points.txt.
        -fr filePath    Set results file to filePath. Default: results.txt
        -n NUM          Set number of points to NUM.
        -k NUM          Set number of nearest points to NUM.
```
To run with MPI use mpiexec. When runnning on one proccess, using MPI + CUDA + OMP it will automatically switch to CUDA + OMP.

##  The problem
Given n points, we need to find k nearest neighbors for each point. 1000 <= n <= 300000, 1 <= k <= 6.

## Naive approach
The simple way is to create distances matrix. On position (i, j) will be the distance from point i to point j. And then just search for indices of k smallest elements on each row. But, on large datasets space needed for this matrix will be enormous. This approach is very simple and the code will be bug free so I used it for testing results from more comlex aproaches.

## Bulk aproach
Another approach is to calculate distances in parts. Each time we calculate distances only for s points, s << n, make little distances matrix, find nearest points and save the into results matrix. This aproach doesn't need much space. We reuse same space for many iterations. Another benefit of this approach is that we can paralelize the calculations for different groups of points.
In my main implementaion I used this approach.

## CUDA + OMP + MPI
This is my main implementation. This implementation should be used on 2 or more computers. Master process will not utilize all resources of the computer so consider running 2 proccesses on main computer. Slaves utilize CUDA and all available cores.

Master routine:
	Load data from input file
	Initialize slaves and send them points and configurations
	While not calculated points left
		Send command to slave to calculate next group of numvbers
		Receive results from slave and save it
	Send each slave terminate message
	Save results into file

Slave routine:
	Receive points and configuration from master
	While master does't sends terminate message 
		Receive job from master
		Run using CUDA + OMP
		Send results to master


## CUDA + OMP
This strategy is used by slaves and when running on single proccess. It can calculate all points or group of points. It will divide the group into snmaller groups to fit in graphic card memory and calculate them. Cuda device calculates distances and proccessor calculates k nearest points. K nearest points will be calculated in same time when next group of distances are being calculated.

Routine:
	Divide given points into smaller groups and foreach do:
		Calculate mini distance matrix
		Calculate k nearest points for points in previous group
		(These 2 steps are performed in same time)


# Running
## MPI + OMP + CUDA
(When running on one proccess will be runned OMP + CUDA strategy)
```
mpiexec -n [num of procs] knn
```

# Linux
## Overview
To run this application on linux machine, I installed on it nvidia drivers, mpi and cuda.
My computer specs:
 - Fujitsu Lifebook A series
 - Inter i7 proccessor with integrated graphics
 - Nvidia GeForce GT 620M (Ivy bridge)
 - Ubuntu 14.04

To compile my work on linux just got tou sorces folder and run make.

## Driver
Main problem in my case was to install nvidia drivers that support CUDA 7 and make the computer to use it instead of integrated graphics. If your machine have one graphic drive and latest drivers are installed you can skip this step.

Here is how I managed to install and use nvidia device:
1. Install bumblebee - https://wiki.ubuntu.com/Bumblebee
2. Install newest nvidia drivers. For me it worked with 355 version.
3. Try running 	
	```
	optirun knn
	```
4. If it works good!
5. If not try this:
	```
	sudo su
	update-alternative --config x86_64-linux-gnu_gl_conf
	```
	Try step 3 on every option.
6. If still not works.... Google.

If you managed to run something on graphic device, the rest is simple.

## Cuda
Install cuda - http://docs.nvidia.com/cuda/cuda-getting-started-guide-for-linux/#ubuntu-installation

## Mpi
Install MPI - https://jetcracker.wordpress.com/2012/03/01/how-to-install-mpi-in-ubuntu/

When writing the code, separate cuda code and mpi code into separate files. This will help you when you will compile them.
To compile cuda sorce code:
```
nvcc -c kernel.cu
```

To compile MPI code:
```
mpic++ -c mpi.cpp
```

You can use mpi compiler to compile other files except cuda sources.
These commands will create *.o files. To link them and create executable use mpi linker:
```
mpic++ *.o -L <PATH_TO_CUDA_LIB_FOLDER> -lcuda -lcudart -o <EXECUTABLE_NAME>
```

Cuda lib folder in my case: /usr/local/cuda-7.5/lib64

If you want to enable OMP add -fopenmp flag:
```
mpic++ -fopenmp *.o -L <PATH_TO_CUDA_LIB_FOLDER> -lcuda -lcudart -o <EXECUTABLE_NAME>
```

You don't have to install OMP. It is part of compiler.
See my Makefile for example.
