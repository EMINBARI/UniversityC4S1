//
//  Graph.cpp
//  TRSPV_3
//
//  Created by Emin Bari on 02.11.2019.
//  Copyright © 2019 Emin Bari. All rights reserved.
//
#include <mpi.h>
#include <stdio.h>


class Graph{
    
public:
    MPI_Comm graph;
    int processRank;
    int processCount;
    int *index;
    int *edges;
    
    Graph(MPI_Comm, int procRank, int procCount){
        
        index = new int[processCount];
        edges = new int[processCount];

        for (int i = 1; i <= processCount; i++) {
           index[i - 1] = i;
           edges[i - 1] = i % processCount;
        }

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Graph_create(MPI_COMM_WORLD, this.processCount, index, edges, 0, &graph);
        MPI_Comm_size(graph, &processCount);

        MPI_Comm_rank(graph, &processRank);

        delete[] index;
        delete[] edges;
    }
    
    void createGrapthTopology(){
        
    }
    
    
    
    
}
