#ifndef ESORTING_H
#define ESORTING_H

#include "utility.h"
#include "support.h"

#define AUXDIR "./tmp/auxiliar"

// make peparations for using the external sorting algorithm
FILE* ExternalSorting(int memSize, int numReg);

// recursive call for the extern quick sort
void ExternalQuicksort(FILE **readI, FILE **writeI, FILE **readS, FILE **writeS, Register *memory, int memLen, int left, int right);

// part of the extern quick sort: separate the file in subfiles and order the elements in the primary memory
void PartDisk(FILE **readI, FILE **writeI, FILE **readS, FILE **writeS, Register *memory, int memLen, int left, int right, int *newPart1, int *newPart2);

// read an element from the start, left/inferior part, of the file/subfile
void ReadInferior(FILE **readI, Register *reg, long int *ri, int *flag);

// write an element from the start, left/inferior part, of the file/subfile
void WriteMinimum(FILE **writeI, Register reg, long int *wi);

// read an element from the end, right/superior part, of the file/subfile
void ReadSuperior(FILE **readS, Register *reg, long int *rs, int *flag);

// write an element from the end, right/superior part, of the file/subfile
void WriteMaximum(FILE **writeS, Register reg, long int *ws);

// read elements from the file/subfile and insert on the memory
void FillMemory(FILE **readI, FILE **readS, Register *memory, int memLen, int *usedMem, long int *rs, long int *ri, int *flag);

// write an element from the memory on the file and, in exchange, read an element from file and insert on the memory
void ChangeMemory(FILE **writeFile, Register *memory, Register reg, int pos, long int *type);

#endif  //ESORTING_H
