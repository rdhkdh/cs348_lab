#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_SYMBOL_TABLE_SIZE 100

// Define the symbol table structure
typedef struct {
    char label[10];
    int address;
} SymbolTableEntry;

// Global variables
SymbolTableEntry symbolTable[MAX_SYMBOL_TABLE_SIZE];
int symbolTableSize = 0;

// Function declarations
void passOne(FILE* input);
void passTwo(FILE* input, FILE* output);
int getSymbolAddress(char* symbol);
void addToSymbolTable(char* label, int address);
void printSymbolTable();

int main() {
    FILE* input = fopen("input.asm", "r");
    FILE* output = fopen("output.obj", "w");

    if (input == NULL || output == NULL) {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    // Pass one - build symbol table
    passOne(input);

    // Print the symbol table
    printSymbolTable();

    // Reset the file pointer for pass two
    fseek(input, 0, SEEK_SET);

    // Pass two - generate machine code
    passTwo(input, output);

    printf("Assembly successful.\n");

    fclose(input);
    fclose(output);

    return 0;
}

// Pass one - build the symbol table
void passOne(FILE* input) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), input) != NULL) {
        char label[10], opcode[10], operand[10];
        sscanf(line, "%s %s %s", label, opcode, operand);

        if (label[0] != '.') {  // Ignore comments
            if (label[0] != '\0') {
                addToSymbolTable(label, 0);  // Assume address 0 for labels in pass one
            }
        }
    }
}

// Pass two - generate machine code
void passTwo(FILE* input, FILE* output) {
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), input) != NULL) {
        char label[10], opcode[10], operand[10];
        sscanf(line, "%s %s %s", label, opcode, operand);

        if (label[0] != '.') {  // Ignore comments
            int address = getSymbolAddress(label);

            // Generate machine code based on opcode and operand
            // This is where you would handle different opcodes and addressing modes
            // For simplicity, assume only a few instructions and direct addressing here

            fprintf(output, "%02X %04X\n", 3, address);
        }
    }
}

// Get the address of a symbol from the symbol table
int getSymbolAddress(char* symbol) {
    for (int i = 0; i < symbolTableSize; i++) {
        if (strcmp(symbol, symbolTable[i].label) == 0) {
            return symbolTable[i].address;
        }
    }
    return -1;  // Symbol not found
}

// Add a label and its address to the symbol table
void addToSymbolTable(char* label, int address) {
    if (symbolTableSize < MAX_SYMBOL_TABLE_SIZE) {
        strcpy(symbolTable[symbolTableSize].label, label);
        symbolTable[symbolTableSize].address = address;
        symbolTableSize++;
    } else {
        printf("Error: Symbol table full.\n");
        exit(EXIT_FAILURE);
    }
}

// Print the symbol table
void printSymbolTable() {
    printf("Symbol Table:\n");
    printf("Label\tAddress\n");

    for (int i = 0; i < symbolTableSize; i++) {
        printf("%s\t%04X\n", symbolTable[i].label, symbolTable[i].address);
    }
}
