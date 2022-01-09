#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "brainfuckLib.h"

#define VALID 0
#define MISSING_LEFT 1
#define MISSING_RIGHT 2
#define ENDLESS_LOOP 3


int checkRightBracket(int *bracketStack, bool emptyLoop,
    size_t actualPosition, size_t *firstLeftPosition)
{
    if (emptyLoop)
    {
        fprintf(stderr, "Error: Contains endless loop at position %d.\n", actualPosition);
        return ENDLESS_LOOP;
    }
    (*bracketStack)--;
    if (*bracketStack < 0)
    {
        fprintf(stderr, "Error: Missing left bracket for loop ending at position %d.\n", actualPosition);
        return MISSING_LEFT;
    }
    if (*bracketStack == 0)
    {
        *firstLeftPosition = 0;
    }
}


int validate(FILE* input)
{
    size_t firstLeftPosition = 0;
    bool emptyLoop = false;
    int bracketStack = 0;

    char actualCommand;
    size_t actualPosition = 0;

    while ((actualCommand = fgetc(input)) != EOF)
    {
        actualPosition++;

        switch (actualCommand)
        {
        case '[':
            if (firstLeftPosition == 0)
            {
                firstLeftPosition = actualPosition;
            }
            emptyLoop = true;
            bracketStack++;
            break;
        
        case ']':
            int rightBracketControl = checkRightBracket(&bracketStack, 
                emptyLoop, actualPosition, &firstLeftPosition);
            if (rightBracketControl != VALID)
            {
                return rightBracketControl;
            }
            break;
        
        case '.':
        case ',':
        case '+':
        case '-':
        case '<':
        case '>':
            emptyLoop = false;
        
        default:
            break;
        }
    }

    if (bracketStack > 0)
    {
        fprintf(stderr, "Error: Missing right bracket for loop starting at position %d.\n", firstLeftPosition);
        return MISSING_RIGHT;        
    }
    return VALID;
}

int runProgram(FILE *input)
{
    struct Program program;

    if (initProgram(&program) != BF_SUCCESS)
    {
        return BF_FAILURE;
    }

    if (load(input, &program) != BF_SUCCESS)
    {
        destroyProgram(&program);
        return BF_FAILURE;
    }

    if (run(&program) != BF_SUCCESS)
    {
        destroyProgram(&program);
        return BF_FAILURE;        
    }

    destroyProgram(&program);
    return BF_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Error: Expected file to open.\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[0], "r");
    if (!input)
    {
        fprintf(stderr, "Error: Can't open file %s.\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (validate(input) != VALID)
    {
        fclose(input);
        return EXIT_FAILURE;
    }

    if (fseek(input, 0, SEEK_SET) != 0)
    {
        fprintf(stderr, "Error: Problem occured while reading from %s.\n", argv[0]);
        fclose(input);
        return EXIT_FAILURE;        
    }

    if (runProgram(input) != BF_SUCCESS)
    {
        fclose(input);
        return EXIT_FAILURE;        
    }

    fclose(input);
    return EXIT_SUCCESS;
}