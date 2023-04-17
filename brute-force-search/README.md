
# nqueens_stop.c compute [solution_count] count of solutions
# how to run nqueens_stop.c

### setup environment

module load gcc-12.2
export OMP_CANCELLATION=true

### compile

gcc -O -fopenmp -o nqueens_stop nqueens_stop.c

### run code

./nqueen_bfs [problem_size] [thread_count] [solution_count]

# nqueens_stop.c compute [solution_count] count of solutions
# how to run nqueens_all.c


### compile

gcc -O -fopenmp -o nqueens_all nqueens_all.c

### run code

./nqueens_all [problem_size] [thread_count]