#include <iostream>
#include <ctime>
#include <sstream>
#include <mpi.h>
using namespace std;

int procNum = 0;
int procRank = 0;
MPI_Status status;

void numbers(int* &data,int &countNumbers,int &countNumInProc){
    srand(6); // рандомизация генератора случайных чисел
    cout<<"Введите количество чисел для сортировки: ";
    cin>>countNumbers;
    if( countNumbers % procNum != 0){
        cout<<"Ошибка: количество чисел должно быть кратно количеству процессов!!!"<<endl;
        exit(-1);
    }

    countNumInProc = countNumbers/procNum;
    data = new int[countNumbers];
    
    for(int i = 0; i < countNumbers; i++) {
        data[i] = rand()%100;
    }
}

void sort(int* &data_1, int* &data_2, int length, bool smaller){
    int length_mass = length*2;
    int *mass=new int[length_mass];
    
    for(int i=0, j=0;i<length;i++){
        mass[j++]=data_1[i];
        mass[j++]=data_2[i];
    }
    for(int k=0; k<length_mass; k++){
        for(int i=0;i<length_mass-1;i++){
            if(mass[i]>mass[i+1]){
                int tmp = mass[i];
                mass[i] = mass[i+1];
                mass[i+1] = tmp;
            }
        }
    }
    if(smaller){
        for(int i=0;i<length;i++){
            data_1[i] = mass[i];
        }
    }
    else{
        for(int i=0;i<length;i++){
            data_1[i] = mass[length+i];
        }
    }
    
}

void predSort(int* &mass,int length){
    for(int k=0; k<length; k++){
        for(int i=0;i<length-1;i++){
            if(mass[i]>mass[i+1]){
                int tmp = mass[i];
                mass[i] = mass[i+1];
                mass[i+1] = tmp;
            }
        }
    }
}

int main(int argc, char **argv) {
    
    int countNumbers,countNumInProc;
    int* data;
    int* result;
    int* recv_data;
    int* recv_data_2;
    bool smaller;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(procRank==0){
        numbers(data,countNumbers,countNumInProc);
        cout<<"Массив чисел:";
        for(int i = 0; i < countNumbers; i++) {
            cout<<data[i]<<" ";
        }
        cout<<endl;
    }
    
    MPI_Bcast(&countNumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&countNumInProc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    recv_data = new int[countNumInProc];
    recv_data_2 = new int[countNumInProc];
    result = new int[countNumbers];
    
    MPI_Scatter(data, countNumInProc, MPI_INT, recv_data, countNumInProc, MPI_INT, 0, MPI_COMM_WORLD);
    
    predSort(recv_data, countNumInProc);
    
    for(int iter=0; iter < countNumbers; iter++){
        if(iter%2==0){
            if(procRank%2==0){
                MPI_Sendrecv(recv_data, countNumInProc, MPI_INT, procRank+1, 99,recv_data_2, countNumInProc, MPI_INT, procRank+1, 99, MPI_COMM_WORLD, &status);
                smaller=true;
                sort(recv_data,recv_data_2, countNumInProc, smaller);
            }
            else if(procRank%2!=0){
                MPI_Sendrecv(recv_data, countNumInProc, MPI_INT, procRank-1, 99,recv_data_2, countNumInProc, MPI_INT, procRank-1, 99, MPI_COMM_WORLD, &status);
                smaller=false;
                sort(recv_data,recv_data_2, countNumInProc,smaller);
            }
         }
        else{
            if(procRank%2==1 && procRank!=(procNum-1)){
                MPI_Sendrecv(recv_data, countNumInProc, MPI_INT, procRank+1, 99,recv_data_2, countNumInProc, MPI_INT, procRank+1, 99, MPI_COMM_WORLD, &status);
                smaller=true;
                sort(recv_data,recv_data_2, countNumInProc,smaller);
            }
            else if(procRank%2!=1 && procRank!=0){
                MPI_Sendrecv(recv_data, countNumInProc, MPI_INT, procRank-1, 99,recv_data_2, countNumInProc, MPI_INT, procRank-1, 99, MPI_COMM_WORLD, &status);
                smaller=false;
                sort(recv_data,recv_data_2, countNumInProc,smaller);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(recv_data, countNumInProc, MPI_INT, result, countNumInProc, MPI_INT, 0, MPI_COMM_WORLD);
    if(procRank==0){
        stringstream ss2;
        ss2<<endl<<"RESULT:";
        for(int i=0;i<countNumbers;i++){
            ss2<<" "<<result[i];
        }
        cout<<ss2.str()<<endl;
        
    }
    
    /*cout<<endl<<"procRank: "<<procRank;
    cout<<" countNumbers:"<<countNumbers<<" countNumInProc:"<<countNumInProc;
    cout<<" РњР°СЃСЃРёРІ С‡РёСЃРµР»: ";
    for(int i = 0; i < countNumInProc; i++) {
        cout<<recv_data[i]<<" ";
    }*/
    
    MPI_Barrier(MPI_COMM_WORLD);
    if(procRank==0){
        cout << "runtime = " << clock()/1000.0 << endl; // время работы программы
    }
    MPI_Finalize();
    return 0;
}
