
#include <mpi.h> 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Lab4_IO.h"

// Structure to store the node info
struct node{
    int *inlinks;
    int num_in_links;
    int num_out_links;
};

//global values
#define e = 0.00001;
#define d = 0.85; 
int main(int argc, char* argv[]){

    int numOfThreads;
    int rank; 
    int rowsPerThread;
    int size; 
    double sTime, eTime; 
    double* rank_list;
    FILE *fp; 
    //init MPI and get the process information 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfThreads);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    struct node **nodeHead; 
   
    //master process will call the node_init function provided
    //process very similar to init_node function provided in Lab4_IO.h
    if (rank == 0) {
        //open the data_input_meta file to get the amount of nodes generated.
        //saved in size
        if ((fp = fopen("data_input_meta","r")) == NULL){
            printf("Fail to open file 'data_input_meta'. \n");
            exit(1);
        }
        //getting amount of nodes stored in size
        fscanf(fp,"%d\n",&size);
        //allocate size amount of spaces to store the node in our graph
        nodeHead = malloc(size * sizeof(struct node));
        //allocating space for size length array to store count of nodes.
        int nodeID, num_in, num_out;
        for ( int i = 0; i < size; ++i){
            fscanf(fp, "%d\t%d\t%d\n", &nodeID, &num_in, &num_out);
            if (nodeID != i){
                printf("Error loading meta data, node id inconsistent!\n");
                return -2;
            }
            (nodeHead)[i]->num_in_links = num_in;
            (nodeHead)[i]->num_out_links = num_out;
            (nodeHead)[i]->inlinks = malloc(num_in * sizeof(int));
        }
        fclose(fp);
     
    }
    


}