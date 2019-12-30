#include<mpi.h>
#include<iostream>
#include<stdio.h>
#include "math.h"

const size_t MATRIX_SIZE = 4;
const size_t MATRIX_SIZE_3 = 3;
const int TAG = 0;
const int ROOT_PROCCESS = 0;
const size_t MATRIX_3x3_DTERMINANT_CALC_ITERATION_COUNT = 6;
using namespace std;

double* GetMatrixFactor(double** matrix, size_t size, int proccess);
void PrintMatrix(double** matrix, size_t size);
void PrintRow(double* row, int size);
double** Row_to_matrix(double* matrix_row, size_t matrix_size);
double* Matrix_to_row(double** matrix, size_t matrix_size);

int main(int argc, char** argv)
{
    double** matrix = new double* [MATRIX_SIZE]
    {
            new double[MATRIX_SIZE] {1., 2., 4., 1.},
            new double[MATRIX_SIZE] {3., 4., 1., 2.},
            new double[MATRIX_SIZE] {3., 4., 1., 1.},
            new double[MATRIX_SIZE] {1., 2., 3., 7.}
    };

    // 3x3 matrix create
    double** matrix3;

    double* matrix3_row;
    int proccess_rank = 0;
    int proccess_count = 0;
    int buff_size = 0;
    MPI_Status status;
    double* temp_row;
    double result = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proccess_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proccess_rank);

    double determinant = 0.0;

    if (proccess_rank == ROOT_PROCCESS) {
        cout << "Proccess count: " << proccess_count << endl;
        PrintMatrix(matrix, MATRIX_SIZE);

        matrix3 = new double* [MATRIX_SIZE_3];

        for (size_t i = 0; i < MATRIX_SIZE; ++i)
        {
            matrix3[i] = new double[MATRIX_SIZE_3];
        }

        // 3x3 matrix init
        for (size_t z = 0; z < proccess_count - 1; ++z)
        {
            for (size_t k = 0, index_counter = 0; k < MATRIX_SIZE; ++k)
            {
                if (z != k) {
                    for (size_t j = 1; j < MATRIX_SIZE; ++j)
                    {
                        matrix3[index_counter][j - 1] = matrix[k][j];
                    }
                    ++index_counter;
                }
            }

            matrix3_row = Matrix_to_row(matrix3, MATRIX_SIZE_3);
            MPI_Send(matrix3_row, (MATRIX_SIZE_3) * (MATRIX_SIZE_3), MPI_DOUBLE, z + 1, TAG, MPI_COMM_WORLD);
        }

        for (size_t z = 0; z < MATRIX_SIZE; ++z)
        {
            MPI_Recv(&determinant, 1, MPI_DOUBLE, z + 1, TAG, MPI_COMM_WORLD, &status);

#pragma region Determinant 4x4 calculation
                result += matrix[z][0] * determinant * pow(-1, z + 2);
#pragma endregion
        }
        cout << "Determinant 4x4: " << result << endl;

    }
    else {
        MPI_Probe(ROOT_PROCCESS, TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_DOUBLE, &buff_size);

        matrix3_row = new double[buff_size];
        size_t matrix3_size = sqrt(buff_size);
        MPI_Recv(matrix3_row, buff_size, MPI_DOUBLE, ROOT_PROCCESS, TAG, MPI_COMM_WORLD, &status);
        matrix3 = Row_to_matrix(matrix3_row, matrix3_size);
        //PrintRow(matrix3_row, buff_size);
        //PrintMatrix(matrix3, matrix3_size);
        for (int i = 0; i < MATRIX_3x3_DTERMINANT_CALC_ITERATION_COUNT; ++i)
        {
            double temp_result = 1.0;
            temp_row = GetMatrixFactor(matrix3, matrix3_size, i + 1);

            for (int j = 0; j < matrix3_size; ++j)
            {
                temp_result *= temp_row[j];

            }
#pragma region 3x3 Determinants Calculation
                determinant += (i < 3)
                    ? temp_result
                    : temp_result * (-1);

            delete[] temp_row;
#pragma endregion
        }
        cout << "Proccess - "<< proccess_rank << " determinant: " << determinant << endl;
        MPI_Send(&determinant, 1, MPI_DOUBLE, ROOT_PROCCESS, TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    // Delock memory
    for (size_t i = 0; i < MATRIX_SIZE; ++i)
    {
        delete[] matrix[i];
    }

    for (size_t i = 0; i < MATRIX_SIZE_3; ++i)
    {
        delete[] matrix3[i];
    }

    delete[] matrix;
    //delete matrix3;
    delete matrix3_row;

    return 0;
}

void PrintRow(double* row, int size) {

    for (size_t j = 0; j < size; ++j)
    {
        cout << row[j] << " ";
    }

    cout << endl;
}

void PrintMatrix(double** matrix, size_t size) {
    cout << "Matrix:" << endl;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            cout << matrix[i][j] << " ";
        }

        cout << endl;
    }
}

double* GetMatrixFactor(double** matrix, size_t size, int proccess) {

    switch (proccess) {
    case 1: {
        double* a = new double[size];
        a[0] = matrix[0][1];
        a[1] = matrix[1][2];
        a[2] = matrix[2][0];

        return a;
    }
    case 2: {
        return new double[size] {
            matrix[0][2],
                matrix[1][0],
                matrix[2][1]
        };
    }
    case 3: {
        return new double[size] {
            matrix[0][0],
                matrix[1][1],
                matrix[2][2]
        };
    }
    case 4: {
        return new double[size] {
            matrix[0][0],
                matrix[1][2],
                matrix[2][1]
        };
    }
    case 5: {
        return new double[size] {
            matrix[0][1],
                matrix[1][0],
                matrix[2][2]
        };
    }
    case 6: {
        return new double[size] {
            matrix[0][2],
                matrix[1][1],
                matrix[2][0]
        };
    }
    }

    return NULL;
}

double** Row_to_matrix(double* matrix_row, size_t matrix_size) {

    double** result = new double* [MATRIX_SIZE_3];

    for (size_t i = 0; i < matrix_size; ++i)
    {
        result[i] = new double[MATRIX_SIZE_3];
    }

    for (size_t k = 0, i = 0; k < matrix_size; ++k)
    {
        for (size_t j = 0; j < matrix_size; ++j, ++i)
        {
            result[k][j] = *(matrix_row + i);
        }
    }

    return result;

}

double* Matrix_to_row(double** matrix, size_t matrix_size) {
    double* result = new double[matrix_size * matrix_size];

    for (size_t k = 0, i = 0; k < matrix_size; ++k)
    {
        for (size_t j = 0; j < matrix_size; ++j, ++i)
        {
            *(result + i) = matrix[k][j];
        }
    }

    return result;
}
