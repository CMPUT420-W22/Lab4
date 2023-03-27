
#include <mpi.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"
#include "timer.h"
#define LAB4_EXTEND
// Structure to store the node info
struct node{
    int *inlinks;
    int num_in_links;
    int num_out_links;
};

// global values defined in lab manual
#define EPSILON 0.00001
#define DAMPING 0.85

double get_relative_error(double *r, double *t, int size){
    // same as rel_error() function defined in Lab4_IO
    // redefined here as rel_error() was causing errors
    int i;
    double norm_diff = 0, norm_vec = 0;
    for (i = 0; i < size; ++i){
        norm_diff += (r[i] - t[i]) * (r[i] - t[i]);
        norm_vec += t[i] * t[i];
    }
    return sqrt(norm_diff/norm_vec);
}

int main(int argc, char* argv[]){
    // initialize variables
    int numProcesses, rank, rowsPerProcess;
    int numNodes, numNodesLocal, localStartNode; 
    double sTime, eTime; 
    double *r_new, *r_old, *r_local;
    FILE *fp; 
    struct node *nodeHead; 

    // init MPI and get the process information 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    
    //getting amount of nodes stored in size
    if ((fp = fopen("data_input_meta","r")) == NULL){
        printf("Fail to open file 'data_input_meta'. \n");
        exit(1);
    }
    fscanf(fp,"%d\n",&numNodes);

    // use node_init function to initialize array of nodes
    node_init(&nodeHead,0,numNodes);

    // set up number of nodes per process and the starting node for each process
    numNodesLocal = numNodes / numProcesses;
    localStartNode = rank * numNodesLocal;

    // allocate space for rank vectors
    // r_new can be considered r(t+1)
    // r_old can be considered previous value of r
    // r_local is each process' rank vector to work on
    r_new = malloc(numNodes * sizeof(double));
    r_old = malloc(numNodes * sizeof(double));
    r_local = malloc(numNodesLocal * sizeof(double));

    // to optimize performance we only need the master process to initialize the rank vector
    if (rank == 0) {
        for (int i = 0; i < numNodes; ++i){
            // initially each node has probability value 1/N
            r_new[i] = 1.0 / numNodes;
        }

    }

    // start timer
    if (rank == 0){
        GET_TIME(sTime);
    }


    // we continue to compute r until we reach threshold
    do {

        // we only need the master process to update the rank vector
        // so now we have 2 copies of rank vector, one previous and one new 
        if (rank == 0){
            // copy r_new into r_old
            vec_cp(r_new, r_old, numNodes);
        }

        // broadcast the rank vector to all other processes
        MPI_Bcast(r_old, numNodes, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        //MPI_Scatter(r_old, numNodesLocal, MPI_DOUBLE,
        //            r_local, numNodesLocal, MPI_DOUBLE,
        //           0, MPI_COMM_WORLD);

        
        // loop over # of nodes local to each processor
        for (int i = 0; i < numNodesLocal; ++i){
            r_local[i] = 0;

            // Sum over set of nodes with an outgoing link to node i (which is incoming links to node i)
            for (int j = 0; j < nodeHead[i+localStartNode].num_in_links;++j){
                // Perform calculation from lab manual
                // r_j(t) / l_j
                r_local[i] += r_old[nodeHead[i+localStartNode].inlinks[j]] / nodeHead[nodeHead[i+localStartNode].inlinks[j]].num_out_links;
            }

            // Account for damping factors
            r_local[i] *= DAMPING;
            r_local[i] += ((1-DAMPING) / numNodes);
            // by the end of this, r_local[i] = (1-d)*(1/N) + d * sum(r_j(t) / l_j) as per lab manual

        }

        // Combine local rank vectors into r_new
        MPI_Allgather(r_local, numNodesLocal, MPI_DOUBLE,
                        r_new, numNodesLocal, MPI_DOUBLE,
                        MPI_COMM_WORLD);
        

    } while (get_relative_error(r_new,r_old,numNodes) >= EPSILON);


    if (rank == 0){
        // we only need one process to save the time and save to output
        GET_TIME(eTime);
        Lab4_saveoutput(r_new,numNodes, eTime-sTime);
    }
    
    // clean up
    MPI_Finalize();
    node_destroy(nodeHead);
    free(r_new); free(r_old); free(r_local);

    return 0;
    
}
