#ifndef SUPPORT_C
#define SUPPORT_C

#include "utility.h"
#include "support.h"
#include "eSorting.h"

void InitializeRegister(Register *reg) {
    memset(reg, 0, sizeof(Register));
    memset(reg->word, '\0', strlen(reg->word));
    reg->frequency = 1;
} /*InitializeRegister*/

Register BuildRegister(char *word, int document, int frequency, int position) {
    Register reg;

    InitializeRegister(&reg);
    //fil the register with information
    memcpy(reg.word, word, MAXWORD);
    reg.document = document;
    reg.frequency = frequency;
    reg.position = position;

    return reg;
} /*BuildRegister*/

int Compare(const void *a, const void *b) {
    int result = 0;
    // cast the const void to a Register type
    Register *regA = (Register *)a;
    Register *regB = (Register *)b;

    // compare the registers' names
    result = strcmp(regA->word, regB->word);
    if(result != 0)
        return result;
    // if the names are equal, compare which document the registers' came
    if(regA->document != regB->document)
        return (regA->document < regB->document ? -1 : 1);
    // if the document is the same for the registers, compare their position on it
    if(regA->position != regB->position)
        return (regA->position < regB->position ? -1 : 1);
    else return 0;
} /*Compare*/

int Partition(Register *array, int low, int high) {
    int regLow = 0, regHigh = 0;
    Register pivot, reg;
    InitializeRegister(&pivot);
    InitializeRegister(&reg);

    // set the variables on critical points
    regLow = low - 1;
    pivot = array[high];

    reg = pivot;
    pivot = array[(high + low)/2];
    array[(high + low)/2] = reg;

    for (regHigh = low; regHigh < high; regHigh++) {
        if(Compare(&array[regHigh], &pivot) < 0) {
            // if some register from the right side of the pivot is less than it
            // change the register to the left side
            regLow++;
            reg = array[regLow];
            array[regLow] = array[regHigh];
            array[regHigh] = reg;
        }
    }
    // change the pivot
    array[high] = array[regLow + 1];
    array[regLow + 1] = pivot;

    return regLow + 1;
} /*Partition*/

void Quicksort(Register *array, int low, int high) {
    if(low < high) {
            int part = Partition(array, low, high);
            Quicksort(array, low, part - 1);
            Quicksort(array, part + 1, high);
    }
} /*Quicksort*/

void Insertionsort(Register *array, int size) {
    int currentRegister1 = 0, currentRegister2 = 0;
    Register reg;
    InitializeRegister(&reg);

    for (currentRegister1 = 0; currentRegister1 < size; currentRegister1++) {
        reg = array[currentRegister1];
        currentRegister2 = currentRegister1 - 1;

        while((currentRegister2 >= 0) && Compare(&reg, &array[currentRegister2]) < 0) {
            // if a register that is less than its predecessor is on the right
            // change the register's and the predecessor's position
            array[currentRegister2 + 1] = array[currentRegister2];
            currentRegister2 -= 1;
        }
        array[currentRegister2 + 1] = reg;
    }
} /*Insertionsort*/

void CountFrequency(FILE *file) {
    FILE *auxiliar = NULL;
    int cont = 0, position = 0, currentWord = 0, document = 0, newWord = 0;
    char word[MAXWORD];
    Register reg1, reg2;
    InitializeRegister(&reg1);
    InitializeRegister(&reg2);
    memset(word, '\0', strlen(word));

    auxiliar = OpenFile(AUXDIR, "r+b");

    fseek(file, 0, SEEK_SET);
    //read the first register
    fread(&reg1, sizeof(Register), 1, file);
    //set the information for the register
    cont = 1;
    strcpy(word, reg1.word);
    document = reg1.document;
    //start count frequency
    while(fread(&reg1, sizeof(Register), 1, file) == 1) {
        if(strcmp(word, reg1.word) == 0 && reg1.document == document) {
            //if the word repeat on the document
            cont ++;
            newWord = 1;
        }
        else {
            //found a new word, go back to the first word of comparation
            //and start updating the frequency
            currentWord = 0;
            while(currentWord < cont) {
                fread(&reg2, sizeof(Register), 1, auxiliar);
                reg2.frequency = cont;
                position = ftell(auxiliar) - sizeof(Register);
                fseek(auxiliar, position, SEEK_SET);
                fwrite(&reg2, sizeof(Register), 1, auxiliar);
                currentWord ++;
            }
            //set the information for the new register founded
            strcpy(word, reg1.word);
            document = reg1.document;
            cont = 1;
            newWord = 0;
        }
    }
    if(newWord == 1) {
        //if the file came to an end but there's registers to update
        currentWord = 0;
        while(currentWord < cont) {
            fread(&reg2, sizeof(Register), 1, auxiliar);
            reg2.frequency = cont;
            position = ftell(auxiliar) - sizeof(Register);
            fseek(auxiliar, position, SEEK_SET);
            fwrite(&reg2, sizeof(Register), 1, auxiliar);
            currentWord ++;
        }
    }
    fclose(auxiliar);
} /*CountFrequency*/

#endif  //SUPPORT_C
