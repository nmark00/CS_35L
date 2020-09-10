#include<stdio.h>
#include<stdlib.h>

char unfrobByte(const char a);
int frobcmp(char const* a, char const* b);
int checkForIOError(void)
{
  if (ferror(stdin))
  {
    fprintf(stderr, "I/O Error! Exiting...\n");
    return 0;
  }
  return 1;
}

// Generic Function to call frbcmp from void* type data, to send to qsort.
int genericFrbCmp(const void* a, const void* b)
{
  // Typecasting to double pointers, as thats the datatype of allTheWords
  // and allTheWords is going to be sorted.
  const char* inp1 = *(const char**)a;
  const char* inp2 = *(const char**)b;
  return frobcmp(inp1, inp2);
}

int main(void)
{
  char* currentWord; // Its a pointer so that it can store a variably sized string.
  char** allTheWords; // Double pointer to store pointers to "currentWords".
  
  currentWord = (char*)malloc(sizeof(char)); // give space for 1 character.
  if (currentWord == NULL) // checking for memalloc erorr
  {
    fprintf(stderr, "Memory Allocation Error! Exiting...");
    exit(1);
  }
  allTheWords = NULL; // no words yet, so null.
  int n = 0; // n indicates how many letters are in currentWword.
  int m = 0; // m indicates how many words there are in allTheWords
  currentWord[n] = getchar(); // getting the first character from stdin. 
  if (!checkForIOError()) {
    free(currentWord);
    exit(1);
  }
  while (currentWord[n] != EOF && checkForIOError()) // while char isnt EOF and no IOError
  {
    char next = getchar(); // store next char from stdin in next
    if (!checkForIOError())
    {
      free(currentWord);
      for (int i = 0; i < m; i++)
	free(allTheWords[i]);
      free(allTheWords);
      exit(1);
    }
        
    if (currentWord[n] == ' ') // if space, then new word
    {
      // reserve space for m (current no) of words + 1
      char** temp = realloc(allTheWords, m*sizeof(char*) + sizeof(char*));
      if (temp != NULL)
      {
	allTheWords = temp; // if realloc succesful, then point allTheWords to new space
	allTheWords[m] = currentWord; // point last element of allTheWords to currentWord
	m++;
	n = -1; // -1 so that the future ++ makes it zero.
	currentWord = (char*)malloc(sizeof(char)); // make space for one char.
      } else {
	free (currentWord); // free all memory and exit if realloc unsuccesful.
	for (int i = 0; i < m; i++)
	  free(allTheWords[i]);
	free (allTheWords);
	fprintf(stderr, "Memory Allocation Error! Exiting...\n");
	exit(1);
      }
      if (next == EOF) // if current is space and next is EOF, then exit program.
	break;
      while (next == ' ')  // if current is space, while next is space, consume next chars
      {
	next = getchar(); // PIAZZA answer suggested treating multiple spaces as 1 space
	checkForIOError();
      }
    } else if (next == EOF) { // if current is NOT space, and next is EOF, then add space.
      next = ' ';
    }
    
    n++; // increase char count in currentWord.
    char* temp = realloc(currentWord, n*sizeof(char*) + sizeof(char*));
    if (temp != NULL)
    {
      currentWord = temp; // if realloc successful, then point currentWord to new mem
      currentWord[n] = next; // point last element of currWord to the next character.
    }
    else
    {
      free (currentWord); // free all memory and exit if realloc unsuccesful.
      for (int i = 0; i < m; i++)
	free(allTheWords[i]);
      free (allTheWords);
      fprintf(stderr, "Memory Allocation Error! Exiting...\n");
      exit(1);
    }
  }

  qsort(allTheWords, m, sizeof(char*), genericFrbCmp);

  // Write sorted list to stdout.
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; allTheWords[i][j] != ' '; j++)
    {
      printf("%c", allTheWords[i][j]);
    }
    printf(" ");
  }

  // free all allocated memory and exit with code 0.
  for (size_t i = 0; i < m; i++)
    free (allTheWords[i]);
  free (allTheWords);
  free (currentWord);
  return 0;
}

char unfrobByte(const char a)
{
  // New info: ^ is C's XOR operator!
  // Also, 00101010 is 42 in int.
  // And the ^ operator accepts two int operands.
  // and yes, a is a char but is also int when convenient
  // because typecasting.
  return (a^42);
}

int frobcmp(char const* a, char const* b)
{
  for(; ;a++, b++)
  {
    while (*a == '\0') // Null Bytes are allowed and DO NOT contribute
      a++;
    
    while (*b == '\0')
      b++;

    if (*a == ' ' && *b == ' ') // Space indicates end of word
    {
      return 0; // if both words ended, then return zero.
    }
    
    if (*a == ' ' || unfrobByte(*a) < unfrobByte(*b))
    {
      // Either a ends before b, or a < b.
      return -1;
    }

    if (*b == ' ' || unfrobByte(*b) < unfrobByte(*a))
    {
      return 1; // if b ends before a, or b < a.
    }
  }
}