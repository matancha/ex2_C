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
    char *letters;
    int numCharacters;
} Sequence;

int main(int argc, char* argv[])
{
    /* Check for correct number of arguments */
    char *filePath = argv[FILE_PATH_ARG_INDEX];
    int matchScore;
    int mismatchScore;
    int gapScore;
    /* check if sscanf failed */
    sscanf(argv[MATCH_ARG_INDEX], "%d", &matchScore);
    sscanf(argv[MISMATCH_ARG_INDEX], "%d", &mismatchScore);
    sscanf(argv[GAP_ARG_INDEX], "%d", &gapScore);

    FILE* fp;
    fp = fopen(filePath, "r");
    /* Check if file is readable */

    char line[MAX_CHARS_LINE + 1];
    Sequence sequences[100];

    int i = -1;
    while(fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, ">", sizeof(char)) == 0)
        {
            i++;
            char *name = strtok(line, "\n");
            memcpy(sequences[i].name, &name[1], strlen(name));
            sequences[i].numCharacters = 0;
        }
        else
        {
            char *characters = strtok(line, "\n");
            /* check if malloc succeeded */
            if (sequences[i].numCharacters != 0)
            {
                char *buffer = (char *)malloc(strlen(sequences[i].letters) + 1);
                strcpy(buffer, sequences[i].letters);
                sequences[i].letters = (char *)realloc(sequences[i].letters, strlen(characters)
                                                                             + strlen(buffer) + 1);
                strcpy(sequences[i].letters, buffer);
                strcat(sequences[i].letters, characters);
                free(buffer);
            }
            else
            {
                sequences[i].letters = (char *)malloc(strlen(characters));
                strcpy(sequences[i].letters, characters);
            }
            sequences[i].numCharacters = (int)strlen(sequences[i].letters);
            printf("%s\n", sequences[i].letters);
        }
    }

    int num_sequences = i + 1;
    int **dynamicTable;

    for (int k = 0; k < num_sequences; ++k)
    {
        dynamicTable = (int **)malloc((sequences[k].numCharacters+1)*sizeof(int *));
        for (int h = k + 1; h < num_sequences; ++h)
        {
            for (int row = 0; row < sequences[k].numCharacters+1; ++row)
            {
                dynamicTable[row] = (int *)malloc((sequences[h].numCharacters+1)*sizeof(int));
                for (int column = 0; column < sequences[h].numCharacters+1; ++column)
                {
                    if (column == 0)
                    {
                        dynamicTable[row][column] = gapScore * row;
                        continue;
                    }
                    else if (row == 0)
                    {
                        dynamicTable[row][column] = gapScore * column;
                        continue;
                    }

                    int biggestResult;
                    if (sequences[k].letters[row-1] == sequences[h].letters[column-1])
                    {
                        int matchFinal = dynamicTable[row-1][column-1] + matchScore;
                        biggestResult = matchFinal;
                    }
                    else
                    {
                        int mismatchFinal = dynamicTable[row-1][column-1] + mismatchScore;
                        biggestResult = mismatchFinal;
                    }

                    int gapFinalY = dynamicTable[row][column-1] + gapScore;
                    if (gapFinalY > biggestResult)
                    {
                        biggestResult = gapFinalY;
                    }

                    int gapFinalX = dynamicTable[row-1][column] + gapScore;
                    if (gapFinalX > biggestResult)
                    {
                        biggestResult = gapFinalX;
                    }

                    dynamicTable[row][column] = biggestResult;
                }
            }
//            for (int row = 0; row < sequences[k].numCharacters+1; ++row)
//            {
//                for (int column = 0; column < sequences[h].numCharacters+1; ++column)
//                {
//                    printf("%d ", dynamicTable[row][column]);
//                }
//                printf("\n");
//            }
            printf("%s and %s, %d\n", sequences[k].name, sequences[h].name,
                    dynamicTable[sequences[k].numCharacters][sequences[h].numCharacters]);
        }
    }
}