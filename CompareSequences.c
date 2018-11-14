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
    char **letters;
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
    Sequence *seq;

    int i = -1;
    while(fgets(line, sizeof(line), fp))
    {
        if (strncmp(&line[0], ">", sizeof(char)) == 0)
        {
            i++;
            seq = (Sequence *)malloc(sizeof(Sequence));
            char *name = strtok(line, "\n");
            memcpy(seq->name, &name[1], strlen(name));
            seq->letters = NULL;
            sequences[i] = *seq;
        }
        else
        {
            seq->letters = (char **)realloc(seq->letters, strlen(strtok(line, "\n")));
            *(seq->letters) = strtok(line, "\n");
            printf("%s\n", *(seq->letters));
        }
    }

    for (int j=0; j<3; ++j)
    {
        char *hi = *(sequences[j].letters);
        printf("%s\n", *sequences[j].letters);
    }
}