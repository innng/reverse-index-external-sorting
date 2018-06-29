#ifndef SUPPORT_H
#define SUPPORT_H

#include "utility.h"

#define MAXWORD 21
#define AUXDIR "./tmp/auxiliar"
#define REGSIZE 32
#define MAXREG(memSize) (memSize / REGSIZE)

// struct to contain the register of a word
typedef struct Reg{
    char word[MAXWORD];
    int document;
    int frequency;
    int position;
}Register;

// initialize a struct Register
void InitializeRegister(Register *reg);

// fill the register with recieved information
Register BuildRegister(char *word, int document, int frequency, int position);

// compare two register
int Compare(const void *a, const void *b);

// part of intern quick sort algorithm: parts an array in two around the pivot
int Partition(Register *array, int low, int high);

// recursive call for intern quick sort
void Quicksort(Register *array, int low, int high);

// insertion sort algorithm
void Insertionsort(Register *array, int size);

// count the frequency of a given word in a conjunt of chats
void CountFrequency(FILE *file);

#endif  //SUPPORT_H
