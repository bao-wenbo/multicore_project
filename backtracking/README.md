# How to compile

The two cpp file are for two different problem. nqueens.cpp is to find all the solution of an n-queens problem. nqueens_first.cpp is to find the first X (e.g. X = 1, 3, 5) solution of an n-queens problem.

# How to run

1. Use "module load gcc-9.2" to switch to gcc 9.2

2. Use "make" to compile 

3. To find all the solutions of a N-queens problem, run `./nqueens num_threads N`, where num_threads is the number of threads, N is the number of queens.

4. To find X unique solutions of a N-queens problem,, run `./nqueens_first num_threads N X`,  X is the first X solution.

5. To find all the solutions of a N-queens problem using referenced code, run `./nqueens_ref N`, where N is the number of queens and N threads will be automatically created. The source code is from https://rosettacode.org/wiki/N-queens_problem#C++. 