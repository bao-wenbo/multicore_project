#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <sys/time.h>
// gcc sas.c -std=c99 -o sas
// ./sas
#define MAX_ITERATIONS 100
#define TEMP_INITIAL 100 
#define TEMP_MINIMUM 0.0001 
#define COOLING_RATE 0.95 

int num_threads = 4;
unsigned int seed; // Seed for rand_r()
void swap_queens(int i, int j, int *queens) {
    int temp = queens[i];
    queens[i] = queens[j];
    queens[j] = temp;
}
void shuffle(int arr[], int size) {

    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1); 
        swap_queens(i, j, arr);
    }
}
void initializeBoard(int* queens, int N) {
    for (int i = 0; i < N; i++) {
        queens[i] = i;
    }
    shuffle(queens, N);
}

int get_conflicts_count(int *queens, int N) {
    int conflicts_count = 0;
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



void simulated_annealing(int N,int target,int num_threads){
    int find_counters = 0;

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
        
            for (int i = 0; i < MAX_ITERATIONS*N && current_conflicts > 0 && temperature > TEMP_MINIMUM&&find_counters<target; i++) 
            {
            
                for (int queen = 0; queen < N; queen++) {

                    // randomly choose queens to swap
                    int xi = rand() % N;
                    int xj = rand() % N;
                    while (xj == queens[xi]) {
                        xj = rand() % N;
                    }
                    swap_queens(xi, xj, queens);
                    int new_conflicts = get_conflicts_count(queens, N);
                    if (accept(current_conflicts, new_conflicts,temperature)) {
                        current_conflicts = new_conflicts;
                    } else {
                        swap_queens(xi, xj, queens); // Undo the move
                    }
                }


                if (current_conflicts < best_conflicts) {
                    best_conflicts = current_conflicts;

                }
                if (best_conflicts == 0) {
                        find_counters++; // find one solution
                        // printf("%d/%d solutions found:\n",find_counters,target);
                        // for (int i = 0; i < N; i++) {
                        //     printf("%d ", best_queens[i]);
                        // }
                        // printf("\n");
                }
                // Cool the temperature
                temperature *= COOLING_RATE;

                
            }
        
        


        }
    

} 
double get_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec + tp.tv_usec / 1000000.0;
}
int main(int argc , char **argv) 
{
    // int target = 1;
    // int Ns[] = {8,10,12,14,16,18,20};
    // int Ns[] = {20,24,28,29,30,31,32};
    int Ns[] = {25,50,60,80,100};
    int targets[] = {1,3,5};
    int threads[] = {1,5,10,20};
    int run_times = 5;
    // if (argc == 2) {
    //     target = atoi(argv[1]);


    // }
    // else if (argc == 3) {
    //     target = atoi(argv[1]);
    //     num_threads=atoi(argv[2]);
    //     printf("target %d\n",target);
    //     printf("num_threads %d\n",num_threads);

    // }
    
    int length = sizeof(Ns) / sizeof(Ns[0]);
    for (int target_i=0;target_i<3;target_i++)
    {
        for (int thread_i=0;thread_i<4;thread_i++)
        {
            printf("target %d\n",targets[target_i]);
            printf("num_threads %d\n",threads[thread_i]);
        
            for (int i = 0; i < length; i++) {
                double time_diff, time_start;
                time_start = get_time();
                srand(time(NULL));
                seed = rand(); 
                for (int run_times_i = 0; run_times_i < run_times; run_times_i++) {
                    simulated_annealing(Ns[i],targets[target_i],threads[thread_i]);
                }
                
                time_diff = (get_time() - time_start)/run_times;

                printf("N %2d,Time %f sec.\n", Ns[i], time_diff);
            }
        }
    }
    return 0;
}
