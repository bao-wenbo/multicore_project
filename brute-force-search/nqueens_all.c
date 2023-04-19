// followed https://github.com/victoraldecoa/N-Queens-Solver_OpenMP_Example/blob/master/src/nqueens-openmp.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include <omp.h>

#define MAX_N 16

int check_acceptable(int queen_rows[MAX_N], int n)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = i+1; j < n; j++)
		{
			// two queens in the same row => not a solution!
			if (queen_rows[i] == queen_rows[j]) return 0;
			
			// two queens in the same diagonal => not a solution!
			if (queen_rows[i] - queen_rows[j] == i - j ||
			    queen_rows[i] - queen_rows[j] == j - i)
			    return 0;
		}
	}

	return 1;
}

int main(int argc, char* argv[])
{
    int n;
    long max_iter = 1;
    
    double start_time, end_time;
    int number_solutions = 0;
        

    int num_workers;
    int i;

    n = (argc > 1) ? atoi(argv[1]) : 8;
    num_workers = (argc > 2) ? atoi(argv[2]) : 30;
    
        
    for (i = 0; i < n; i++)
    {
        max_iter *= n;
    }
  
    start_time = omp_get_wtime();
    
	long iter;

#pragma omp parallel num_threads(num_workers) \
	default(none) shared(number_solutions, n, max_iter) \
	private(iter)


#pragma omp for
	for (iter = 0; iter < max_iter; iter++)
	{
		long code = iter;
		int i;
	    int queen_rows[MAX_N];
		// the index correspond to the queen's number and the queen's collumn
		// we only generate configurations where there's only one queen per collumn
		for (i = 0; i < n; i++)
		{
			queen_rows[i] = code % n;
			
			code /= n;
		}
		
		if (check_acceptable(queen_rows, n))
		{
#pragma omp atomic
            number_solutions++;
		}
	}
    // get end time
    end_time = omp_get_wtime();
    // print results
    printf("The execution time is %g sec\n", end_time - start_time);
    printf("Number of found solutions is %d\n", number_solutions);
    
	return 0;
}
