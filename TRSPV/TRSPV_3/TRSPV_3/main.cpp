

#include <iostream>
#include <mpi.h>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace std;

static MPI_Comm graph;
static MPI_Comm comm;
static int processRank;
static int processCount;
static int **matrixA, **matrixB, **matrixC, *row, *column, *tmpColumn, *result;

int nextProcess();
void initBuffers(int *rowsBuf, int *columnsBuf);
void createGraph();
void initRowAndColumn(int *rowsBuf, int *columnsBuf, int *row, int *column);
void iteration(int *row, int *column, int *result, int index);
void colsExchange(int *column, int *tmpColumn);
void tapeMultiplication(int *row, int *column, int *result, int *tmpColumn);
void resultRowsGather();
void rootProc();
void otherProc();

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &processRank);
    MPI_Comm_size(comm, &processCount);
    createGraph();
    
    if (processRank == 0) {
        rootProc();
    }
    otherProc();
    
    MPI_Finalize();

    return 0;
}


int nextProcess() {

    int rankToNextProcess;
    MPI_Graph_neighbors(graph, processRank, 1, &rankToNextProcess);
    return rankToNextProcess;

}

void initBuffers(int *rowsBuf, int *columnsBuf) {
     int k = 0;
     for (int i = 0; i < processCount; i++) {
         for (int j = 0; j < processCount; j++) {
             rowsBuf[k] = matrixA[i][j];
             columnsBuf[k++] = matrixB[j][i];
         }
     }
 }


void createGraph() {

    int n = processCount;
    int *index = new int[processCount];
    int *edges = new int[processCount];
    for (int i = 1; i <= processCount; i++) {
        index[i - 1] = i;
        edges[i - 1] = i % processCount;
    }
    
    MPI_Barrier(comm);

    MPI_Graph_create(comm, n, index, edges, 0, &graph);
    MPI_Comm_size(graph, &processCount);

    MPI_Comm_rank(graph, &processRank);

    delete[] index;
    delete[] edges;

}

void initRowAndColumn(int *rowsBuf, int *columnsBuf, int *row, int *column) {

    MPI_Barrier(graph);
    MPI_Scatter(rowsBuf, processCount, MPI_INT, row, processCount, MPI_INT, 0, graph);
    MPI_Barrier(graph);
    MPI_Scatter(columnsBuf, processCount, MPI_INT, column, processCount, MPI_INT, 0, graph);

}



void iteration(int *row, int *column, int *result, int index) {

    result[index] = 0;
    for (int i = 0; i < processCount; result[index] += row[i] * column[i], i++);

}



void colsExchange(int *column, int *tmpColumn) {

    MPI_Status status;
    int next;
    next = !processRank ? processCount - 1 : processRank - 1;
    if (!(processRank % 2)) {
        MPI_Barrier(graph);
        MPI_Send(column, processCount, MPI_INT, next, 0, graph);
    }

    MPI_Barrier(graph);
    MPI_Recv(tmpColumn, processCount, MPI_INT, nextProcess(), MPI_ANY_TAG, graph, &status);

    if (processRank % 2) {
        MPI_Barrier(graph);
        MPI_Send(column, processCount, MPI_INT, next, 0, graph);
    }

}

void tapeMultiplication(int *row, int *column, int *result, int *tmpColumn) {

    int index = processRank;

    for (int i = 0; i < processCount; i++) {

        iteration(row, column, result, index);

        if (i != processCount - 1) {
            colsExchange(column, tmpColumn);
            for (int j = 0; j < processCount; column[j] = tmpColumn[j], j++);
        }

        index = (index + 1) % processCount;
    }
}

void resultRowsGather() {

    int *tmpBuf;
    
    if (processRank == 0) {
        tmpBuf = new int[processCount * processCount];
    } else {
        tmpBuf = new int[1];
    }

    MPI_Barrier(graph);
    MPI_Gather(result, processCount, MPI_INT, tmpBuf, processCount, MPI_INT, 0, graph);

    if (processRank == 0) {
        int k = 0;

        for (int i = 0; i < processCount; i++) {
            for (int j = 0; j < processCount; matrixC[i][j++] = tmpBuf[k++]);
        }
    }
}



void rootProc() {
    
    matrixA = new int *[processCount];
    matrixB = new int *[processCount];
    matrixC = new int *[processCount];

    for (int i = 0; i < processCount; i++) {
        matrixA[i] = new int[processCount];
        matrixB[i] = new int[processCount];
        matrixC[i] = new int[processCount];
    }

    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processCount; j++) {
            matrixA[i][j] = rand() % 9;
        }
    }
    
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processCount; j++) {
            matrixB[i][j] = rand() % 9;
        }
    }
    
    
    result = new int[processCount];
    row = new int[processCount];
    column = new int[processCount];
    tmpColumn = new int[processCount];
    int *rowsBuf = new int[processCount * processCount];
    int *columnsBuf = new int[processCount * processCount];
    initBuffers(rowsBuf, columnsBuf);
    initRowAndColumn(rowsBuf, columnsBuf, row, column);
    tapeMultiplication(row, column, result, tmpColumn);
    resultRowsGather();

    cout<<"matrixA * matrixB = matrixC"<<endl;
    cout<<endl;
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processCount; j++) {
           cout<<matrixA[i][j]<<" ";
        }
        cout<<endl;
    }
    
    cout << "\t*" << endl;
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processCount; j++) {
           cout<<matrixB[i][j]<<" ";
        }
        cout<<endl;
    }
    
    cout << "\t=" << endl;
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processCount; j++) {
           cout<<matrixC[i][j]<<" ";
        }
        cout<<endl;
    }

    delete matrixA;
    delete matrixB;
    delete matrixC;
    delete result;
    delete row;
    delete column;
    delete tmpColumn;
    delete[] rowsBuf;
    delete[] columnsBuf;

 }

void otherProc() {

    result = new int[processCount];
    row = new int[processCount];
    column = new int[processCount];
    tmpColumn = new int[processCount];
    int *rowsBuf = new int[1];
    int *columnsBuf = new int[1];
    initRowAndColumn(rowsBuf, columnsBuf, row, column);
    tapeMultiplication(row, column, result, tmpColumn);
    resultRowsGather();
    delete[] result;
    delete[] row;
    delete[] column;
    delete[] tmpColumn;
    delete[] rowsBuf;
    delete[] columnsBuf;

 }


