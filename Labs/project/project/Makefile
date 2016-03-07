# Target rules
all: build

build: kernel.o linear.o project.o points.o utils.o
	g++ -fopenmp kernel.o project.o utils.o linear.o points.o -L /usr/local/cuda-7.5/lib64 -lcuda -lcudart -o project

kernel.o:
	nvcc -c kernel.cu

linear.o:
	g++ -c linear.cpp

project.o:
	g++ -c project.cpp

points.o:
	g++ -c points.cpp

utils.o:
	g++ -c utils.cpp

clean:
	rm *.o
