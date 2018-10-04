#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mymalloc.h"



int main(int argc,char** arg){

//finding time for a-f


    struct timeval begin, end;
gettimeofday(&begin, NULL);
    int i;
    i=0;
    while(i<100){
            a();

        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for A in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 
gettimeofday(&begin, NULL);
    i=0;
    while(i<100){
            b();

        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for B in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 
gettimeofday(&begin, NULL);
    i=0;
    while(i<100){
            c();

        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for C in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 

gettimeofday(&begin, NULL);
    i=0;
    while(i<100){
            d();
        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for D in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 
    gettimeofday(&begin, NULL);
    i=0;
    while(i<100){
            e();
        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for E in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 
    gettimeofday(&begin, NULL);
    i=0;
    while(i<100){
            f();
        
        i++;
    }

gettimeofday(&end, NULL);
    printf("Time for F in microseconds: %ld microseconds\n",
            ((end.tv_sec - begin.tv_sec)*1000000L
           +end.tv_usec) - begin.tv_usec
          ); 
}

void a(){
  int *allot[150];      
  int i;                            
  for(i=0;i<150;i++)
  {
    allot[i]=(char *)malloc(1);      
    free(allot[i]);                                               
  }



}
void b(){
  int *allot[150];      
  int i;   
  for(i=0;i<150;i++)
  {
    allot[i]=(char *)malloc(1);          
  }
  for(i=0;i<150;i++)
  {
    free(allot[i]);  
  }
}

void c(){

       char *a[150];
       int n[150];
    int rnd=rand() % 2;
    int i=0;
    int k=0;
    int l=0;
    int tracker=0;
    while(i<150){
        if(rnd==0||tracker==0){
            a[i]=(char*)malloc(1);
            n[k]=i;
            k++;
            tracker++;
        }else{
                  free(a[n[l]]);
                  l++;
            tracker--;
        }
        rnd=rand() % 2;
        i++;

    }
    while(tracker>0){
                                             
             free(a[n[l]]);
             
             l++;
        tracker--;
    }





}
void d(){

        char *a[150];
    int n[150];
    int rando=rand() % 2;
    int all=rand()%64+1;
    int i=0;
    int k=0;
    int l=0;
    int tracker=0;
    while(k<150){
        if(rando==1||tracker==0){
            a[k]=(char*)malloc(all);
            all=rand()%64+1;
            n[k]=k;
            k++;
            tracker++;

        }else{


                  free(a[n[l]]);
            l++;
            tracker--;

        }
        rando=rand() % 2;

    }
    while(tracker>0){

             free(a[n[l]]);
             l++;
        tracker--;
    }
    merge();








}
//allocates 1 and 2 bytes every other time and frees them in the same order.
void e(){

  int *allot[150];      
  int i;   
  int j=0;
  for(i=0;i<150;i++)
  {
    if(j==0){
    allot[i]=(char *)malloc(1);          
    j=1;
    }else{
            allot[i]=(char *)malloc(2);  
        j=0;
    }
  }
  for(i=0;i<150;i++)
  {
    free(allot[i]);

  }

}

//Allocates 2 bytes 150 times. This is proceeded by freeing the previous allocation and mallocing one byte at the same time. The end of the method frees the rest of the allocations. 
void f(){
  int *allot[300];      
  int i;   
  for(i=0;i<150;i++)
  {
    allot[i]=(char *)malloc(2);          
  }
  for(i=150;i<300;i++)
  {
    free(allot[i-150]);  
    allot[i]=(char *)malloc(1); 
  }
    for(i=150;i<300;i++)
  {
    free(allot[i]);  
  }
}









 