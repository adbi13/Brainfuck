#ifndef BRAINFUCKLIB_H
#define BRAINFUCKLIB_H

#define BF_SUCCESS 0
#define BF_FAILURE 1
#define BF_REWIND 2
#define BF_BREAK 3

struct Program
{
    char *commands;
    size_t commandsSize;
    size_t actualCommand;

    unsigned char *memory;
    size_t memorySize;
    unsigned char *pointerPosition;
};

int initProgram(struct Program *program);

int destroyProgram(struct Program *program);

int step(struct Program *program);

int run(struct Program *program);

int load(FILE *input, struct Program *program);

#endif // BRAINFUCKLIB_H