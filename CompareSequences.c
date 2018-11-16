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
const char *USAGE_MSG = "Usage: CompareSequences <path_to_sequences_file> <m> <s> <g>";
const char *FILE_IO_ERR = "File can't be opened!";
const char *ILLEGAL_STR_ERR = "Illegal parameter supplied";

/**
 * Struct represnting a sequence
 */
typedef struct
{
    char name[100];
    char *letters;
    int numCharacters;
} Sequence;

/**
 * Populates dynamic table cell with integer
 * @param dynamicTable table
 * @param sequences array
 * @param row # row
 * @param column # column
 * @param matchScore score for match
 * @param mismatchScore score for mismatch
 * @param gapScore score for gap
 * @param firstSeqIndex score for first index
 * @param secondSeqIndex score for second index
 */
void populateCell(int **dynamicTable, const Sequence *sequences, const int row, const int column,
                  const int matchScore, const int mismatchScore, const int gapScore,
                  const int firstSeqIndex, const int secondSeqIndex)
{
    if (column == 0)
    {
        dynamicTable[row][column] = gapScore * row;
        return;
    }
    else if (row == 0)
    {
        dynamicTable[row][column] = gapScore * column;
        return;
    }

    int biggestResult;
    /* Case 1 - x1x2...x_i-1 and y1y2...y_j-1 */
    if (sequences[firstSeqIndex].letters[row-1] ==
        sequences[secondSeqIndex].letters[column-1])
    {
        int matchFinal = dynamicTable[row-1][column-1] + matchScore;
        biggestResult = matchFinal;
    }
    else
    {
        int mismatchFinal = dynamicTable[row-1][column-1] + mismatchScore;
        biggestResult = mismatchFinal;
    }

    /* Case 2 - x1x2...x_i and y1y2...y_j-1 */
    int gapFinalY = dynamicTable[row][column-1] + gapScore;
    if (gapFinalY > biggestResult)
    {
        biggestResult = gapFinalY;
    }

    /* Case 3 - x1x2...x_i-1 and y1y2...y_j */
    int gapFinalX = dynamicTable[row-1][column] + gapScore;
    if (gapFinalX > biggestResult)
    {
        biggestResult = gapFinalX;
    }

    dynamicTable[row][column] = biggestResult;
}

/**
 * calculates alignment for two sequences
 * @param sequences array
 * @param matchScore score for matching characters
 * @param mismatchScore score for mismatching characters
 * @param gapScore score for gaps
 * @param firstSeqIndex first seq
 * @param secondSeqIndex second seq
 * @return alignment score
 */
int calculateAlignment(const Sequence *sequences, const int matchScore,  const int mismatchScore,
                       const int gapScore, const int firstSeqIndex, const int secondSeqIndex)
{
    int alignment = 0;
    int **dynamicTable = (int **) malloc(
            (sequences[firstSeqIndex].numCharacters + 1) * sizeof(int *));
    if (dynamicTable == NULL) {
        fprintf(stderr, "%s", MEMORY_ALLOCATION_ERR);
        exit(EXIT_FAILURE);
    }
    for (int row = 0; row < sequences[firstSeqIndex].numCharacters + 1; ++row) {
        dynamicTable[row] = (int *) malloc(
                (sequences[secondSeqIndex].numCharacters + 1) * sizeof(int));
        if (dynamicTable[row] == NULL) {
            fprintf(stderr, "%s", MEMORY_ALLOCATION_ERR);
            exit(EXIT_FAILURE);
        }
        for (int column = 0; column < sequences[secondSeqIndex].numCharacters + 1; ++column)
        {
            populateCell(dynamicTable, sequences, row, column, matchScore, mismatchScore,
                         gapScore, firstSeqIndex, secondSeqIndex);
        }
    }
    alignment = dynamicTable[sequences[firstSeqIndex].numCharacters]
    [sequences[secondSeqIndex].numCharacters];

    int rowInd = 0;
    for (rowInd = 0; rowInd < sequences[firstSeqIndex].numCharacters + 1; ++rowInd)
    {
        free(dynamicTable[rowInd]);
    }
    free(dynamicTable);

    return alignment;
}

/**
 * Concatenates strings
 * @param old_str base string
 * @param new_str characters to add to old_str
 */
void concatenateStrings(char *old_str, const char *new_str)
{
    char *buffer = (char *) malloc(strlen(old_str) + 1);
    if (buffer == NULL) {
        fprintf(stderr, "%s", MEMORY_ALLOCATION_ERR);
        exit(EXIT_FAILURE);
    }
    strcpy(buffer, old_str);
    old_str = (char *) realloc(old_str, strlen(new_str) + strlen(buffer) + 1);
    strcpy(old_str, buffer);
    strcat(old_str, new_str);
    free(buffer);
}

/**
 * Parses sequences file
 * @param filePtr file pointer to be parsed
 * @param sequences array to be populated
 * @param numSequences num of sequences
 */
void parseSequences(FILE* filePtr, Sequence sequences[MAX_SEQUENCES_NUM], int *numSequences)
{
    char line[MAX_CHARS_LINE + 1];
    int sequenceIndex = -1;

    while(fgets(line, sizeof(line), filePtr))
    {
        /* New sequence line */
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
                /* First characters line */
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
        fprintf(stderr, "%s", ILLEGAL_STR_ERR);
        exit(EXIT_FAILURE);
    }
}

/**
 * Main function
 * @param argc num of arguments
 * @param argv arguments
 * @return exit status
 */
int main(int argc, char* argv[]) {
    if (argc < NUM_ARGUMENTS) {
        fprintf(stdout, "%s", USAGE_MSG);
        exit(EXIT_FAILURE);
    }

    char *filePath = argv[FILE_PATH_ARG_INDEX];
    int matchScore, mismatchScore, gapScore;
    convertStrToInt(argv[MATCH_ARG_INDEX], &matchScore);
    convertStrToInt(argv[MISMATCH_ARG_INDEX], &mismatchScore);
    convertStrToInt(argv[GAP_ARG_INDEX], &gapScore);

    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s", FILE_IO_ERR);
        exit(EXIT_FAILURE);
    }
    Sequence sequences[MAX_SEQUENCES_NUM];
    int numSequences;
    parseSequences(fp, sequences, &numSequences);
    fclose(fp);

    for (int k = 0; k < numSequences; ++k) {
        for (int h = k + 1; h < numSequences; ++h) {
            int alignment = calculateAlignment(sequences, matchScore, mismatchScore, gapScore, k,
                                               h);
            printf("Score for alignment of %s to %s is %d\n", sequences[k].name, sequences[h]
            .name, alignment);
        }
    }

    for (int j = 0; j < numSequences; ++j) {
        free(sequences[j].letters);
    }
}