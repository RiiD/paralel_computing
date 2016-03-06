# KNN

## Usage

``` 
project.exe -l [-fp filePath] [-fr filePath]                    Linear run
project.exe [-fp filePath] [-fr filePath]                       Defalt run(All options)
project.exe -g [-fp filePath] [-fr filePath] [-n NUM] [-k NUM]  Generate points

    Parameters:
        -fp filePath    Set points file to filePath. Default: points.txt.
        -fr filePath    Set results file to filePath. Default: results.txt
        -n NUM          Set number of points to NUM.
        -k NUM          Set number of nearest points to NUM.
```

## Implementation details
### Linear implementation
To find k nearest points for one point p1 I need to calculate distances between p1 and all other points. For linear implementation I just created n * n double distances matrix, so distance between point i and point j is accesed by distances[i][j]. But memory that is needed for n = 300000 is: 
300000^2 * sizeof(double) = 720GB
I keept this linear implementation for testing purposes because it is very simple and probably correct.

### CUDA + OMP implementation
To avoid large memory usages, in this implementation I decided to divide the points into small groups and calculate them. By doing this I can reuse same memory for groups of numbers.
CODA device calculates distances for some number of points and OMP finds k nearest points and saves it in matrix. To gain maimum efficiency, OMP searches for k nearest points when CUDA computes distances for next group of points.
#### Assumption
I assume that number of points will be dividable by CUDA_THREADS_PER_BLOCK(default 1024).

### CUDA + OMP + MPI
TODO
