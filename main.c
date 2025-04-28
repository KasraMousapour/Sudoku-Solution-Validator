#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 9


typedef struct {
    int row;    
    int col;    
    int type;   //0: row checking, 1: col checking, 2:subnet checking
    int index;  
} parameters;


int sudoku[N][N] = {
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,8,6,1,4,2,3},
    {4,2,6,7,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

// the array for seving results
int valid[27] = {0};

//thread function
void *check(void *arg) {
    parameters *param = (parameters *) arg;
    int i, j;
    int check_arr[10] = {0};   
    int validFlag = 1;         

    // row check
    if (param->type == 0) {
        int row = param->row;
        for (j = 0; j < N; j++) {
            int num = sudoku[row][j];
            if (num < 1 || num > 9 || check_arr[num] == 1) {
                validFlag = 0;
                break;
            } else {
                check_arr[num] = 1;
            }
        }
    }
    // col check
    else if (param->type == 1) {
        int col = param->col;
        for (i = 0; i < N; i++) {
            int num = sudoku[i][col];
            if (num < 1 || num > 9 || check_arr[num] == 1) {
                validFlag = 0;
                break;
            } else {
                check_arr[num] = 1;
            }
        }
    }
    // 3*3 subnet check
    else if (param->type == 2) {
        int startRow = param->row;
        int startCol = param->col;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                int num = sudoku[startRow + i][startCol + j];
                if (num < 1 || num > 9 || check_arr[num] == 1) {
                    validFlag = 0;
                    break;
                } else {
                    check_arr[num] = 1;
                }
            }
            if (!validFlag)
                break;
        }
    }
    valid[param->index] = validFlag;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[27];
    parameters params[27];
    int threadIndex = 0;
    int i, j;

    // init 9 thread for rows
    for (i = 0; i < N; i++) {
        params[threadIndex].row = i;
        params[threadIndex].col = 0;
        params[threadIndex].type = 0;   
        params[threadIndex].index = threadIndex;
        if (pthread_create(&threads[threadIndex], NULL, check, (void *)&params[threadIndex])) {
            fprintf(stderr, "Error creating thread for row %d\n", i);
            return 1;
        }
        threadIndex++;
    }

    // init 9 thread for cols
    for (i = 0; i < N; i++) {
        params[threadIndex].row = 0;
        params[threadIndex].col = i;
        params[threadIndex].type = 1;   
        params[threadIndex].index = threadIndex;
        if (pthread_create(&threads[threadIndex], NULL, check, (void *)&params[threadIndex])) {
            fprintf(stderr, "Error creating thread for column %d\n", i);
            return 1;
        }
        threadIndex++;
    }

    // init 9 thread for subnets
    for (i = 0; i < N; i += 3) {
        for (j = 0; j < N; j += 3) {
            params[threadIndex].row = i;
            params[threadIndex].col = j;
            params[threadIndex].type = 2;   
            params[threadIndex].index = threadIndex;
            if (pthread_create(&threads[threadIndex], NULL, check, (void *)&params[threadIndex])) {
                fprintf(stderr, "Error creating thread for subnet starting at (%d, %d)\n", i, j);
                return 1;
            }
            threadIndex++;
        }
    }
    

    for (i = 0; i < 27; i++) {
        pthread_join(threads[i], NULL);
    }

    // final check
    int isValid = 1;
    for (i = 0; i < 27; i++) {
        if (!valid[i]) {
            isValid = 0;
            break;
        }
    }

    if (isValid)
        printf("The Sudoku solution is valid.\n");
    else
        printf("The Sudoku solution is invalid.\n");

    return 0;
}
