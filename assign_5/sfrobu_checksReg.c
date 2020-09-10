#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define CSIZE sizeof(char*)
int foption; // 1 if -f, 0 if not

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
int decode(char const a)
{
	if (a > CHAR_MAX)
		return CHAR_MAX;
	if (a < CHAR_MIN)
		return 0;
	return a;
}
int frobcmpUP(char const *a, char const *b)
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
		if(toupper(decode(*a^42)) < toupper(decode(*b^42)) || *a == ' ')//a ends first or < b 
			return -1;
		if(toupper(decode(*a^42)) > toupper(decode(*b^42)) || *b == ' ')//b ends first or < a
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
void errorOut() //checks putchar errors
{
	fprintf(stderr, "Error With Standard Output\n");
	exit(1);
	
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
	if (foption)
		return frobcmpUP( *(char**) a, *(char**) b);
	else
		return frobcmp( *(char**) a, *(char**) b);
}

char nextchar()
{
	char* ptr;
	switch (read(STDIN_FILENO,ptr, 1))
	{
		case -1:
			fprintf(stderr, "Error reading file\n");
			exit(1);
			break;
		case 1:
			return *ptr;
			break;
		default:
			return 0;
			break;
	}
}

int main(int argc, char const *argv[])
{
// --- Check if -f is used ---
	if (argc == 2 && strcmp(argv[1], "-f") == 0)
    	foption = 1;
    else
    	foption = 0;

// ---- Error check file and allocate memory ---
	struct stat buf; //initial buffer
	if (fstat(STDIN_FILENO, &buf) == 0) //get info on file
	{
		if (S_ISREG(buf.st_mode) == 0) //check if file is regular
		{
			fprintf(stderr, "Not a Regular File\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "Error with file\n");
		exit(1);
	}

// --- Read the buffer into fptr ---
	char* fptr; //pointer to the file
	int fsize;
	fsize = buf.st_size; //size of the file (bytes)

	/**** Test Growing FIle ****/
	// fsize = fsize /2;
	/* REMOVE PREVIOUS LINE LATER */

	fptr = malloc(sizeof(char)*fsize + 1); //allocate memory for buffer
	errorAlloc(fptr);

	if (read(STDIN_FILENO,fptr, fsize) == -1)//read in file, store in fptr
	{
		fprintf(stderr, "Error reading file\n");
		exit(1);
	}

// --- Split contents into words and store into base ---
	char** base = malloc(CSIZE*fsize/2+1); //pointer to array of words
		errorAlloc(base);
	int len = 0; //length of array of words
	int i = 0;
	while (fptr[i] == ' ')//place start of base at first word (non space)
		i++;
	base[len++] = &fptr[i]; //set base to first non space char

	while (i < fsize)
	{
		if (fptr[i++] == ' ')
		{
			while (fptr[i++] == ' ');
			base[len++] = &fptr[i-1];
		}

	} 
	int olen = len;
	// printf("checkpoint 1\n");

// --- Grab the characters dynamically added ---
	//initialize variables
	char c; // len = 1
	char* cptr = (char*)malloc(CSIZE);
	errorAlloc(cptr);
	int wlen = 0; //length of current word

	while ((c = nextchar())!= 0)//keep reading until end of file
	{
		if (c == ' ') //keep going until non space char
			continue;
		wlen = 0; //reset word length
		cptr = (char*)malloc(CSIZE); //points to char of word
		errorAlloc(cptr);

		while (c != ' ' && c != 0)
		{
			// printf("checkpoint 2b\n");
			cptr[wlen++] = c; //store letter at end of string
			cptr = (char*)realloc(cptr, (wlen+1)*sizeof(char)); //allocate more memory
			errorAlloc(cptr);
			c = nextchar(); //get next letter
		}
		// ADD SPACE TO END OF WORD
		cptr[wlen++] = c; //add space to end of word
		cptr = (char*)realloc(cptr, (wlen+1)*sizeof(char)); //allocate more memory
		errorAlloc(cptr);
		// ADD WORD TO BASE
		base = realloc(base, (len+1)*CSIZE);
		errorAlloc(base);
		base[len++] = cptr;
	}

// --- Sort the words ---
	qsort(base, len, CSIZE, compar); //Sort array

// --- OUTPUT SORTED WORDS ---
	int j;
	for (i=0; i< len; i++)
	{
		for (j=0; base[i][j] != ' ' && base[i][j] != '\0' && base[i][j] != 0; j++);
			if (write(STDOUT_FILENO,base[i], j+1) == -1)
				errorOut();	
	}

// --- Free all the dynamic memory pointers ---
	if (len != 0) //not an empty file
	{
		for (i = len; i > olen; i--)
			free(base[i-1]);
		free(fptr);
		free(base);
	}
	exit(0);
}