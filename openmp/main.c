#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define min -65536.65536
#define max 65536.65536
#define first_row 1024
#define first_col 1024
#define second_row 1024
#define second_col 1024
double firstMatrix[first_row][first_col] = {0.0};
double secondMatrix[second_row][second_col] = {0.0};
double resultMatrix[first_row][second_col] = {0.0};

// 随机数生成器
double rand_double() {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// 计算矩阵对应单元的结果
double calculateUnit(int row, int col) {
    double result = 0.0;
    int k = 0;
    #pragma omp parallel for private(k) reduction(+:result)
    for (k = 0; k < first_col; ++k) {
        result += firstMatrix[row][k] * secondMatrix[k][col];
    }
    return result;
}

// 矩阵乘法运算并行
void matrixMultiOpenMp() {
    int i, j;
    #pragma omp parallel for private(i, j)
    for (i = 0; i < first_row; ++i) {
        for (j = 0; j < second_col; ++j) {
            resultMatrix[i][j] = calculateUnit(i, j);
        }
    }
}

// 矩阵乘法运算串行
void matrixMulti() {
    for (int i = 0; i < first_row; ++i) {
        for (int j = 0; j < second_col; ++j) {
            resultMatrix[i][j] = calculateUnit(i, j);
        }
    }
}

// 随机矩阵生成
void matrixInit() {
    for (int i = 0; i < first_row; ++i) {
        for (int j = 0; j < first_col; ++j) {
            firstMatrix[i][j] = rand_double();
        }
    }
    for (int i = 0; i < first_row; ++i) {
        for (int j = 0; j < first_col; ++j) {
            secondMatrix[i][j] = rand_double();
        }
    }
}

int main(int argc, char *argv[]) {
    matrixInit();
    clock_t t1 = clock();
    matrixMulti();
    clock_t t2 = clock();
    printf("Serial Time: %Lf\n", (long double)(t2 - t1));

#ifndef _OPENMP
    printf("OpenMP not supported\n");
#endif
    matrixInit();
    clock_t t3 = clock();
    matrixMultiOpenMp();
    clock_t t4 = clock();
    printf("Parallel Time: %Lf\n", (long double)(t4 - t3));

    return 0;
}
