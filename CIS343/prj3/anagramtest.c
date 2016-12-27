/************************************************************************
 * filename: anagrams.c													*
 *																		*
 * Reads from a dictionary of words and builds lists of anagrams		*
 * (see example dictionary files and output produced for each 			*
 * input/dictionary file).												*
 *																		*
 * An anagram is a word formed by rearranging the letters of another	*
 * word such as the word "cinema" formed from the word "iceman".		*
 *																		*
 * Author(s): Sam Essenburg Isaac Smith									*
 ***********************************************************************/

#include <stdbool.h>
        #include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_ARRAY_SIZE 40000
#define MAX_WORD_SIZE 50

// node in a linked list
struct node {
	char *text;
	struct node *next;
};

typedef struct node Node;

// structure used for an array element
//		- head field points to the first element of a linked list
//		- size field stores number of nodes in the linked list
typedef struct {
	int size;
	Node *head;
} AryElement;

/************************************************************************
 * YOU MUST NOT DEFINE ANY GLOBAL VARIABLES (i.e., OUTSIDE FUNCTIONS).  *
 * COMMUNICATION BETWEEN FUNCTIONS MUST HAPPEN ONLY VIA PARAMETERS.     *
 ************************************************************************/

/************************************************************************
 * Function declarations/prototypes										*
 ************************************************************************/

AryElement *buildAnagramArray(char *infile, int *aryLen);

void printAnagramArray(char *outfile, AryElement *ary, int aryLen);

void freeAnagramArray(AryElement *ary, int aryLen);

bool areAnagrams(char *word1, char *word2);

Node *createNode(char *word);


/************************************************************************
 * Main driver of the program.											*
 * The input file is assumed to contain one word (of lower case			*
 * letters only) per line.												*
 * Use the following commands to compile and run the program:			*
 *		$ gcc -Wall -std=c99 -o anagrams anagrams.c						*
 * 		$ ./anagrams  dictionary1.txt  output1.txt						*
 ************************************************************************/
int main(int argc, char *argv[])
{
	AryElement *ary;
	int aryLen;

	if (argc != 3) {
		printf("Wrong number of arguments to program.\n");
		printf("Usage: ./anagrams infile outfile\n");
		exit(EXIT_FAILURE);
	}

	char *inFile = argv[1];
	char *outFile = argv[2];
	
	ary = buildAnagramArray(inFile,&aryLen);

	printAnagramArray(outFile,ary,aryLen);

	freeAnagramArray(ary,aryLen);

	return EXIT_SUCCESS;
}

/************************************************************************
 * Takes a filename that contains one word (of lower case letters) per	*
 * line, reads the file contents, and builds an array of linked lists	*
 * and returns a pointer to this array. It also sets the aryLen 		*
 * parameter to size/length of the array returned from the function.	*
 ************************************************************************/
AryElement *buildAnagramArray(char *infile, int *aryLen)
{
	AryElement *ary = NULL;		// stores pointer to dynamically allocated array of structures
	char word[MAX_WORD_SIZE];	// stores a word read from the input file
	int curAryLen;				// stores current length/size of the array
	int nbrUsedInAry = 0;		// stores number of actual entries in the array
    	//int read;
    	char *line = NULL;
    	size_t len = 0;
    	size_t read;

	// prepare the input file for reading
	FILE *fp = fopen(infile,"r");
	if (fp == NULL) {
		fprintf(stderr,"Error opening file %s\n", infile);
		exit(EXIT_FAILURE);
	}

    ary = calloc(INITIAL_ARRAY_SIZE, sizeof(AryElement));
    curAryLen = INITIAL_ARRAY_SIZE;

	while(read = getline(&line, &len, fp) != -1){
		printf("Start of loop\n");
		printf("theword %s\n", line);
		Node *n = createNode(line);
		printf("nodetexttest%s\n", n->text);
        	for(int i = 0; i <= nbrUsedInAry; i++){
		   if(nbrUsedInAry == 0){
			ary[i].head = n;
			nbrUsedInAry++;
			break;
		   }
		   if(areAnagrams(ary[i].head->text, line)){
			printf("are anagrams\n");
			(ary[i].head)->next = n;
			ary[i].size = ary[i].size + 1;
		   }else{
			printf("not anagrams\n");
		   }
		   if(i == nbrUsedInAry){
			ary[nbrUsedInAry].size = 1;
			ary[nbrUsedInAry].head = n;
			nbrUsedInAry++;
			break;
			
		   }
		}

		//int index = nbrUsedInAry * sizeof(AryElement);
    }
	fclose(fp);
	*aryLen = nbrUsedInAry;

    return ary;
}

/************************************************************************
 * Takes a filename used for output, a pointer to the array, and size	*
 * of the array, and prints	the list of anagrams (see sample output) 	*
 * to the file specified.												*
 ************************************************************************/
void printAnagramArray(char *outfile, AryElement *ary, int aryLen)
{
	FILE *fp = fopen(outfile, "w");
	if (fp == NULL) {
		fprintf(stderr,"Error opening file %s\n", outfile);
		exit(EXIT_FAILURE);
	}

	// TO DO
	for(int i = 0; i < aryLen; i++){
	   if(ary[i].size > 1){
	     Node *temp = ary[i].head;
	     while(temp->next != NULL){
		fprintf(fp, "%c ", temp->text);
		temp = temp->next; 
	     }
	     fprintf(fp, "\n");
	   }
	}	
	fclose(fp);
}

/************************************************************************
 * Releases memory allocated for the array and all the linked lists.	*
 * This involves releasing the memory allocated for each Node of each	*
 * linked list and the array itself. Before freeing up memory of a Node *
 * object, make sure to release the memory allocated for the 			*
 * "text" field of that node first.										*
 ************************************************************************/
void freeAnagramArray(AryElement *ary, int aryLen)
{
	Node *ptr, *tmp;
	for (int i = 0; i < aryLen; i++) {
		if (ary[i].size > 0) {
			ptr = ary[i].head;
			while (ptr != NULL) {
				free(ptr->text);
				tmp = ptr->next;
				free(ptr);
				ptr = tmp;
			}
		}
	}
	free(ary);
}

/************************************************************************
 * Allocates memory for a Node object and initializes the "text" field	*
 * with the input string/word and the "next" field to NULL. Returns a	*
 * pointer to the Node object created.									*
 ************************************************************************/
Node *createNode(char *word)
{
	Node *node = NULL;
	char *temp = NULL;
	//temp = (char*)malloc(sizeof(char));
	temp = word;
	printf("%s\n", temp);
	node = (Node*)malloc(sizeof(Node));
    	//node->text = (Node*)malloc(sizeof(temp));
	node->text = temp;
    	node->next = NULL;
	printf("node created\n");
	printf("1234node text %s\n", node->text);
	
	return node;
}

/************************************************************************
 * Returns true if the input strings are anagrams, false otherwise.		*
 * Assumes the words contain only lower case letters.					*
 ************************************************************************/
bool areAnagrams(char *word1, char *word2)
{
	// TO DO
    printf("checking for anagrams\n");
    char test = word1;
    printf("%c ", test);
    int n1 = strlen(*word1);
    int n2 = strlen(*word2);
    printf("n1n2");
    // If length of both strings is not same, then they 

    // cannot be anagram

    if (n1 != n2)
      return false;

    //sort the two words    

    char string[128];
    char temp;

        int n, m, i, j;

        n = strlen(word1);
        m = strlen(word2);
	printf("loops in checkana happening");
        for (i=0; i<n-1; i++)
        {
          for (j=i+1; j<n; j++)
          {
            if (word1[i] > word1[j])
            {
                temp = word1[i];
                word1[i] = word1[j];
                word1[j] = temp;
            }
          }
        }
        for (i=0; i<m-1; i++)
        {
          for (j=i+1; j<m; j++)
          {
            if (word2[i] > word2[j])
            {
                temp = word2[i];
                word2[i] = word2[j];
                word2[j] = temp;
            }
          }
        }                    
                                             
    // Compare sorted strings
    for (int i = 0; i < n1;  i++)
      if (word1[i] != word2[i])
        return false;
        return true;
}
