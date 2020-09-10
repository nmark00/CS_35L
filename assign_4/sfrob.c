#include <stdio.h>
#include<stdlib.h>
#define CSIZE sizeof(char*)


int frobcmp(char const *a, char const *b)
{
	// a and b point to a cstring
	while (*a != ' ' || *b!= ' ') //iterate through word
	{
		while(*a == '\0')//ignore null 
			a++;
		while(*b == '\0')
			b++;
		if(*a == ' '&& *b == ' ')//equal
			break;
		if((*a^42) < (*b^42) || *a == ' ')//a ends first or < b 
			return -1;
		if((*a^42) > (*b^42) || *b == ' ')//b ends first or < a
			return 1;
		
		a++; b++;
	}
	return 0;
}
void errorIn() //checks stdin errors
{
	if(ferror(stdin))
	{
		fprintf(stderr, "Error Reading Standard Input\n");
		exit(1);
	}
}
void errorOut(char out) //checks putchar errors
{
	if (out == EOF)
	{
		fprintf(stderr, "Error With Standard Output\n");
		exit(1);
	}
}
void errorAlloc(void* ptr) //checks error with malloc/realloc
{
	if (ptr == NULL)
	{
		free(ptr);
		fprintf(stderr, "Error with memory allocation\n");
		exit(1);
	}
}
//Used for qsort compar function
int compar(const void* a, const void* b)
{
	return frobcmp( *(char**) a, *(char**) b);
}


int main()
{
	//initialize variables
	char c;
	char* cptr;
	// char* wstart = cptr; //points to start of word
	char** base = malloc(CSIZE); //pointer to array of words
		errorAlloc(base);
	int wlen = 0; //length of current word
	int len = 0; //length of array of words
	//Must get all the strings in the text, store in free memory
	while ((c = getchar()) != EOF) //keep reading until the end of file
	{
		errorIn();
		if (c == ' ') //keep going until non space char
			continue;
		// READY NEXT WORD:
		wlen = 0; //reset word length
		cptr = malloc(CSIZE); //points to char of word
		errorAlloc(cptr);
		// READ IN WORD
		while (c != ' ' && c != EOF) //keep getchar until space
		{
			cptr[wlen] = c; //store letter at end of string
			wlen++; //word grows by one char
			cptr = realloc(cptr, (wlen+1)*sizeof(char)); //allocate more memory
			errorAlloc(cptr);
			c = getchar(); //get next letter
			errorIn();
		}
		// ADD WORD TO BASE
		base[len] = cptr;
		len++;
		base = realloc(base, (len+1)*CSIZE);
		errorAlloc(base);
		// free(cptr);
	}//end of while loop

	qsort(base, len, CSIZE, compar); //Sort array
	// OUTPUT SORTED WORDS
	int i, j;
	char out;
	for (i=0; i< len; i++)
	{
		for (j=0; base[i][j] != ' ' && base[i][j] != '\0' && base[i][j] != EOF; j++)
			errorOut(putchar(base[i][j]));
		putchar(' ');
	}
	putchar('\n');//New line, make the output pretty
	// Free all the dynamic memory pointers
	if (len != 0) //not an empty file
	{
		for (i = 0; i < len; i++)
			free(base[i]);
		free(base);
	}
	exit(0);
}