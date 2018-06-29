#ifndef UTILITY_C
#define UTILITY_C

#include "utility.h"
#include "support.h"
#include "eSorting.h"

void ReadEntry() {
    // numDoc = number of documents to be read
    // memSize = size of the primary memory used on the program
    // number of registers/words on the conjunt of documents
    int numDoc = 0, memSize = 0, numReg = 0;
    // InputDirectory = where to find the document
    // OutputDirectory = where to write the solution
    char InputDirectory[MAXDIR] = {'\0'}, OutputDirectory[MAXDIR] = {'\0'};
    // auxiliarFile = pointer to the binary file
    // outputFile = pointer to the text file
    FILE *auxiliarFile = NULL, *outputFile = NULL;

    //take the input
    scanf("%d", &numDoc);
    scanf("%d", &memSize);
    scanf("%s", InputDirectory);
    scanf("%s", OutputDirectory);

    //function's call
    auxiliarFile = OpenFile(AUXDIR, "wb+");
    outputFile = OpenOutputFile(OutputDirectory);
    OpenDocuments(auxiliarFile, InputDirectory, numDoc, &numReg);
    fclose(auxiliarFile);
    auxiliarFile = ExternalSorting(memSize, numReg);
    CountFrequency(auxiliarFile);
    WriteOutputFile(outputFile, auxiliarFile);
    fclose(auxiliarFile);
    fclose(outputFile);
} /*ReadEntry*/

FILE* OpenFile(char *filename, char *type) {
    FILE *file = NULL;
    file = fopen(filename, type);

    //test if the program suceed open the file to write and/or read
    if(file == NULL) {
        perror("Couldn't open file.");
        exit(EXIT_FAILURE);
    } else return file;
} /*OpenFile*/

FILE* OpenOutputFile(char *directory) {
    FILE *file = NULL;
    char *filename = NULL;

    //concatened the output directory with the padronized name of the file
    filename = malloc(strlen(directory) + strlen("index") + 1);
    sprintf(filename, "%s%s", directory, "index");

    file = OpenFile(filename, "w");
    free(filename);
    return file;
} /*OpenOutputFile*/

void OpenDocuments(FILE *auxiliar, char *directory, int numDoc, int *numReg) {
    FILE *document = NULL;
    char *filename = NULL;
    int currentDocument = 0;
    filename = malloc(strlen(directory) + sizeof(int) + 1);

    //open and process the documents one-by-one
    for (currentDocument = 1; currentDocument <= numDoc; currentDocument++) {
        // concatened the input directory with the number of the document
        sprintf(filename, "%s%d", directory, currentDocument);

        document = OpenFile(filename, "r");
        *numReg += ReadDocument(auxiliar, document, currentDocument);
        fclose(document);
    }
    free(filename);
} /*OpenDocuments*/

int ReadDocument(FILE *auxiliar, FILE *document, int currentDocument) {
    char word[MAXWORD] = {'\0'};
    Register reg;
    int position = 0, numReg = 0;

    InitializeRegister(&reg);
    // read the file
    while(fscanf(document, "%s", word) != EOF) {
        // save the position of the word in the file
        position = ftell(document) - strlen(word);
        // build a register that contains the word and its informations
        reg = BuildRegister(word, currentDocument, 1, position);
        // increase the number of registers/words read in this execution
        numReg += 1;
        // write on the bynary file
        auxiliar = WriteAuxiliarFile(auxiliar, reg);
    }
    return numReg;
} /*ReadDocument*/

FILE* WriteAuxiliarFile(FILE *auxiliar, Register reg) {
    fwrite(&reg, sizeof(Register), 1, auxiliar);
    return auxiliar;
} /*WriteAuxiliarFile*/

FILE* WriteOutputFile(FILE *output, FILE *auxiliar) {
    Register reg;

    InitializeRegister(&reg);
    fseek(auxiliar, 0, SEEK_SET);
    while(fread(&reg, sizeof(Register), 1, auxiliar) == 1) {
        fprintf(output, "%s,%d,%d,%d\n",  reg.word, reg.document, reg.frequency, reg.position);
    }
    return output;
}

#endif  //UTILITY_C
