#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHSIZE 200

struct Node {
    char* symbol;
    unsigned int address;
    unsigned int order;
};

struct Node* hashArray[HASHSIZE];

unsigned int hashFunction(char* symbol) {
    unsigned int hash = 0;
    for(unsigned int i = 0; i < strlen(symbol); i++) {
        hash += symbol[i];
    }
    return hash % HASHSIZE;
}

void insertNode(char* symbol, unsigned int address, unsigned int order) {
    unsigned int index = hashFunction(symbol);
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode->symbol = symbol;
    newNode->address = address;
    newNode->order = order;
    while(!hashArray[index]) {
        index = (index + 1) % HASHSIZE;
    }
    hashArray[index] = newNode;
}

struct Node* findNode(char* symbol) {
    unsigned int index = hashFunction(symbol);
    while(hashArray[index]) {
        if(strcmp(hashArray[index]->symbol, symbol) == 0)
            return hashArray[index];
        index = (index + 1) % HASHSIZE;
    }
    return NULL;
}

void inOrderTraversal(unsigned int count) {
    struct Node* temp;
    for(int i = 0; i < count; i++) {
        temp = findNode(count);
        printf("%s %X\n",temp->symbol,temp->address);
    }
}

int stringHasSpecialChars(char* string) {
    for(int i = 0; i < strlen(string); i++) {
        if((string[i] == ' ') || (string[i] == '$') || (string[i] == '!') || (string[i] == '=') ||
           (string[i] == '+') || (string[i] == '-') || (string[i] == '(') || (string[i] == ')') || (string[i] == '@'))
            return 1;
    }
    return 0;
}

void printError(char* line,unsigned int lineNumber, char* error) {
    printf("<%s>\nLine<%d><%s>\n",line,lineNumber,error);
}

int main(unsigned int argc, char* argv[]) {
    /* SIC Specifications */
    /* Memory */
    // 8-bit bytes, 3-byte words, 32K byte limit (2^15 bytes)

    /* Registers (#) */
    // All word sized:
    // A - Accumulator, for arithmetics(0)
    // X - Index register, addressing(1)
    // L - Linkage register, stores the RA from JSUB instruction(2)
    // PC - Program Counter, contains address of next instruction to be fetched(8)
    // SW - Status Word, contains a variety of info, including Condition Code(CC)(9)

    /* Data/Instruction Formats */
    // All integers are 24-bit signed, Characters are 8-bit ascii coded, SIC has no support for floating point
    // Instruction format(24-bit): opcode(8)-x,addressing mode(1)-address(15)
    // If x = 0; target address(TA) = address. If x = 1; TA = address + offset

    /* Other Quirks */
    // I/O operations are performed one byte at a time to or from the rightmost 8-bits of register A
    // Each connected device is assigned an 8-bit device ID. The SIC has three I/O instructions which specify 1-byte operand and device ID
    // Test device(TD) instruction sets CC to test a device ID for I/O

    // Symbols cannot be defined with a name that matches a directive
    // Symbols must start with alpha character[A-Z]
    // Symbols cannot be longer than six characters
    // Symbols cannot contain spaces, $, !, =, +, -, (, ), @
    // # is a comment

    if(argc != 2) {
        printf("Use %s with <filename>",argv[0]);
        return 1;
    }

    FILE* inputFile = fopen(argv[1],"r");
    if(!inputFile) { // If pointer to buffer is null
        printf("Error in opening file\n");
        return 1;
    }

    // Pass 1
    unsigned int lineCount = 0;
    unsigned int address = 0;
    unsigned int symbolCount = 0;
    char line[1024];
    while(fgets(line,1024,inputFile)) {
        lineCount++;
        if(strlen(line) > 0) {
            if(line[0] == '#') // comment case
                continue;

            char* token = strtok(line," \t");
            if((line[0] >= 'A') && (line[0] <= 'Z')) { // Read symbol
                if(findNode(token) != NULL) {
                    printError(line,lineCount,"Duplicate symbol");
                } else if((strcmp(token,"START") == 0 ) || (strcmp(token,"END") == 0 ) || (strcmp(token,"BYTE") == 0 ) ||
                          (strcmp(token,"WORD") == 0 ) || (strcmp(token,"RESB") == 0 ) || (strcmp(token,"RESW") == 0 ) ||
                          (strcmp(token,"RESR") == 0 ) || (strcmp(token,"EXPORTS") == 0 )) {
                    printError(line,lineCount,"Symbol = directive name");
                } else if(strlen(token) > 6) {
                    printError(line,lineCount,"Symbol cannot be longer than 6 characters");
                } else if(stringHasSpecialChars(token) != 0) {
                    printError(line,lineCount,"Symbol cannot contain spaces, $, !, =, +, -, (, ), @");
                } else {
                    insertNode(token,address,symbolCount++);
                }
            }
            
            while(token) {
                
                token = strtok(NULL," \t"); // will be null or the next token
            }
        }
    }
    inOrderTraversal(symbolCount);

    fclose(inputFile);
    return 0;
}