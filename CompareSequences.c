#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_CHARS_LINE 100
#define MAX_SEQUENCES_NUM 100

const int NUM_ARGUMENTS = 4;
const unsigned int FILE_PATH_ARG_INDEX = 1;
const unsigned int MATCH_ARG_INDEX = 2;
const unsigned int MISMATCH_ARG_INDEX = 3;
const unsigned int GAP_ARG_INDEX = 4;
const char SEQUENCE_NAME_LINE_MARKER = '>';
const char *MEMORY_ALLOCATION_ERR = "Memory allocation failed";

typedef struct
{
    char name[100];
    char *letters;
    int numCharacters;
} Sequence;

/**
 * Concatenates strings
 * @param old_str base string
 * @param new_str characters to add to old_str
 */
void concatenateStrings(char *old_str, const char *new_str)
{
    char *buffer = (char *)malloc(strlen(old_str) + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "%s", MEMORY_ALLOCATION_ERR);
        exit(EXIT_FAILURE);
    }
    strcpy(buffer, old_str);
    old_str = (char *)realloc(old_str, strlen(new_str) + strlen(buffer) + 1);
    strcpy(old_str, buffer);
    strcat(old_str, new_str);
    free(buffer);
}

void parseSequences(FILE* filePtr, Sequence sequences[MAX_SEQUENCES_NUM], int *numSequences)
{
    char line[MAX_CHARS_LINE + 1];
    int sequenceIndex = -1;

    while(fgets(line, sizeof(line), filePtr))
    {
        if (strncmp(line, &SEQUENCE_NAME_LINE_MARKER, sizeof(char)) == 0)
        {
            sequenceIndex++;
            char *name = strtok(line, "\n");
            memcpy(sequences[sequenceIndex].name, &name[1], strlen(name));
            sequences[sequenceIndex].numCharacters = 0;
        }
        else
        {
            char *characters = strtok(line, "\n");
            if (sequences[sequenceIndex].numCharacters != 0)
            {
                concatenateStrings(sequences[sequenceIndex].letters, characters);
            }
            else
            {
                sequences[sequenceIndex].letters = (char *)malloc(strlen(characters) + 1);
                if (sequences[sequenceIndex].letters == NULL)
                {
                    fprintf(stderr, "%s", MEMORY_ALLOCATION_ERR);
                    exit(EXIT_FAILURE);
                }
                strcpy(sequences[sequenceIndex].letters, characters);
            }
            sequences[sequenceIndex].numCharacters = (int)strlen(sequences[sequenceIndex].letters);
        }
    }

    *numSequences = sequenceIndex + 1;
}

/**
 * Assigns string value to int variable. If failure is detected, exits the program.
 * @param str string to convert
 * @param variable int pointer
 */
void convertStrToInt(const char *str, int *variable)
{
    char *endptr;
    *variable = (int)strtol(str, &endptr, 10);
    if (*variable == 0 && (errno != 0 && endptr == str))
    {
        fprintf(stderr, "Illegal parameter supplied");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    if (argc < NUM_ARGUMENTS)
    {
        fprintf(stdout, "Usage: CompareSequences <path_to_sequences_file> <m> <s> <g>");
        exit(EXIT_FAILURE);
    }

    char *filePath = argv[FILE_PATH_ARG_INDEX];
    int matchScore, mismatchScore, gapScore;
    convertStrToInt(argv[MATCH_ARG_INDEX], &matchScore);
    convertStrToInt(argv[MISMATCH_ARG_INDEX], &mismatchScore);
    convertStrToInt(argv[GAP_ARG_INDEX], &gapScore);

    FILE* fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "File can't be opened!");
        exit(EXIT_FAILURE);
    }
    Sequence sequences[MAX_SEQUENCES_NUM];
    int numSequences;
    parseSequences(fp, sequences, &numSequences);
    fclose(fp);

    int **dynamicTable;
    for (int k = 0; k < numSequences; ++k)
    {
        for (int h = k + 1; h < numSequences; ++h)
        {
            dynamicTable = (int **)malloc((sequences[k].numCharacters+1)*sizeof(int *));
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
            printf("%s and %s, %d\n", sequences[k].name, sequences[h].name,
                   dynamicTable[sequences[k].numCharacters][sequences[h].numCharacters]);

            for (int row = 0; row < sequences[k].numCharacters+1; ++row)
            {
                free(dynamicTable[row]);
            }
            free(dynamicTable);
        }
    }

    for (int j = 0; j < numSequences; ++j)
    {
        free(sequences[j].letters);
    }
}