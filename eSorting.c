#ifndef ESORTING_C
#define ESORTING_C

#include "utility.h"
#include "support.h"
#include "eSorting.h"

FILE* ExternalSorting(int memSize, int numReg) {
    // readI = pointer to read in the left side of a file
    // writeI = pointer to write in the left side of a file
    FILE *readI = NULL, *writeI = NULL;
    // readS = pointer to read in the right side of a file
    // writeS = pointer to write in the left side of a file
    FILE *readS = NULL, *writeS = NULL;
    // memLen = number of registers that fits on the primary memory
    int memLen = MAXREG(memSize), currentRegister = 0;
    // memory = primary memory
    Register memory[memLen];

    for (currentRegister = 0; currentRegister < memLen; currentRegister++)
        InitializeRegister(&memory[currentRegister]);

    //open and test if suceed the opening of the binary file for all the
    // 4 pointers
    readI = OpenFile(AUXDIR, "r+b");
    writeI = OpenFile(AUXDIR, "r+b");
    readS = OpenFile(AUXDIR, "r+b");
    writeS = OpenFile(AUXDIR, "r+b");

    ExternalQuicksort(&readI, &writeI, &readS, &writeS, memory, memLen, 0, numReg - 1);
    fflush(readI);
    fclose(writeI);
    fclose(readS);
    fclose(writeS);

    return readI;
} /*ExternalSorting*/

void ExternalQuicksort(FILE **readI, FILE **writeI, FILE **readS, FILE **writeS, Register *memory, int memLen, int left, int right) {
    // the two new limits for new partitions on the file
    int newPart1 = 0, newPart2 = 0;

    // if the subfile cannot be parted anymore
    if(right - left < 1)
        return;
    PartDisk(readI, writeI, readS, writeS, memory, memLen, left, right, &newPart1, &newPart2);

    // always part the smaller part before the bigger one
    if(newPart1 - left < right - newPart2) {
        ExternalQuicksort(readI, writeI, readS, writeS, memory, memLen, left, newPart1);
        ExternalQuicksort(readI, writeI, readS, writeS, memory, memLen, newPart2, right);
    } else {
        ExternalQuicksort(readI, writeI, readS, writeS, memory, memLen, newPart2, right);
        ExternalQuicksort(readI, writeI, readS, writeS, memory, memLen, left, newPart1);
    }
} /*ExternalQuicksort*/

void PartDisk(FILE **readI, FILE **writeI, FILE **readS, FILE **writeS, Register *memory, int memLen, int left, int right, int *newPart1, int *newPart2) {
    //rs = read superior, ws = write superior --> file's right side control
    long int rs = right, ws = right;
    //ri = read inferior, wi = write inferior --> file's left side control
    long int ri = left, wi = left;
    //usedMem = how much of memory it was used
    //changeSide = intercalation flag
    int usedMem = 0, changeSide = 1, currentRegister = 0;
    Register reg;

    InitializeRegister(&reg);
    // set the pointer on the start of the file
    fseek(*readI, ri*sizeof(Register), SEEK_SET);
    fseek(*writeI, wi*sizeof(Register), SEEK_SET);

    // pre-set the new limits in case they won't be set
    *newPart1 = left - 1;
    *newPart2 = right + 1;

    // put elements on the memory untill there's no room left
    FillMemory(readI, readS, memory, memLen, &usedMem, &rs, &ri, &changeSide);
    // sort the memory
    Quicksort(memory, 0, usedMem - 1);

    // visit the file (rs < ri means that the file was all visited)
    while(rs >= ri) {
        if(rs == ws)
            // prevent that rs < ws
            ReadSuperior(readS, &reg, &rs, &changeSide);
        else if(ri == wi)
        // prevent that ri < wi
            ReadInferior(readI, &reg, &ri, &changeSide);
        else if(changeSide)
            //consider the intercalation of sides to read an element
            ReadSuperior(readS, &reg, &rs, &changeSide);
        else
            ReadInferior(readI, &reg, &ri, &changeSide);
        if(Compare(&reg, &memory[0]) < 0) {
            // is the read element is less than the lesser of the memory
            *newPart1 = wi;
            WriteMinimum(writeI, reg, &wi);
            continue;
        } else if(Compare(&reg, &memory[usedMem - 1]) > 0) {
            // is the read element is great than the greater of the memory
            *newPart2 = ws;
            WriteMaximum(writeS, reg, &ws);
            continue;
        } else {
            // if the read element is between the greater and the lesser elements of the memory
            if(wi - left < right - ws)
                ChangeMemory(writeI, memory, reg, 0, &wi);
            else
                ChangeMemory(writeS, memory, reg, memLen - 1, &ws);
            // re-sort the memory
            Insertionsort(memory, usedMem);
        }
    }

    // write the rest of the ordened elementes of the memory in the file
    for (currentRegister = 0; currentRegister < usedMem; currentRegister++)
        WriteMinimum(writeI, memory[currentRegister], &wi);

    fflush(*readI);
    fflush(*writeI);
    fflush(*readS);
    fflush(*writeS);
} /*PartDisk*/

void ReadInferior(FILE **readI, Register *reg, long int *ri, int *flag) {
    fread(reg, sizeof(Register), 1, *readI);
    (*ri) ++;
    *flag = 1;
} /*ReadInferior*/

void WriteMinimum(FILE **writeI, Register reg, long int *wi) {
    fwrite(&reg, sizeof(Register), 1, *writeI);
    (*wi) ++;
} /*WriteMinimum*/

void ReadSuperior(FILE **readS, Register *reg, long int *rs, int *flag) {
    fseek(*readS, *rs*sizeof(Register), SEEK_SET);
    fread(reg, sizeof(Register), 1, *readS);
    (*rs) --;
    *flag = 0;
} /*ReadSuperior*/

void WriteMaximum(FILE **writeS, Register reg, long int *ws) {
    fseek(*writeS, *ws*sizeof(Register), SEEK_SET);
    fwrite(&reg, sizeof(Register), 1, *writeS);
    (*ws) --;
} /*WriteMaximum*/

void FillMemory(FILE **readI, FILE **readS, Register *memory, int memLen, int *usedMem, long int *rs, long int *ri, int *flag) {
    while((*usedMem < memLen) && (*rs >= *ri)) {
        if(*flag)
            //consider the intercalation of sides to read an element
            ReadSuperior(readS, &memory[*usedMem], rs, flag);
        else
            ReadInferior(readI, &memory[*usedMem], ri, flag);
        // update the memory's used space
        *usedMem += 1;
    }
} /*FillMemory*/

void ChangeMemory(FILE **writeFile, Register *memory, Register reg, int pos, long int *type) {
    if(pos == 0)
        // take the lesser or the greater element from the memory and write on the file
        WriteMinimum(writeFile, memory[pos], type);
    else
        WriteMaximum(writeFile, memory[pos], type);

    InitializeRegister(&memory[pos]);
    // fill the position with a new element
    memory[pos] = BuildRegister(reg.word, reg.document, reg.frequency, reg.position);
} /*ChangeMemory*/

#endif  //ESORTING_C
