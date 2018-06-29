#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "support.h"
#include "eSorting.h"

#define MAXDIR 256
#define MAXWORD 21
#define AUXDIR "./tmp/auxiliar"

// read the program input and call fuctions
void ReadEntry();

// open a file and teste if suceed
FILE* OpenFile(char *filename, char *type);

// open the text file with the solution
FILE* OpenOutputFile(char *directory);

// open the documents with chats
void OpenDocuments(FILE *auxiliar, char *directory, int numDoc, int *numReg);

// read a chat, build a register and count the number of words in it
int ReadDocument(FILE *auxiliar, FILE *document, int currentDocument);

// write a register in the binary auxiliar file
FILE* WriteAuxiliarFile(FILE *auxiliar, Register reg);

// transfer the information in the auxiliar file to the text file
FILE* WriteOutputFile(FILE *output, FILE *auxiliar);

#endif  //UTILITY_H
