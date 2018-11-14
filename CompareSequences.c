#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHARS_LINE 100

const unsigned int FILE_PATH_ARG_INDEX = 1;
const unsigned int MATCH_ARG_INDEX = 2;
const unsigned int MISMATCH_ARG_INDEX = 3;
const unsigned int GAP_ARG_INDEX = 4;

typedef struct
{
    char name[100];
    char *letters[100];
} Sequence;

int main(int argc, char* argv[])
{
    /* Check for correct number of arguments */
    char *filePath = argv[FILE_PATH_ARG_INDEX];
    int matchScore = argv[MATCH_ARG_INDEX];
    int mismatchScore = argv[MISMATCH_ARG_INDEX];
    int gapScore = argv[GAP_ARG_INDEX];

    FILE* fp;
    fp = fopen(filePath, "r");
    /* Check if file is readable */

    char line[MAX_CHARS_LINE + 1];
    Sequence sequences[100];

    int i = -1;
    int j = 0;
    while(fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, ">", sizeof(char)) == 0)
        {
            j = 0;
            i++;
            char *name = strtok(line, "\n");
            memcpy(sequences[i].name, &name[1], strlen(name));
        }
        else
        {
            char *characters = strtok(line, "\n");
            /* check if malloc succeeded */
            sequences[i].letters[j] = (char *)malloc(strlen(characters));
            strcpy(sequences[i].letters[j], characters);
            printf("%s\n", sequences[i].letters[j]);
            j++;
        }
    }
}