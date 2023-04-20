# How to compile

sa.c is the parallelized version of the simulated annealing algorithm, after running the code will find 1,3,5 solutions for 1,5,10,20 threads respectively. 


sac.c is the serialized version.


# How to run

## for sa.c

1. gcc -fopenmp -Wall  sa.c -std=c99 -o sa -lm

2. ./sa

## for sas.c

1. gcc sas.c -std=c99 -o sas

2. ./sas

