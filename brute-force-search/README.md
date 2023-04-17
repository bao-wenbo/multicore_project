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