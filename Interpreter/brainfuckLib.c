#include "brainfuckLib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int initProgram(struct Program *program)
{
    program->commands = NULL;
    program->commandsSize = 0;
    program->actualCommand = 0;
    program->memory = malloc(8 * sizeof(unsigned char));
    if (!program->memory)
    {
        fprintf(stderr, "Error: Allocation failed.\n");
        return BF_FAILURE;
    }
    program->memorySize = 8;
    program->pointerPosition = program->memory;
    return BF_SUCCESS;
}

int destroyProgram(struct Program *program)
{
    free(program->commands);
    free(program->memory);
    memset(program, 0, sizeof(program));
    return BF_SUCCESS;
}

void printCommands(struct Program *program)
{
    for (size_t i = 0; i < program->commandsSize; i++)
    {
        fprintf(stderr, " %c |", program->commands[i]);
    }
}

static int loop(struct Program *program)
{
    size_t loopStart = program->actualCommand;
    int result;
    do
    {
        result = step(program);
        if (result == BF_REWIND)
        {
            program->actualCommand = loopStart;
        }
        if (result == BF_FAILURE)
        {
            return BF_FAILURE;
        }
    } while (result != BF_BREAK);

    return BF_SUCCESS;
}

static int loopEnd(struct Program *program)
{
    if (*(program->pointerPosition) == 0)
    {
        return BF_BREAK;
    }
    return BF_REWIND;
}

static int increment(struct Program *program)
{
    (*(program->pointerPosition))++;
    return BF_SUCCESS;
}

static int decrement(struct Program *program)
{
    (*(program->pointerPosition))--;
    return BF_SUCCESS;
}

static int in(struct Program *program)
{
    *(program->pointerPosition) = (unsigned char) getchar();
    return BF_SUCCESS;
}

static int out(struct Program *program)
{
    putchar(*(program->pointerPosition));
    return BF_SUCCESS;
}

static int right(struct Program *program)
{
    program->pointerPosition++;
    if (program->pointerPosition >= program->memory + program->memorySize)
    {
        program->memorySize *= 2;
        unsigned char *tmp = program->memory;
        program->memory = realloc(program->memory ,program->memorySize * sizeof(unsigned char));
        if (!program->memory)
        {
            free(tmp);
            program->memory = NULL;
            fprintf(stderr, "Error: Allocation failed.\n");
            return BF_FAILURE;            
        }
        program->pointerPosition = program->memory + (program->pointerPosition - tmp);
    }
    return BF_SUCCESS;
}

static int left(struct Program *program)
{
    if (program->pointerPosition <= program->memory)
    {
        fprintf(stderr, "Error: Step out of memory.");
        return BF_FAILURE;
    }
    program->pointerPosition--;
    return BF_SUCCESS;
}

int step(struct Program *program)
{
    if (!program)
    {
        fprintf(stderr, "Error: Can't run program.\n");
        return BF_FAILURE;        
    }
    if (!program->commands || program->actualCommand >= program->commandsSize)
    {
        fprintf(stderr, "Error: Missing command.\n");
        return BF_FAILURE;        
    }

    int (*command)(struct Program*);

    switch (program->commands[program->actualCommand])
    {
    case '+':
        command = increment;
        break;

    case '-':
        command = decrement;
        break;

    case '[':
        command = loop;
        break;

    case ']':
        command = loopEnd;
        break;

    case '<':
        command = left;
        break;

    case '>':
        command = right;
        break;

    case '.':
        command = out;
        break;

    case ',':
        command = in;
        break;
    
    default:
        fprintf(stderr, "Error: Unknown command %c (ASCII %d) at position %ld.\n", program->commands[program->actualCommand], program->commands[program->actualCommand], program->actualCommand);
        return BF_FAILURE;
    }
    
    program->actualCommand++;
    return command(program);
}

int run(struct Program *program)
{
    if (!program)
    {
        fprintf(stderr, "Error: Can't run program.\n");
        return BF_FAILURE;        
    }
    if (!program->commands)
    {
        fprintf(stderr, "Error: Missing commands.\n");
        return BF_FAILURE;        
    }
    while (program->actualCommand < program->commandsSize && program->commands[program->actualCommand] != 0)
    {
        if (step(program) == BF_FAILURE)
        {
            return BF_FAILURE;
        }
    }

    return BF_SUCCESS;
}

int load(FILE *input, struct Program *program)
{
    if (!program->commands)
    {
        program->commands = malloc(32 * sizeof(char));
        if (!program->commands)
        {
            fprintf(stderr, "Error: Allocation failed.\n");
            return BF_FAILURE;               
        }
        program->commandsSize = 32;
    }

    size_t actual = 0;
    char command;

    while ((command = fgetc(input)) != EOF)
    {
        if (program->commandsSize == actual)
        {
            char *tmp = program->commands;
            program->commands = realloc(program->commands, program->commandsSize * 2);
            if (!program->commands)
            {
                free(tmp);
                fprintf(stderr, "Error: Allocation failed.\n");
                return BF_FAILURE;
            }
            memset(program->commands + program->commandsSize, 0, program->commandsSize);
            program->commandsSize *= 2;
        }

        switch (command)
        {
        case '[': case ']':
        case '+': case '-':
        case ',': case '.':
        case '<': case '>':
            program->commands[actual] = command;
            actual++;
            break;
        
        default:
            break;
        }
    }
    
    return BF_SUCCESS;
}