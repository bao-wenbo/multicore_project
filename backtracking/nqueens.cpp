#include <algorithm>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <climits>
using namespace std;


class Solution {
public:
    vector<vector<string>> res;
    int found = 0;
    int queens_to_find = INT_MAX;

    vector<vector<string>> solveNQueens(int num_threads, int n) {
        vector<string> board(n, string(n, '.'));
        // backtracking(0, 0, n, board);
        // printf("Solutions to find: %d\n", queens_to_find);
        #pragma omp parallel num_threads(num_threads)
        loadBalance_v2(num_threads, n);
        // loadBalance(num_threads, n);
        return res;
    }

    void loadBalance(int thread_num, int n) {
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

    void loadBalance_v2(int thread_num, int n) {
        int my_rank = omp_get_thread_num();
        // int round = (n - 1) / thread_num;
        vector<string> local_board(n, string(n, '.'));

        int start_setting_count = 0;
        //col0 is the col of row 0
        for (int col0 = 0; col0 < n; col0++) {
            //col1 is the col of row 1
            for (int col1 = 0; col1 < n; col1++) {
                if (col1 <= col0+1 && col1 >= col0-1) 
                {
                    //Invalid set
                    continue;
                } 
                else 
                {
                    //This task should be done by this thread
                    //Set the first two rows
                    //There are n*(n-3)+2 unique start settings
                    if (start_setting_count % thread_num == my_rank)
                    {
                        local_board[0][col0] = 'Q';
                        local_board[1][col1] = 'Q';
                        //Start from row 2.
                        backtracking_v2(2, n, local_board);
                        local_board[0][col0] = '.';
                        local_board[1][col1] = '.';
                    }
                    //Not critical because it is only used for assign threads.
                    start_setting_count++;
                }
            }
        }

        // printf("%d start settings are set.\n", start_setting_count);

    }

    void backtracking_v2(int row, int n, vector<string> &board) {
        if (found >= queens_to_find)
            return;
        if (row == n)
        {
        #pragma omp critical
            res.push_back(board);
            found ++;
            return;
        }

        for (int col = 0; col < n; col++) {
            if (isValid(row, col, n, board)) {
                // path.push_back(vector<int> vect1{row, col});
                board[row][col] = 'Q';
                backtracking_v2(row + 1, n, board);
                board[row][col] = '.';
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
    if (argc != 4 && argc != 3) {
        printf("Wrong Input: Use ./nqueens num_threads num_queens [num_unique]");
        return -1;
    }
    int num_queens;
    int num_threads;
    int sol_to_find;
    sscanf(argv[1], "%d", &num_threads);
    sscanf(argv[2], "%d", &num_queens);
    Solution s;
    s.queens_to_find = INT_MAX;
    s.solveNQueens(num_threads, num_queens);
    if (argc == 4) {
        sscanf(argv[3], "%d", &sol_to_find);
    }
    cout << "Solution found: " << s.res.size() << endl;
}