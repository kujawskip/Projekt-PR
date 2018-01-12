#include "stdio.h"
#include "stdlib.h"
#include "omp.h"

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

int lineSwap(int* array, int mode, int length,int threads, double* time)
{
    int start,sum = 0;
    double startTime, result;
    startTime = omp_get_wtime();
    #pragma omp parallel for private(start) shared(mode, length, array) reduction(+: sum) num_threads(threads) schedule(static)
    for(start = mode%2;start<length-1;start = start + 2)
    {
	sum += compareAndSwap(array,start);
    }
    result = omp_get_wtime() - startTime;
    *time = result;
    return sum;
}

int verify_sorted(int* array, int length) {
  int i;
  for(i = 0;i < length-1; i++) {
	if(array[i]>array[i+1]) return -(i+1);
 
}
return 0;
}

int main(int argc, char** argv) {
  int* inputs;
  int i,n,iter,threads,result,quiet,checker;
  FILE *f1;
  double totalTime = 0, time;
  char line[1024];
  quiet = argc > 3;
  n = 0;
  f1 = fopen(argv[2], "r");
 
  while(fgets(line,sizeof(line),f1) != NULL) 
  {
    n++;
  }
  fseek(f1,0,SEEK_SET);
  
  threads = atoi(argv[1]);
  inputs = (int*)malloc(n*sizeof(int));
  i=0;
  while(fgets(line,sizeof(line),f1) != NULL) 
  {
    inputs[i] = atoi(line);
    i++;
  }

  for(iter = 0; iter < n; iter++)
  {
    checker = lineSwap(inputs,iter,n,threads, &time);  
    totalTime = totalTime + time;
    if(checker ==0) break;
  }
  result = verify_sorted(inputs,n);
  if(!quiet) for(i = 0; i<n;i++) printf("%d ",inputs[i]);
  printf("\ntime: %f",totalTime);
  free(inputs);
  fclose(f1);
	
  return result;
}
