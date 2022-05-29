/*
Project : PRL project 1
Student:  Adam Rybanský
Login:    xryban00
*/


#include <iostream>
#include <fstream>
#include <algorithm> 
#include <mpi.h>

#define MASTER 0               //rank of the master processor
#define NUMBERS_FILE "numbers" //name of the input file
#define NUMBERS_COUNT 8        //how many numbers should be on input
#define COMM MPI_COMM_WORLD    //default setting
#define TAG 0                  //default setting

using namespace std;

//from which processor should each processor await input

int adjacency_from[19][2] = 
{
  {MASTER,MASTER},  //0
  {MASTER,MASTER},  //1
  {MASTER,MASTER},  //2
  {MASTER,MASTER},  //3
  {0,1},            //4
  {0,1},            //5
  {2,3},            //6
  {2,3},            //7
  {4,5},            //8
  {6,7},            //9
  {4,6},            //10
  {8,9},            //11
  {8,9},            //12
  {5,7},            //13
  {10,11},          //14
  {12,13},          //15
  {14,12},          //16
  {14,15},          //17
  {11,15}           //18
};

//where should each processor send output

int adjacency_to[19][2] = 
{
  {4,5},            //0
  {4,5},            //1
  {6,7},            //2
  {6,7},            //3
  {10,8},           //4
  {8,13},           //5
  {10,9},           //6
  {9,13},           //7
  {12,11},          //8
  {12,11},          //9
  {MASTER,14},      //10
  {14,18},          //11
  {16,15},          //12
  {15,MASTER},      //13
  {16,17},          //14
  {17,18},          //15
  {MASTER,MASTER},  //16
  {MASTER,MASTER},  //17
  {MASTER,MASTER}   //18
};

//read the input file, print numbers to console, send numbers to the first layer
void master_read_and_send(const int rank)
{
    if (rank == MASTER)
    {
      ifstream numbers(NUMBERS_FILE);
      int new_rank = 0;
      for (int i = 0 ; i < NUMBERS_COUNT; i++){
        const int number = numbers.get();
        cout << number << " ";
        MPI_Send(&number, 1, MPI_INT, new_rank, TAG, COMM);
        if (i % 2 == 1)
          new_rank++;
      }
      cout << endl;
    }
}

//receive 2 numbers and store them to variables
void node_receive(int &first, int &second, const int rank)
{
   int from1 = adjacency_from[rank][0]; 
   int from2 = adjacency_from[rank][1];  
   MPI_Recv(&first, 1, MPI_INT, from1, TAG, COMM, nullptr);
   MPI_Recv(&second, 1, MPI_INT, from2, TAG, COMM, nullptr);
}

//sort the 2 variables and send them to the next layer
void node_send(int &first,int &second, const int rank)
{
    int min = std::min(first,second);
    int max = std::max(first,second);
    int to1 = adjacency_to[rank][0];
    int to2 = adjacency_to[rank][1];
    MPI_Send(&min, 1, MPI_INT, to1, TAG, COMM);
    MPI_Send(&max, 1, MPI_INT, to2, TAG, COMM);
}

//receive sorted numbers and store them to an array
void master_receive(int *const data, const int rank)
{
  	if (rank == MASTER)
  	{
     int i = 0;
     MPI_Recv(&data[i++], 1, MPI_INT, 10, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 16, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 16, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 17, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 17, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 18, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 18, TAG, COMM, nullptr);
     MPI_Recv(&data[i++], 1, MPI_INT, 13, TAG, COMM, nullptr);
  	}
}

//print the array of sorted numbers to console
void master_print(const int *const data, const int rank)
{
    if (rank == MASTER)
      for (int i = 0; i < NUMBERS_COUNT; i++)
    	  cout << data[i] << endl;
}


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
	  int rank, first, second, output[NUMBERS_COUNT];
	  MPI_Comm_rank(COMM, &rank);
    master_read_and_send(rank);
    node_receive(first,second,rank);
    node_send(first,second, rank);
    master_receive(output, rank);
	  master_print(output, rank);
	  MPI_Finalize();
    return 0;
}