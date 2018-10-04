#include<stdio.h>
#include <stddef.h>

typedef struct Node
{
		        size_t size;

        int ifFree;

};
static char myBlock[5000];


void * mymalloc(size_t size, char *FILE, int LINE); 
void myfree(void*toFree, char *FILE, int LINE);
void * merge();
void printMemory();
#define malloc( x ) mymalloc( x, __FILE__, __LINE__ )

#define free( x ) myfree( x, __FILE__, __LINE__ )

