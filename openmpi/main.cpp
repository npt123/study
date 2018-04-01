#include <iostream>
#include <mpi.h>
using namespace std;

#define slaver_num 2
#define min -1024.1024
#define max 1024.1024
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

// 计算矩阵对应单元的结果
double calculateUnit(double line[], int col) {
    double result = 0.0;
    for (int i = 0; i < first_col; ++i) {
        result += line[i] * secondMatrix[i][col];
    }
    return result;
}

int main(int argc, char *argv[]) {
    int rank, tot;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tot);
    MPI_Status status;
    MPI_Status send_status[first_row];
    MPI_Status recv_status[first_row];
    MPI_Request send[first_row];
    MPI_Request recv[first_row];

    if (rank == 0) {
        matrixInit();
        double t1 = MPI_Wtime();
        for (int i = 0; i < slaver_num; ++i) {
            MPI_Send(&secondMatrix[0], second_row*second_col, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
//            cout << "Send secondMatrix to " << i+1 << endl;
        }
        for (int i = 0; i < first_row; ++i) {
            MPI_Isend(&firstMatrix[i], first_col, MPI_DOUBLE, i%slaver_num+1, i+1, MPI_COMM_WORLD, &send[i]);
//            cout << "Send firstMatrix to " << i%slaver_num+1 << endl;
        }
        for (int i = 0; i < first_row; ++i) {
            MPI_Recv(&resultMatrix[i], second_col, MPI_DOUBLE, i%slaver_num+1, i+1025, MPI_COMM_WORLD, &recv_status[i]);
        }
//        MPI_Waitall(first_row, recv, recv_status);
        double t2 = MPI_Wtime();
        cout << "Time: " << t2-t1 << endl;
    } else {
        MPI_Recv(&secondMatrix[0], second_row*second_col, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
//        cout << rank << " receive from master: secondMatrix" << endl;
        double line[first_col] = {0.0};
        double result[second_col] = {0.0};
        for (int i = 0; i < first_row/slaver_num; ++i) {
            MPI_Recv(&line, first_col, MPI_DOUBLE, 0, i*slaver_num+rank, MPI_COMM_WORLD, &recv_status[i]);
//            cout << rank << " receive from master: firstMatrix[" << i*slaver_num+rank << "]" << endl;
            for (int j = 0; j < second_row; ++j) {
                result[j] = calculateUnit(line, j);
            }
            MPI_Isend(&result, second_col, MPI_DOUBLE, 0, i*slaver_num+rank+1024, MPI_COMM_WORLD, &send[i]);
//            for (int k = 0; k < second_row; ++k) {
//                cout << rank << " send resultMatrix[" << i*slaver_num << "][" << k << "]: " << result[k] << endl;
//            }
        }
        MPI_Waitall(first_row/slaver_num, send, send_status);
    }

    MPI_Barrier(MPI_COMM_WORLD);

//    if (rank == 0) {
//        for (int i = 0; i < first_row; ++i) {
//            for (int j = 0; j < second_col; ++j) {
//                cout << resultMatrix[i][j] << " ";
//            }
//            cout << endl;
//        }
//    }

    MPI_Finalize();
    return 0;
}