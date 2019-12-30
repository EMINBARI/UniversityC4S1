#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

void MultiplicationMatrix(int matrA2[2][5],int matrB[5][3], int semiResult[2][3]);

int main(int argc, char **argv) {
    int rank;
    MPI_Status status;
    
    int arrayA[8][5]={
        { 2, 2, 2, 2, 2 },
        { 1, 1, 1, 1, 1 },
        { 3, 3, 3, 3, 3 },
        { 4, 4, 4, 4, 4 },
        { 5, 5, 5, 5, 5 },
        { 6, 6, 6, 6, 6 },
        { 2, 2, 2, 2, 2 },
        { 0, 0, 0, 0, 0 },
    };

    int arrayB[5][3]={
        { 3, 3, 3 },
        { 3, 3, 3 },
        { 3, 3, 3 },
        { 3, 3, 3 },
        { 3, 3, 3 },
    };
    
    int arrayA2[2][5];
    int arrayC[8][3];
    int resultArray[2][3];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Scatter(&arrayA, 10, MPI_INT, &arrayA2, 10, MPI_INT, rank, MPI_COMM_WORLD);
    MPI_Bcast(&arrayB, 15, MPI_INT, 0, MPI_COMM_WORLD);
    
    MultiplicationMatrix(arrayA2, arrayB, resultArray);

    MPI_Gather(resultArray, 6, MPI_INT, &arrayC, 6, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0){
        cout<<"------Результатирующая матрица:------"<<endl;
        for (int  i = 0; i < 8; i++){
            for(int j = 0; j < 3 ; j++){
                cout<<arrayC[i][j]<<" ";
            }
            cout<<endl;
        }
    }

    MPI_Finalize();
    return 0;
}

void MultiplicationMatrix(int arrayA2[2][5],int arrayB[5][3], int semiResult[2][3]){
    for(int k=0; k<2; k++){
        for (int  i = 0; i < 3; i++){
                for(int j = 0; j < 5 ; j++){
                    semiResult[k][i] += arrayA2[k][j] * arrayB[j][i];
            }
        }
    }
}
