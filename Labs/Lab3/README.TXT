Usage:

Proc 0 will be master, so if you want 3 slaves run on 4 procs.

mpirun main [-l] [-s arraySize] [-c chunkDelimeter]

-l	Run linear function on the array. For testing purposes. Default = 0.
-s	Array size. Default = 2000.
-c	Chunk delimeter. How many chunks on worker? Default = 10.
-p	Shows progress in parrallel run.(Works only on linux)

Chunk size formula:
chunkSize = ceil(arraySize / ((commSize - 1) * chunkDelimeter))
