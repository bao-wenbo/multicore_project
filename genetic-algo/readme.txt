Test CIMS server: crunchy5

GCC Version:
module load gcc-12.2

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