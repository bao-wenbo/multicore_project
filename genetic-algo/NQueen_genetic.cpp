// Sequential Genetic Algorithm for N-Queen
// Reference: https://github.com/hiradbaba/GANQ/blob/master/Borad.py

#include<iostream>
#include<cmath>
#include<cstdlib>
#include<algorithm>
#include<sstream>
#include<cstring>
#include<vector>
#include <random>
#include<limits>
#include<ctime>

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

// thread-safe random number generator
int intRand(const int & min, const int & max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(min,max);
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
            for (int i = 0; i < size; i++) {
                board[chromosome[i]][i] = 1;
            }
        }

        int fitness(const vector<int>& chromosome) {
            int hits = 0;
            vector<vector<int>> board = createBoard(size);
            setBoard(board, chromosome);

            for (int col = 0; col < size; col++) {
                int dna = chromosome[col];
                for (int i = col - 1; i >= 0; i--) {
                    if (board[dna][i] == 1) {
                        hits++;
                    }
                }
                for (int i = dna - 1, j = col - 1; i >= 0 && j >= 0; i--, j--) {
                    if (board[i][j] == 1) {
                        hits++;
                    }
                }
                for (int i = dna + 1, j = col - 1; i < size && j >= 0; i++, j--) {
                    if (board[i][j] == 1) {
                        hits++;
                    }
                }
            }
            return hits;
        }

        void crossOver(vector<int>& first, vector<int>& second) {
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
            int leftSideIndex = rand() % (bound + 1);
            int rightSideIndex = rand() % (size - bound - 1) + bound + 1;

            vector<int> newChromosome;
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

            for (int i = 1; i < cur_population_size; i+=2) {
                vector<int> first = population[i - 1];
                vector<int> second = population[i];
                crossOver(first, second);
                first = Mutant(first);
                second = Mutant(second);
                population[cur_population_size + i - 1] = first;
                population[cur_population_size + i] = second;
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
            // check if chromosome meet condition
            for (const auto& chromosome : population) {
                if (isSolution(chromosome)) {
                    solutions.push_back(chromosome);
                    printf("Found %d Solutions\n", solutions.size());
                    if (print_solutions)
                        print_solution(chromosome, size);
                    if (solutions.size() >= num_solutions)
                        return chromosome;
                }
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
    unsigned int seed = 0;
    srand(seed);
    // check input parsing
    if (argc != 5) {
        printf("Usage: ./NQueen_genetic nq population_size num_solutions print_solutions\n");
        return 1;
    }

    int nq = atoi(argv[1]);
    int ps = atoi(argv[2]);
    int num_sol = atoi(argv[3]);
    bool print_sol = atoi(argv[4]);

    vector<vector<int>> solutions;
    vector<int> solution;

    NQueenSolver nqsolver(nq, ps, num_sol, print_sol);
    solution = nqsolver.solveGA();

    return 0;
}
