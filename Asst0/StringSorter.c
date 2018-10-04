#include<stdio.h>
#include<string.h>
#include<stdlib.h>
typedef struct // defining structure of name nano
{
char *answer;
}nano;

int main(int argc,char ** argv)
{
nano N[10],H; //making variables of structure nano
int i=0,j=0,k=0,u=0; // defining variables used in program
char input[20]; // input string
   char *temp,*ser;
  int l;


if(argc==2)
{

char *p= strtok(argv[1],"1234567890'!:;,.~`|@#$%^&*(_+)-=/{[?<>]} "); // valida$
while (p != NULL) // while loop for validation
    {

    N[i].answer=p; // validated parts stored in structure

        p = strtok(NULL,"1234567890!'@#$%^&*(~`|,.:;_+)-=/{[?<>]} ");

i++;
    }

    for(j;j<(i-1);j++) // sorting the validated parts
    {
        //selection sort
        k=j;
        temp=N[j].answer;
        for(l=(j+1);l<i;l++)
        {

        if(strcmp(N[l].answer,temp)<0)
        {
        temp=N[l].answer;
        k=l;
        }
        }
            N[k].answer=N[j].answer;
            N[j].answer=temp;
        }
        for(u;u<i;u++) // printing sorted values
        {
        printf("%s \n",N[u].answer);
        }
        }

        else
        {
        printf("error text");
        }
        return 0;
        }
