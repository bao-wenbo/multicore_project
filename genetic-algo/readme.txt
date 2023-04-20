Test CIMS server: crunchy5

GCC Version:
module load gcc-12.2

1. Genetic Algorithm
To Compile: 
Sequential Version : g++ NQueen_genetic.cpp -o NQueen_genetic
Parallel Versioin: g++ -fopenmp NQueen_genetic_par.cpp -o NQueen_genetic_par

To Run:
Sequential Version: time ./NQueen_genetic <num queens> <population size> <num solutions> <print solutions>
Parallel Version: time ./NQueen_genetic_par <num queens> <population size> <num solutions> <num threads> <print solutions>

where <print solutions> = 1 indicate print out all found solutions and <print solutions> = 0 indicate not print out solutions

Note that <population size> need to be <= permutation(<num queens>) because that's the maximum possible
permutation can be made to fill the population

Note: In small chance, Genetic Algorithm may stuck at local minimum during running. In this case, you need to rerun
the program to get the results

To reproduce our experiments, set <print solution> = 0 since we run all experiments without printing solutions for
reducing overheads. However, you are free to check the correctness of solutions by turning on the flag.

2. Backtracking Algorithm
To compile:

Use "make" and the makefile in the backtraking directory.

To run

(1). Use "module load gcc-9.2" to switch to gcc 9.2

(2). Use "make" to compile 

(3). To find all the solutions of a N-queens problem, run `./nqueens num_threads N`, where num_threads is the number of threads, N is the number of queens.

(4). To find X unique solutions of a N-queens problem,, run `./nqueens_first num_threads N X`,  X is the first X solution.

(5). To find all the solutions of a N-queens problem using referenced code, run `./nqueens_ref N`, where N is the number of queens and N threads will be automatically created. The source code is from https://rosettacode.org/wiki/N-queens_problem#C++. 


3. Brutal Force Algorithm

# run nqueens_stop.c
nqueens_stop.c compute [solution_count] count of solutions
### setup environment
```
module load gcc-12.2
export OMP_CANCELLATION=true
```
### compile
```
gcc -O -fopenmp -o nqueens_stop nqueens_stop.c
```
### run code
```
./nqueen_bfs [problem_size] [thread_count] [solution_count]
```

# run nqueens_all.c
nqueens_stop.c compute [solution_count] count of solutions

### compile
```
gcc -O -fopenmp -o nqueens_all nqueens_all.c
```

### run code
```
./nqueens_all [problem_size] [thread_count]
```