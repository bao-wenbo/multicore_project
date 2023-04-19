The reference code is from: https://rosettacode.org/wiki/N-queens_problem#C++.

# How to compile

The two cpp file are for two different problem. nqueens.cpp is to find all the solution of an n-queens problem. nqueens_first.cpp is to find the first X (e.g. X = 1, 3, 5) solution of an n-queens problem.

# How to run

1. Use "module load gcc-9.2" to switch to gcc 9.2

2. Use "make" to compile 

3. To find all the solutions of a N-queens problem, run `./nqueens num_threads N`, where num_threads is the number of threads, N is the number of queens.

4. To find X solutions of a N-queens problem, run `./nqueens num_threads N X`, where X is the first X solution.

5. To test the reference code for a N-queens problem, run `nqueens_ref N`.
