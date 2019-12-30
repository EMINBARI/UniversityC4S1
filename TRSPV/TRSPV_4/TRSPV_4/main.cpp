#include <iostream>
#include <ctime>
#include <mpi.h>

using namespace std;

enum {REQUEST = 1, RELEASE = 2, CONNECT = 3, CONFIRM = 4, DISCONNECT = 5, BUSY = 6, CLIENTS = 7, FREE = 8};
const int SERVER = 0;
const int RESOURCE = 1;
int procNum;
int procRank;
int clients = 0;
MPI_Status status;
MPI_Comm comm;

void server();
void client();
void resource();
void getResurce(int resource);

int main(int argc, char **argv) {
    
    MPI_Init(&argc, &argv);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &procNum);
    MPI_Comm_rank(comm, &procRank);

    if(procRank == SERVER) server();
    else if(procRank == RESOURCE) resource();
    else client();
    
    MPI_Barrier(comm);
    MPI_Finalize();
    return 0;
}

void resource(){
    int resources[] = {10, 2, 5};
    int resourcesInUse[] = {0, 0, 0};
    int infoBuf;
    int numberResource;
    int bufForResource;
    int fail_buffer = -1;
    
    do{
        MPI_Recv(&infoBuf, 1, MPI_INT, SERVER, MPI_ANY_TAG, comm, &status);
        switch(status.MPI_TAG){
            case CLIENTS:
                clients = infoBuf;
                break;
                
            case REQUEST :
                numberResource = infoBuf;
                cout<<"Запрос сервера на ресурс "<<numberResource<<";"<<endl;;
                if(resourcesInUse[numberResource] != 0){
                    MPI_Send(&fail_buffer, 1, MPI_INT, SERVER, BUSY,  comm);
                    break;
                }
                resourcesInUse[numberResource] = 1;
                MPI_Send(&resources[numberResource], 1, MPI_INT, SERVER, CONFIRM,  comm);
                break;
                
            case RELEASE :
                 numberResource = infoBuf;
                 cout<<"Запрос сервера на особождение ресурса "<<numberResource<<";";
                 resourcesInUse[numberResource] = 0;
                 cout<<"Ресурс "<<numberResource<<" освобожден;"<<endl;
                 MPI_Send(&fail_buffer, 1, MPI_INT, SERVER, FREE,  comm);
                 break;
        }
        
    }while(clients);
}

void server(){
    int numberResource;
    int queueProc=-1, queueResource=-1;
    int bufForResource;
    int clientSource;
    
    do{
        MPI_Recv(&numberResource, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &status);
        clientSource = status.MPI_SOURCE;
        
        switch(status.MPI_TAG){
            case CONNECT :
                 cout<<"Клиент"<<clientSource - 1<<" в системе;"<<endl;
                 clients++;
                 MPI_Send(&clients, 1, MPI_INT, RESOURCE, CLIENTS, comm);
                 break;
                
            case DISCONNECT :
                 cout<<"Клиент"<<clientSource - 1<<" вышел из системы;"<<endl;
                 clients--;
                 MPI_Send(&clients, 1, MPI_INT, RESOURCE, CLIENTS, comm);
                 break;
                
            case REQUEST :
                cout<<"Клиент"<<clientSource - 1<<" запрашивает ресурс "<<numberResource<<";"<<endl;
                MPI_Send(&numberResource, 1, MPI_INT, RESOURCE, REQUEST, comm);
                MPI_Recv(&bufForResource, 1, MPI_INT, RESOURCE, MPI_ANY_TAG, comm, &status);
                
                if(status.MPI_TAG == BUSY){
                     queueProc = clientSource;
                     queueResource = numberResource;
                     cout<<"Для Клиента"<<clientSource - 1<<" ресурс "<<numberResource<<" занят;"<<endl;
                     break;
                 }
                
                 MPI_Send(&bufForResource, 1, MPI_INT, clientSource, CONFIRM,  comm);
                 break;
                
            case RELEASE :
                 cout<<"Клиент"<<clientSource-1<<" освободил ресурс "<<numberResource<<";"<<endl;
                 MPI_Send(&numberResource, 1, MPI_INT, RESOURCE, RELEASE, comm);
                 MPI_Recv(&bufForResource, 1, MPI_INT, RESOURCE, FREE, comm, MPI_STATUS_IGNORE);
                
                 if(queueResource == numberResource){
                     MPI_Send(&numberResource, 1, MPI_INT, RESOURCE, REQUEST, comm);
                     MPI_Recv(&bufForResource, 1, MPI_INT, RESOURCE, MPI_ANY_TAG, comm, &status);
                     MPI_Send(&bufForResource, 1, MPI_INT, queueProc, CONFIRM,  comm);
                     queueProc=-1;
                     queueResource=-1;
                     break;
                 }
                
                 break;
        }
    }while(clients);
    
}

void getResurce(int resource){
    int bufForResource;
    MPI_Send(&resource, 1, MPI_INT, SERVER, REQUEST, comm);
    MPI_Recv(&bufForResource, 1, MPI_INT, SERVER, CONFIRM, comm, &status);
    cout<<"Клиент"<<procRank-1<<"получил ресурс "<<bufForResource<<";"<<endl;
    MPI_Send(&resource, 1, MPI_INT, SERVER, RELEASE, comm);
}

void client(){
    
    int numberResurce;
    srand(10);
    if (procRank == 1) srand(10);
    else srand(11);
    numberResurce = rand() % 3;
    MPI_Send(&numberResurce, 1, MPI_INT, SERVER, CONNECT, comm);
    for(int i = 0; i < 4; i++){
        numberResurce = rand() % 3;
        getResurce(numberResurce);
    }
    MPI_Send(&numberResurce, 1, MPI_INT, SERVER, DISCONNECT, comm);
}

