// Parallel Genetic Algorithm for N-Queen

#include<iostream>
#include<cmath>
#include<cstdlib>
#include<algorithm>
#include<sstream>
#include<cstring>
#include<vector>
#include<ctime>
#include <random>
#include<limits>
#include<omp.h>

using namespace std;

int fact(int n) {
    if (n == 0 || n == 1)
        return 1;
    else
        return n * fact(n - 1);
}

void print_board(const vector<vector<string>> &board) {
    for (const auto &row : board) {
        for (const auto &cell : row) {
            cout << cell << ' ';
        }
        cout << endl;
    }
}

void print_solution(const vector<int> solution, int nq) {
    cout << "Solution: ";
    for (int i = 0; i < nq; i++)
        cout << solution[i] << " ";
    cout << endl;

    vector<vector<string>> board(nq, vector<string>(nq, "X"));

    for (int i = 0; i < nq; i++)
        board[nq-solution[i]-1][i] = "Q";

    cout << endl;
    print_board(board);
}

void init_generator(unsigned int seed) {
    static thread_local std::mt19937 generator(seed);
}

// thread-safe random number generator
int intRand(const int & min, const int & max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

class NQueenSolver {
    public:
        NQueenSolver(int n, int ps, int num_sol, bool print_sol) {
            goalIndex = -1;
            size = n;
            population_size = ps;
            num_solutions = num_sol;
            print_solutions = print_sol;
            population.resize(population_size);

            for (int i = 0; i < population_size; ++i) {
                population[i].resize(size, -1);
            }

            initializeFirstGeneration();
        }

        vector<int> generateChromosome() {
            vector<int> chromosome(size);
            #pragma omp parallel for shared(chromosome)
            for (int i = 0; i < size; i++) {
                chromosome[i] = i;
            }

            random_shuffle(chromosome.begin(), chromosome.end());

            while (count(population.begin(), population.end(), chromosome)) {
                random_shuffle(chromosome.begin(), chromosome.end());
            }
            return chromosome;
        }

        void initializeFirstGeneration() {
            int max_population = fact(size);
            if (population_size > max_population && max_population != 0) {
                printf("Population Size larger than maximum possible combination of N. Adjust to maximum %d\n", max_population);
                population_size = max_population;
            }

            #pragma omp parallel for shared(population)
            for (int i = 0; i < population_size; i++) {
                vector<int> chromosome = generateChromosome();
                population[i] = chromosome;
            }
        }

        vector<vector<int>> createBoard(int n) {
            vector<vector<int>> board(n, vector<int>(n, 0));
            return board;
        }

        void setBoard(vector<vector<int>>& board, const vector<int>& chromosome) {
            int size = board.size();
            #pragma omp parallel for shared(board, chromosome)
            for (int i = 0; i < size; i++) {
                board[chromosome[i]][i] = 1;
            }
        }

        int fitness(const vector<int>& chromosome) {
            int hits = 0;
            vector<vector<int>> board = createBoard(size);
            setBoard(board, chromosome);
            // check conficts
            #pragma omp parallel for shared(chromosome), reduction(+:hits)
            for (int col = 0; col < size; col++) {
                int dna = chromosome[col];
                
                for (int offset = 1; col - offset >= 0; offset++) {
                    // Check horizontally
                    if (board[dna][col - offset] == 1) {
                        hits++;
                    }
                    
                    // Check upward diagonal
                    if (dna - offset >= 0 && board[dna - offset][col - offset] == 1) {
                        hits++;
                    }
                    
                    // Check downward diagonal
                    if (dna + offset < size && board[dna + offset][col - offset] == 1) {
                        hits++;
                    }
                }
            }
            return hits;
        }

        void crossOver(vector<int>& first, vector<int>& second) {
            // swap may give race condition, no efficient to parallelize this loop
            for (size_t i = 1; i < first.size(); i++) {
                if (abs(first[i - 1] - first[i]) < 2) {
                    swap(first[i], second[i]);
                }
                if (abs(second[i - 1] - second[i]) < 2) {
                    swap(first[i], second[i]);
                }
            }
        }

        vector<int> Mutant(vector<int> chromosome) {
            int bound = size / 2;
            int leftSideIndex = intRand(0, numeric_limits<int>::max()) % (bound + 1);
            int rightSideIndex = intRand(0, numeric_limits<int>::max()) % (size - bound - 1) + bound + 1;

            vector<int> newChromosome;
            // need to be sequential because dna and newChromosome are dependent
            for (int dna : chromosome) {
                if (find(newChromosome.begin(), newChromosome.end(), dna) == newChromosome.end()) {
                    newChromosome.push_back(dna);
                }
            }
            for (int i = 0; i < size; i++) {
                if (find(newChromosome.begin(), newChromosome.end(), i) == newChromosome.end()) {
                    newChromosome.push_back(i);
                }
            }

            chromosome = newChromosome;
            swap(chromosome[leftSideIndex], chromosome[rightSideIndex]);
            return chromosome;
        }

        void crossOverAndMutant() {
            int cur_population_size = population.size();

            if (cur_population_size % 2 == 0)
                population.resize(cur_population_size * 2);
            else
                population.resize(cur_population_size * 2 - 1);

            #pragma omp parallel for ordered schedule(static, 2)
            for (int i = 1; i < cur_population_size; i += 2) {
                vector<int> first = population[i - 1];
                vector<int> second = population[i];
                crossOver(first, second);
                first = Mutant(first);
                second = Mutant(second);
                #pragma omp critical
                {
                    population[cur_population_size + i - 1] = first;
                    population[cur_population_size + i] = second;
                }
            }
        }

        bool isSolution(const vector<int>& chromosome) {
            if (fitness(chromosome) == 0)
                return true;
            return false;
        }

        vector<vector<int>> makeSelection() {
            vector<int> fitnesses(population.size());
            vector<vector<int>> newPopulation;
            int minIndex;

            #pragma omp parallel for
            for (size_t i = 0; i < population.size(); i++)
                fitnesses[i] = fitness(population[i]);

            if (*min_element(fitnesses.begin(), fitnesses.end()) == 0) {
                auto minElementIt = min_element(fitnesses.begin(), fitnesses.end());
                goalIndex = distance(fitnesses.begin(), minElementIt);
                solution = population[goalIndex];
                return population;
            }

            while (newPopulation.size() < size) {
                auto minElementIt = min_element(fitnesses.begin(), fitnesses.end());
                minIndex = distance(fitnesses.begin(), minElementIt);
                newPopulation.push_back(population[minIndex]);
                fitnesses.erase(minElementIt);
                population.erase(population.begin() + minIndex);
            }

            return newPopulation;
        }

        vector<int> solveGA() {
            bool solution_found = false;
            #pragma omp parallel for shared(solution, solution_found)
            for (size_t i = 0; i < population.size(); i++) {
                #pragma omp flush(solution_found)
                if (!solution_found && isSolution(population[i])) {
                    #pragma omp critical
                    {
                        if (!solution_found) {
                            solution = population[i];
                            solution_found = true;
                            #pragma omp flush(solution_found)
                        }
                    }
                }
            }
            if (solution_found) {
                solutions.push_back(solution);
                printf("Found %d Solutions\n", solutions.size());
                if (print_solutions)
                    print_solution(solution, size);
                if (solutions.size() >= num_solutions)
                    return solution;
            }

            int count = 0;
            while (true) {
                crossOverAndMutant();
                population = makeSelection();
                count++;

                if (goalIndex >= 0) {
                    // if solution not in current solutinos, add in
                    if (find(solutions.begin(), solutions.end(), solution) == solutions.end()) {
                        solutions.push_back(solution);
                        printf("Found %d Solutions\n", solutions.size());
                        if (print_solutions)
                            print_solution(solution, size);
                    }
                    
                    // if all required solutions found, return
                    // else reset solution at current goal position
                    if (solutions.size() >= num_solutions)
                        return solution;
                    else {
                        population[goalIndex] = generateChromosome();
                        goalIndex = -1;
                    }
                }
            }
        }

    private:
        vector<vector<int>> population;
        vector<vector<int>> solutions;
        vector<int> solution;
        int size;
        int num_solutions;
        int population_size;
        int goalIndex;
        bool print_solutions;
};

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned>(time(0)));
    // check input parsing
    if (argc != 6) {
        printf("Usage: ./NQueen_genetic nq population_size num_solutions num_threads print_solutions\n");
        return 1;
    }

    int nq = atoi(argv[1]);
    int ps = atoi(argv[2]);
    int num_sol = atoi(argv[3]);
    int num_threads = atoi(argv[4]);
    bool print_sol = atoi(argv[5]);

    vector<vector<int>> solutions;
    vector<int> solution;

    omp_set_num_threads(num_threads);
    // fix seed
    init_generator(0);

    NQueenSolver nqsolver(nq, ps, num_sol, print_sol);
    solution = nqsolver.solveGA();

    return 0;
}
