
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
    double* rankVector; 
    FILE *fp; 
    //init MPI and get the process information 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfThreads);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    struct node *nodeHead; 
   
    //master process create the graph
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
        //going through our "graph" and assigning num_in_links, num_out_links, and allocating space for the inlinks property,
        //so when we access the data_input_link we can fill the array for each node.
        int nodeID, num_in, num_out;
        for ( int i = 0; i < size; i++){
            fscanf(fp, "%d\t%d\t%d\n", &nodeID, &num_in, &num_out);
            if (nodeID != i){
                printf("Error loading meta data, node id inconsistent!\n");
                exit(1);
            }
            nodeHead[i].num_in_links = num_in;
            nodeHead[i].num_out_links = num_out;
            nodeHead[i].inlinks = malloc(num_in * sizeof(int));
        }
        fclose(fp);
        
        //once link file is opened we need to update our inlinks property for each node in our graph.
        if ((fp = fopen("data_input_link","r")) == NULL){
            printf("Fail to open file 'data_input_link'. \n");
            exit(1);
        }
        //again will be similar to init_node fucntion
        int *index;
        int src, dst; 
        index = malloc(size * sizeof(int));
        for(int i=0; i<size; ++i){
            index[i] = 0;
        }
        //until EOF
        while(!feof(fp)){
            fscanf(fp, "%d\t%d\n", &src, &dst);
            if (dst >= 0 && dst < size)
                nodeHead[dst].inlinks[index[dst]++] = src;
        }
        free(index);
        fclose(fp);
        //graph has been created and values are set. 
    }

    //letting all proccesses know what the value of size is. 
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //initilizing the rankVector & the amount of rows a thread will be assigned
    
    rankVector = malloc(size* sizeof(double));
    for (int i =0; i<size; i++){
        rankVector[i] = 1/size;
    }
    rowsPerThread = size / numOfThreads;
    
    // Allocate memory for nodeHead on each process
    struct node *nodeSubHead = malloc(rowsPerThread * sizeof(struct node));

    // Scatter nodeHead to all processes
    MPI_Scatter(nodeHead, rowsPerThread * sizeof(struct node), MPI_CHAR, nodeSubHead, rowsPerThread * sizeof(struct node), MPI_CHAR, 0, MPI_COMM_WORLD);

    // Use nodeSubHead instead of nodeHead from now on
    nodeHead = nodeSubHead;
    //at this point every process had its assigned chunk of the nodeHead.

    



    MPI_Finalize();
    
}