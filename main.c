#include "stdio.h"
#include "stdlib.h"

void swap(int* array,int index1,int index2)
{
  array[index1] = array[index1] + array[index2];
  array[index2] = array[index1] - array[index2];
  array[index1] = array[index1] - array[index2];
}

void compareAndSwap(int* array, int index1)
{
  int index2 = index1 + 1;
  if(array[index1] > array[index2]) swap(array,index1,index2);
}

void lineSwap(int* array, int mode, int length)
{
    int start;
    for(start = mode;start<length-1;start = start + 2)
    {
        compareAndSwap(array,start);
    }
}

int main(int argc, char** argv) {
  int* inputs;
  int i,n,iter;
  n = argc-1;
  inputs = (int*)malloc(n*sizeof(int));
  for(i = 1; i < argc; i++)
  {

    inputs[i-1] = atoi(argv[i]);
  }
  
  for(iter = 0; iter < n; iter++)
  {
    lineSwap(inputs,iter%2,n);  
  }
  
  for(i = 0; i<n;i++) printf("%d ",inputs[i]);
  free(inputs);
  return 0;
}
