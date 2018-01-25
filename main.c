#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"
void swap(int* array,int index1,int index2)
{
  array[index1] = array[index1] + array[index2];
  array[index2] = array[index1] - array[index2];
  array[index1] = array[index1] - array[index2];
}

int compareAndSwap(int* array, int index1)
{
  int index2 = index1 + 1;
  if(array[index1] > array[index2]) {
 swap(array,index1,index2);
 return 1;
}
return 0;
}

int lineSwap(int* array, int mode, int length,int threads,int rank, int* sizes, int* displs, double* time)
{
    int* rbuf;
    int sum[1],trueSum[1];
    int start,bufSize,i;
    double startTime, result;

    startTime = MPI_Wtime();

    if(sizes[0] > sizes[threads-1]) bufSize = sizes[0];

    else bufSize = sizes[threads-1];

    rbuf = malloc(sizeof(int)*sizes[rank]);

    sum[0] = 0;

MPI_Type_size(MPI_INT,&i);



    MPI_Scatterv(array, sizes, displs ,MPI_INT, rbuf, sizes[rank], MPI_INT, 0, MPI_COMM_WORLD);


    for(start = (mode+displs[rank])%2;start<sizes[rank]-1;start = start + 2)
    {
	sum[0] += compareAndSwap(rbuf,start);
    }

    MPI_Gatherv(rbuf, sizes[rank], MPI_INT, array, sizes, displs, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(sum,trueSum, 1 ,MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    result = MPI_Wtime() - startTime;
    *time = result;
    free(rbuf);
    return trueSum[0];
}

int verify_sorted(int* array, int length) {
  int i;
  for(i = 0;i < length-1; i++) {
	if(array[i]>array[i+1]) return -(i+1);
 
}
return 0;
}

void getSizes(int* sizes, int* displs, int arrayLength, int processorCount)
{
 int i,sum;
 //we assume that sizes is alloced
 int count = arrayLength/(2*processorCount);
 count = count * 2;
 sum = 0;
 for(i = 0;i < processorCount-1; i++)
 {
   displs[i] = sum ;
   sum = sum + count;
   sizes[i] = count;

 }
 displs[processorCount - 1] = sum ;
 sizes[processorCount - 1] = arrayLength - sum;
   
}

void setSizes(int* sizes, int* displs, int length, int iter)
{
int i = 0;
int total = 0;
int sign = -(2 * (iter%2) - 1);
 for(;i<length-1;i++)
{
 if(i==0) sizes[i] = sizes[i] - sign;
 
 if(i > 0) displs[i] = displs[i] + sign;


}
sizes[length-1] = sizes[length-1] - sign;
displs[length-1] = displs[length-1] +  sign;
}

int main(int argc, char** argv) {
  int* inputs;
  int* sizes;
  int* displs;
  int i,n,iter,threads,result,quiet,checker, rank;
  FILE *f1;
  double totalTime = 0, time;
  char line[1024];

  n = 0;

  MPI_Init(&argc,&argv);
  quiet = argc > 2;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  if(rank == 0)
{

  f1 = fopen(argv[1], "r");

  while(fgets(line,sizeof(line),f1) != NULL) 
  {
 
    n++;
  }
  
  fseek(f1,0,SEEK_SET);

inputs = (int*)malloc(n*sizeof(int));

  i=0;
  while(fgets(line,sizeof(line),f1) != NULL) 
  {
    inputs[i] = atoi(line);
    i++;
  }

  }

  MPI_Comm_size(MPI_COMM_WORLD,&threads);
MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);

  sizes = (int*)malloc(sizeof(int)*threads);
  displs = (int*)malloc(sizeof(int)*threads);

  getSizes(sizes,displs, n, threads);


  for(iter = 1; iter < n+1; iter++)
  {
    MPI_Barrier(MPI_COMM_WORLD);
    setSizes(sizes,displs,threads,iter);
    checker = lineSwap(inputs,iter,n,threads, rank, sizes, displs ,&time);  
    MPI_Barrier(MPI_COMM_WORLD);

  MPI_Bcast(&checker,1,MPI_INT,0,MPI_COMM_WORLD);
   
    totalTime = totalTime + time;
    if(checker == 0) break;
    
  }
  if(rank == 0)
{
  result = verify_sorted(inputs,n);
  printf("result %d\n",result);
  if(!quiet) for(i = 0; i<n;i++) printf("%d ",inputs[i]); 
  printf("\ntime: %f",totalTime);
  free(inputs);
  free(sizes);
  free(displs);
  fclose(f1);
}
MPI_Finalize();	
if(rank > 0) return 0;
  return result;
}
