#include <algorithm>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <omp.h>
using namespace std;


class Solution {
public:
    vector<vector<string>> res;
    int found = 0;
    int queens_to_find = 100;

    vector<vector<string>> solveNQueens(int num_threads, int n) {
        vector<string> board(n, string(n, '.'));
        // backtracking(0, 0, n, board);
        #pragma omp parallel num_threads(num_threads)
        divideBacktracking(num_threads, n);
        return res;
    }

    void divideBacktracking(int thread_num, int n) {
        int my_rank = omp_get_thread_num();
        int round = 1;
        vector<string> local_board(n, string(n, '.'));
        round = (n - 1) / thread_num;
        for (int i = 0; i <= round; i++)
        {
            if (i * thread_num + my_rank < n) 
            {
                backtracking(0, i * thread_num + my_rank, n, local_board);
            }
        }
    }

    void backtracking(int row, int start_col, int n, vector<string> &board) {
        if (found >= queens_to_find)
            return;
        if (row == n)
        {
        #pragma omp critical
            res.push_back(board);
            found ++;
            return;
        }

        if (row == 0) {
            board[row][start_col] = 'Q';
            backtracking(row + 1, start_col, n, board);
            board[row][start_col] = '.';
        } else {
            for (int col = 0; col < n; col++) {
                if (isValid(row, col, n, board)) {
                    // path.push_back(vector<int> vect1{row, col});
                    board[row][col] = 'Q';
                    backtracking(row + 1, 0, n, board);
                    board[row][col] = '.';
                }
            }
        }
        
    }

    bool isValid(int row, int col, int n, vector<string> &board) {
        //Not necessary to check current row.
        //Check current column
        for (int i = 0; row - i >= 0; i++) {
            if (board[row - i][col] == 'Q') return false;
        }
        //Check upper left diagonal
        //[row][col] -> [row-1][col-1]
        for (int i = 1; row - i >= 0 && col - i >= 0; i++) {
            if (board[row - i][col - i] == 'Q') return false;
        }
        //Check upper right diagonal
        //[row][col] -> [row-1][col+1];
        for (int i = 1; row - i >= 0 && col + i < n; i++) {
            if (board[row - i][col + i] == 'Q') return false;
        }
        return true;
    }
};

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Wrong Input: Use ./nqueens num_threads num_queens ");
        return -1;
    }
    int num_queens;
    int num_threads;
    int sol_to_find;
    sscanf(argv[1], "%d", &num_threads);
    sscanf(argv[2], "%d", &num_queens);
    sscanf(argv[3], "%d", &sol_to_find);

    Solution s;
    s.queens_to_find = sol_to_find;
    s.solveNQueens(num_threads, num_queens);

    cout << "Solution found: " << s.res.size() << endl;
}