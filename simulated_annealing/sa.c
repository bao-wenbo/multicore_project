#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <inttypes.h>
#include <sys/time.h>
// gcc -fopenmp -Wall  sa.c -std=c99 -o sa -lm
// ./sa 10 10
#define MAX_ITERATIONS 1000
#define TEMP_INITIAL 100 
#define TEMP_MINIMUM 0.01 
#define COOLING_RATE 0.99 


unsigned int seed; // Seed for rand_r()

void initializeBoard(int* queens, int N) {
    // #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        queens[i] = rand_r(&seed) % N;
    }
}

int get_conflicts_count(int *queens, int N) {
    int conflicts_count = 0;
    #pragma omp parallel for reduction(+:conflicts_count)
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (queens[i] == queens[j] || abs(queens[i] - queens[j]) == abs(i - j)) {
                conflicts_count++;
            }
        }
    }
    return conflicts_count;
}
bool accept(int current_conflicts, int new_conflicts, double temperature){
    if (new_conflicts < current_conflicts){
        return true;
    }
    double probability = exp((current_conflicts - new_conflicts) / temperature);
    return rand_r(&seed)/RAND_MAX < probability;
}
// Function to move one queen to a new position and calculate the new number of current_conflicts
void moveQueen(int *queens, int N, int queen, int position, int* current_conflicts) {
    int oldPosition = queens[queen];
    queens[queen] = position;
    int new_conflicts = get_conflicts_count(queens, N);
    if (new_conflicts < *current_conflicts) {
        *current_conflicts = new_conflicts;
    }
    else {
        queens[queen] = oldPosition;
    }
}


void simulated_annealing(int N,int target){
    int find_counters = 0;
    #pragma omp parallel shared(find_counters)
    {
        while(find_counters<target){

            int* queens;
            queens = (int*)malloc(N * sizeof(int));
            initializeBoard(queens,N);
            // for (int i = 0; i < N; i++) {
            //     printf("%d ", queens[i]);
            // }
            double temperature = TEMP_INITIAL; 
            int current_conflicts = get_conflicts_count(queens,N);

            int best_conflicts = current_conflicts; 
            int best_queens[N]; 
        
            for (int i = 0; i < MAX_ITERATIONS && current_conflicts > 0 && temperature > TEMP_MINIMUM&&find_counters<target; i++) 
            {
            
                for (int queen = 0; queen < N; queen++) {

                    int current_index = queens[queen];
                    int new_conflicts = current_conflicts;

                    // Try neighbor
                    for (int position = 0; position < N; position++) {
                        if (position == current_index) continue;

                        moveQueen(queens, N, queen, position, &new_conflicts);

                        if (accept(current_conflicts, new_conflicts,temperature)) {
                            current_index = position;
                            current_conflicts = new_conflicts;
                        } else {
                            queens[queen] = current_index; // Undo the move
                        }
                    }
                }


                if (current_conflicts < best_conflicts) {
                    best_conflicts = current_conflicts;
                    // #pragma omp parallel for
                    for (int i = 0; i < N; i++) {
                        best_queens[i] = queens[i];
                    }
                }

                // Cool the temperature
                temperature *= COOLING_RATE;

                
            }
        
        

            if (best_conflicts == 0) {
                #pragma omp critical
                {
                    find_counters++; // find one solution
                    printf("%d/%d solutions found:\n",find_counters,target);
                    for (int i = 0; i < N; i++) {
                        printf("%d ", best_queens[i]);
                    }
                    printf("\n");

                }
            }
        }
    
    }

} 

int main(int argc , char **argv) 
{
    int target = 1;
    int Ns[] = {8,25,50,75,100};
    if (argc == 2) {
        target = atoi(argv[1]);


    }
    else if (argc == 3) {
        target = atoi(argv[1]);
        Ns[0]=atoi(argv[2]);

    }
  
    int length = sizeof(Ns) / sizeof(Ns[0]);
    for (int i = 0; i < length; i++) {
        double time_diff, time_start;
        time_start = omp_get_wtime();
        srand(time(NULL));
        seed = rand(); 
        simulated_annealing(Ns[i],target);
        time_diff = (omp_get_wtime() - time_start);

        printf("N %2d,Time %f sec.\n", Ns[i], time_diff);
    }
    return 0;
}
