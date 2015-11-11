# Multithreaded-matrix-multiplication
implementation of a multi-threaded matrix multiplication program. The input to the program is two matrixes A(x*y) and B(y*z) that are read from corresponding files. The output is a matrix C(x*z) that is written to an output file. A parallelized version of matrix multiplication can be done using one of these two methods: (1) a thread computes each row in the output C matrix, or (2) a thread computes each element in the output C matrix.
#How to compile and run code 
1-Open terminal in program directory  
2-Enter “make” in terminal.  
3-to run the program enter : 
./matMultp  MethodID Mat1 Mat2 MatOut 
or 
./matMultp MethodID 
