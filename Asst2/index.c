#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CHARS 50

typedef struct TempTokenList
{
	char* dtoken;
	int tokenC;
    	struct TempTokenList* nextToken;
    	struct TempTokenList* prevToken;
} DocTokenList, *DocTokenPtr;

typedef struct TokenList
{
    	char* _docName;
    	int _tokenC;
	struct TokenList* _next;
    	struct TokenList* _prev;
} TokenList, *TokenPtr;

typedef struct IndexList
{
    	char *_token;    
    	TokenPtr _tokenList;    // The list of document name and token count in doc.
    	struct IndexList* _next;
    	struct IndexList* _prev;
} IndexList, *IndexPtr;

// Global list for the index.
IndexPtr _gIndex = NULL;

int IsAlpha(char ch)
{
    	if ((ch >= 'a' && ch <= 'z' ) || (ch >= 'A' && ch <= 'Z' )) return 1;
    		return 0;
}

int IsAlphaNum(char ch)
{
    	if ((ch >= 'a' && ch <= 'z' ) || (ch >= 'A' && ch <= 'Z' ) || (ch >= '0' && ch <= '9' )) return 1;
    		return 0;
}

void ConvertToLower(char* token)
{
    	int i = 0;
    	/* convert to lower case */
    	for (i = 0; i < strlen(token); i++)
    	{
        	char ch = token[i];
        	if (ch >= 'A' && ch <= 'Z')
            		token[i] = ch + ('a' - 'A');
    	}    
}

void PrintDocList(DocTokenPtr docList)
{
    	DocTokenPtr tN = docList;
    	if (tN == NULL) return;
    	while (tN != NULL)
    	{
        	printf("%s occurred %d times\n", tN->dtoken, tN->tokenC);
        	tN = tN->nextToken;
    	}    
}
/*Write the file name the XML format*/
void OutputToXML(const char* xmlFile)
{
    	if (xmlFile == NULL) return;
    	FILE* fp = fopen(xmlFile, "w");    
    	if (fp == NULL) return;
    	IndexPtr tN = _gIndex;
    	if (tN == NULL) return;
    
    	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", fp);
    	fputs("<fileIndex>", fp);
    	while (tN != NULL)
    	{
       		TokenPtr tokenList = tN->_tokenList;
       		fprintf(fp, "\n\t<word text=\"%s\">", tN->_token);
       		while(tokenList != NULL)
       		{
            		fprintf(fp, "\n\t\t<file name=\"%s\">%d</file>", tokenList->_docName, tokenList->_tokenC);
            		tokenList = tokenList->_next;            
       		}
       		fprintf(fp, "\n\t</word>");
       		tN = tN->_next;
    	}
    	fputs("\n</fileIndex>", fp);
    	fclose(fp);
}
/*Prints the the XML output*/
void ProduceIndex()
{
    	IndexPtr tN = _gIndex;
    	if (tN == NULL) return;
    	printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    	printf("<fileIndex>");
    	while (tN != NULL)
    	{
       		TokenPtr tokenList = tN->_tokenList;
       		printf("\n\t<word text=\"%s\">", tN->_token);
       		while(tokenList != NULL)
       		{
            		printf("\n\t\t<file name=\"%s\">%d</file>", tokenList->_docName, tokenList->_tokenC);
            		tokenList = tokenList->_next;            
       		}
       		printf("\n\t</word>");
       		tN = tN->_next;
    	}
    	printf("\n</fileIndex>\n");
}
/*
 *Frees the allocated space for the Document list
 */
void ReleaseDocList(DocTokenPtr docList)
{
    	DocTokenPtr tN = docList;
    	if (tN == NULL) return;
    	while (tN != NULL)
    	{
        	DocTokenPtr temp = tN;
        	tN = tN->nextToken;
        	if (temp->dtoken != NULL) free (temp->dtoken);
        	free(temp);
    	}	    
}
/*
 * Iterates through the list of docfiles that a token occurs in
 * Increases the count if the doc already exists under the token
 */
void AddDocToTokenList(TokenPtr* tokenListHead, char* fileName, int tC)
{
	TokenPtr tN = *tokenListHead;
    	TokenPtr pN = NULL;
    	TokenPtr nN = NULL;

	nN = (TokenPtr)malloc(sizeof(TokenList));
    	nN->_docName = (char*)malloc(strlen(fileName) + 1);
    	strcpy(nN->_docName, fileName);
    	nN->_docName[strlen(fileName)] = 0;
    	nN->_tokenC = tC;;
    	nN->_next = NULL;
    	nN->_prev = NULL;

	while(tN != NULL)
    	{
		int cR = strcmp(tN->_docName, fileName);
        	if (cR < 0){
            		pN = tN;
            		tN = tN->_next;
        	}
        	else if (cR == 0)
        	{
			tN->_tokenC += tC;
			return;
		/*
            		if (tN->_tokenC < tC)
            		{
                		pN = tN;
               			tN = tN->_next;
            		}
            		else
            		{
                		break;
            		}
		*/
        	}
        	else break;
    	}
/*    
     	nN = (TokenPtr)malloc(sizeof(TokenList));
	nN->_docName = (char*)malloc(strlen(fileName) + 1);
    	strcpy(nN->_docName, fileName);
    	nN->_docName[strlen(fileName)] = 0;
    	nN->_tokenC = tC;;
    	nN->_next = NULL;
    	nN->_prev = NULL;
*/    	
	if (tN != NULL)
    	{
        	nN->_prev = tN->_prev;
        	nN->_next = tN;
        	if(tN->_prev != NULL) tN->_prev->_next = nN;
        	else *tokenListHead = nN;
        	tN->_prev = nN;
    	}
    	else
    	{
        	if (*tokenListHead == NULL) *tokenListHead = nN;
        	else
        	{
            		pN->_next = nN;
           		nN->_prev = pN;
        	}
    	}    
}

void UpdateIndex(DocTokenPtr docList, char* fileName)
{
    	DocTokenPtr tokenP = docList;
    	if (tokenP == NULL) return;
    	while (tokenP != NULL)
    	{
        	char* token = tokenP->dtoken;
        	IndexPtr tN = _gIndex;
        	IndexPtr pN = NULL;
        	IndexPtr nN = NULL;
        	int tokenFound = 0;
        
        	while(tN != NULL)
        	{
            		int cR = strcmp(tN->_token, token);
            		if (cR < 0){
                		pN = tN;
                		tN = tN->_next;
            		}
            		else if (cR == 0)
            		{
                		tokenFound = 1;
                		break;
            		}
            		else break;
        	}
        	if (tokenFound == 0)
        	{
            		nN = (IndexPtr)malloc(sizeof(IndexList));
            		nN->_token = (char*)malloc(strlen(token) + 1);
            		strcpy(nN->_token, token);
            		nN->_token[strlen(token)] = 0;
            		nN->_tokenList = NULL;
            		nN->_next = NULL;
            		nN->_prev = NULL;
            		AddDocToTokenList(&(nN->_tokenList), fileName, tokenP->tokenC);
            		if (tN != NULL)
            		{
                		nN->_prev = tN->_prev;
                		nN->_next = tN;
                		if(tN->_prev != NULL)
				{
					tN->_prev->_next = nN;
				} 
                		else
				{
					_gIndex = nN;
				} 
                		tN->_prev = nN;
            		}
            		else
            		{
                	if (_gIndex == NULL)
			{
				_gIndex = nN;
			} 
                	else
                	{
                    		pN->_next = nN;
                    		nN->_prev = pN;
                	}
            	}
        }
        else
        {
		AddDocToTokenList(&(tN->_tokenList), fileName, tokenP->tokenC);
        }    
        
        tokenP = tokenP->nextToken;
    }    
}

void AddTokenToDoc(DocTokenPtr* docListHead, char* token)
{
    DocTokenPtr tN = *docListHead;
    DocTokenPtr pN = NULL;
    DocTokenPtr nN = NULL;
    int tokenFound = 0;
    ConvertToLower(token);
    
    while(tN != NULL)
    {
        char* nT = tN->dtoken;
        int cR = strcmp(nT, token);
        if (cR < 0){
            pN = tN;
            tN = tN->nextToken;
        }
        else if (cR == 0)
        {
            tokenFound = 1;
            break;
        }
        else break;
    }
    if (tokenFound == 0)
    {
        nN = (DocTokenPtr)malloc(sizeof(DocTokenList));
        nN->dtoken = (char*)malloc(strlen(token) + 1);
        strcpy(nN->dtoken, token);
        nN->dtoken[strlen(token)] = 0;
        nN->tokenC = 1;
        nN->nextToken = NULL;
        nN->prevToken = NULL;
        if (tN != NULL)
        {
            nN->prevToken = tN->prevToken;
            nN->nextToken = tN;
            if(tN->prevToken != NULL) tN->prevToken->nextToken = nN;
            else *docListHead = nN;
            tN->prevToken = nN;
        }
        else
        {
            if (*docListHead == NULL) *docListHead = nN;
            else
            {
                pN->nextToken = nN;
                nN->prevToken = pN;
            }
        }
    }
    else
    {
        tN->tokenC = tN->tokenC + 1;
    }    
}

void ReadFile(const char* fileName)
{
    	FILE *file;
    	DocTokenPtr docList = NULL;;
    	char ch;
    	char word[MAX_CHARS];
    	int wordLength = 0;
    	int isTokenStart = 0;
    	file = fopen(fileName, "r");
    	if (file == NULL) return;
    	do 
    	{
        	ch = (char)fgetc(file);
        	if (isTokenStart == 0)
        	{
            		if (IsAlpha(ch) == 1)
            		{
                		strcpy(word, "");
                		wordLength = 0;
                		isTokenStart = 1;

               			 word[wordLength++] = ch;
            		}
        	}
        	else
        	{ /* token is in building */
            		if (IsAlphaNum(ch) == 1)
            		{
                	/* keep appending to the current token.*/
                		word[wordLength++] = ch;
            		}
            		else
            		{
                	/* End this token */
                		isTokenStart = 0;
                		word[wordLength] = 0;
                		AddTokenToDoc(&docList, word);
            		}
        	}
    	} while(ch != EOF);    
    	fclose(file);

    	//PrintDocList(docList);
    	char *fn = (char*)malloc(strlen(fileName) + 1);
    	char *ptrfn = fn;
    	strcpy(fn,fileName);
    	if (fn[(strlen(fn) - 1)] == '/')
    		fn[(strlen(fn) - 1)] = '\0';
    	(ptrfn = strrchr(fn, '/')) ? ++ptrfn : ptrfn;
    	ConvertToLower(ptrfn);
    	UpdateIndex(docList, ptrfn);
    	//printf("Update Index done for %s\n", ptrfn);
    	ReleaseDocList(docList);
    	free(fn);    
}
void TraverseFiles(const char *name, int indent)
{
    	DIR *dir;
    	struct dirent *entry;

    	if (!(dir = opendir(name)))
    	{
    	/* it should not reach this code.*/
        	ReadFile(name);
        	return;
    	}
 
    	while ((entry = readdir(dir)) != NULL) {
        	char path[1024];
        	snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
        	if (entry->d_type == DT_DIR) {
            		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                		continue;
            		//printf("%*s[%s]\n", indent, "", entry->d_name);
            		TraverseFiles(path, indent + 2);
        	}
        	else {
            		//printf("%*s- %s\n", indent, "", path);
            		//printf("%s finished\n",path);
			ReadFile(path);            
        	}
    	}
    	closedir(dir);
}
void ReleaseIndex()
{
    	IndexPtr iP = _gIndex;
    	if (iP == NULL) return;
    	while(iP != NULL)
    	{
        	IndexPtr temp = iP;
        	iP = iP->_next;
        	if (temp->_token != NULL) free (temp->_token);
        	if (temp->_tokenList != NULL)
        	{
            		TokenPtr tP = temp->_tokenList;
            		while(tP != NULL)
            		{
               			TokenPtr temp1 = tP;
               			tP = tP->_next;
               			if (temp1->_docName != NULL) free(temp1->_docName);
               			free (temp1);
           		}                
        	}
		free (temp);        
    	}    
}

int main(int argc, char** argv) 
{
    
	char *startPath = NULL;
	char* xmlOutPath = NULL;
    	if (argc != 3 || argv[1] == NULL || argv[2] == NULL) return 0;
    	startPath = argv[2];
    	xmlOutPath = argv[1];

    	DIR *dir;
    	if (!(dir = opendir(startPath)))
    	{
        	ReadFile(startPath);
    	}
    	else
    	{
		//printf("past ReadFile\n");
        	closedir(dir);
        	TraverseFiles(startPath, 0);
    	}
    
    	OutputToXML(xmlOutPath);
    	//ProduceIndex();
    
    	ReleaseIndex();
   
    	return (EXIT_SUCCESS);
}

