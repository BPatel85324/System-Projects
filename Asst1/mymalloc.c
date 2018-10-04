#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
  



//initiates the array which will resemble the memory

struct Node *freeSpace=(void*)myBlock;


void* mymalloc(size_t size,char *FILE, int LINE)
{               

        int emptySize;
        struct Node *search=freeSpace;
        if(!(freeSpace->size))
        {                    
          //initiates the memory
          freeSpace->size=5000-sizeof(struct Node);
          freeSpace->ifFree=1;
        }
        int sizeAvai=search->size;
        int tracker=0;
        //parsing through the memory to see where it is free
        while(tracker<sizeof(myBlock)) {
                tracker=tracker+sizeof(struct Node)+search->size;
                if(search->ifFree==1&&(search->size>=sizeof(struct Node)+size)){//found a spot where it could be allocated.
                    emptySize=search->size-size;
                    search->size=size;
                    search->ifFree=0;
                        struct Node* new=(void*)((void*)search+size+sizeof(struct Node));
                        new->ifFree=1;
                        new->size=emptySize-sizeof(struct Node);
                        return (void*)search;
                        
                  
                }
                search=((void*)search+search->size+sizeof(struct Node));


        }
        printf("error not enough space File: %s Line: %d\n",FILE,LINE);

}
void * merge() 
 {//merges all agacent free nodes. 

  struct Node * first; 
  struct Node * next;
  struct Node * holder; 
  
  first = freeSpace; 

  int num = 0;
  
  while(num < sizeof(myBlock)) 
  {
    if(num == 0) 
    { 

        next = ((void*)first+first->size+sizeof(struct Node));
         
        num=sizeof(struct Node) + first->size + sizeof(struct Node) + next->size;
    }
    else
    {
 
    }
    //fouund agacent nodes when num doesnt add up to total size.
    if((first->ifFree==1 && next->ifFree==1)&& num!=(sizeof(myBlock))) 
    {
      first->size = first-> size + next->size + sizeof(struct Node);
      
      holder = ((void*)next+next->size+sizeof(struct Node));
      next=NULL;
      next = holder; 
      
      if(num==sizeof(myBlock))
      {
        next=NULL;
      }
            

    }
    

        
       if(num<5000){ 
        num = num + sizeof(struct Node) + next->size;
      }

      //when added up to total size merge agacent nodes.

        if((first->ifFree==1 && next->ifFree==1) && num==(sizeof(myBlock)))
        {

        first->size = first-> size + next->size + sizeof(struct Node);
        next=NULL;
        next=holder;
        }
        
        if(num!=sizeof(myBlock))
        {
           first = ((void*)first+first->size+sizeof(struct Node));
        }
        if(num<sizeof(myBlock))
        {
            next = ((void*)next+next->size+sizeof(struct Node));
        }
        
        if(first->size==0){
          return;
        }

 }

}
  


void myfree(void*toFree, char *FILE, int LINE){ 

    if(toFree==NULL){
            printf("not a pointer. File: %s Line: %d\n",FILE,LINE);
            return;

    }
    if(toFree>=(void*)myBlock&&(toFree<=(void*)(myBlock+5000))){
                struct Node *ptr=toFree;//puts the pointer as a node so we can easily free it.
                if(ptr->size==0){
                  printf("pointer that was inputed was not allocated by malloc. File: %s Line: %d\n",FILE,LINE);
                  return;

                }
                if(ptr->ifFree==0){
                        ptr->ifFree=1;
                       merge();
                }else{
                    printf("redudent freeing. File: %s Line: %d\n",FILE,LINE);
                }
    

    }else{
      printf("error not a pointer that was malloced.File: %s Line: %d\nn",FILE,LINE);



    }

}



void printMemory(){
       printf("printing memory\n");
        int tracker=0;
        int i=0;
        struct Node *search=freeSpace;
        while(tracker<sizeof(myBlock)){
                printf("node size:%d node ifFree:%d %d\n",(int)search->size,(int)search->ifFree,i);

              tracker=tracker+sizeof(struct Node)+search->size;
              search=((void*)search+search->size+sizeof(struct Node));; 
              i++;


        }


}
