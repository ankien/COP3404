#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996) // For Visual Studio
#endif

struct Node {
    char* symbol;
    uint32_t address;
};

#define HASHSIZE (32768U / sizeof(struct Node)) // 2^15 bytes / size of symbol + address

struct Node* hashArray[HASHSIZE];

uint16_t hashFunction(char* symbol) {
    uint64_t hash = 0;
    for(uint64_t i = 0; i < strlen(symbol); i++) {
        hash += symbol[i];
    }
    return (hash * 3) % HASHSIZE;
}

void insertNode(char* symbol, uint32_t address) {
    uint16_t index = hashFunction(symbol);
    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode->symbol = (char*)malloc(strlen(symbol) + 1);
    strcpy(newNode->symbol,symbol);
    newNode->address = address;
    while(hashArray[index] != NULL) {
        index = (index + 1) % HASHSIZE;
    }
    hashArray[index] = newNode;
}

struct Node* findNode(char* symbol) {
    uint16_t index = hashFunction(symbol);
    while(hashArray[index]) {
        if(strcmp(hashArray[index]->symbol, symbol) == 0)
            return hashArray[index];
        index = (index + 1) % HASHSIZE;
    }
    return NULL;
}

uint8_t stringHasSpecialChars(char* string) {
    for(uint64_t i = 0; i < strlen(string); i++) {
        if((string[i] == ' ') || (string[i] == '$') || (string[i] == '!') || (string[i] == '=') ||
           (string[i] == '+') || (string[i] == '-') || (string[i] == '(') || (string[i] == ')') || (string[i] == '@'))
            return 1;
    }
    return 0;
}

void removeNewLine(char* string) {
    for(uint64_t i = 0; string[i] != '\0'; i++) {
        if(string[i] == '\n')
            string[i]='\0';
        else if(string[i] == '\r')
            string[i]='\0';
    }
}

uint8_t checkIfOpcode(char* string) { // Better performance could be achieved if you switched on the first character of the string
    if((strcmp(string,"ADD") == 0) || (strcmp(string,"ADDF") == 0) || (strcmp(string,"ADDR") == 0) || (strcmp(string,"AND") == 0) ||
       (strcmp(string,"CLEAR") == 0) || (strcmp(string,"COMP") == 0) || (strcmp(string,"COMPF") == 0) || (strcmp(string,"COMPR") == 0) ||
       (strcmp(string,"DIV") == 0) || (strcmp(string,"DIVF") == 0) || (strcmp(string,"DIVR") == 0) || (strcmp(string,"FIX") == 0) ||
       (strcmp(string,"FLOAT") == 0) || (strcmp(string,"HIO") == 0) || (strcmp(string,"J") == 0) || (strcmp(string,"JEQ") == 0) ||
       (strcmp(string,"JGT") == 0) || (strcmp(string,"JLT") == 0) || (strcmp(string,"JSUB") == 0) || (strcmp(string,"LDA") == 0) ||
       (strcmp(string,"LDB") == 0) || (strcmp(string,"LDCH") == 0) || (strcmp(string,"LDF") == 0) || (strcmp(string,"LDL") == 0) ||
       (strcmp(string,"LDS") == 0) || (strcmp(string,"LDT") == 0) || (strcmp(string,"LDX") == 0) || (strcmp(string,"LPS") == 0) ||
       (strcmp(string,"MUL") == 0) || (strcmp(string,"MULF") == 0) || (strcmp(string,"MULR") == 0) || (strcmp(string,"NORM") == 0) ||
       (strcmp(string,"OR") == 0) || (strcmp(string,"RD") == 0) || (strcmp(string,"RMO") == 0) || (strcmp(string,"RSUB") == 0) ||
       (strcmp(string,"SHIFTL") == 0) || (strcmp(string,"SHIFTR") == 0) || (strcmp(string,"SIO") == 0) || (strcmp(string,"SSK") == 0) ||
       (strcmp(string,"STA") == 0) || (strcmp(string,"STB") == 0) || (strcmp(string,"STCH") == 0) || (strcmp(string,"STF") == 0) ||
       (strcmp(string,"STI") == 0) || (strcmp(string,"STL") == 0) || (strcmp(string,"STS") == 0) || (strcmp(string,"STSW") == 0) ||
       (strcmp(string,"STT") == 0) || (strcmp(string,"STX") == 0) || (strcmp(string,"SUB") == 0) || (strcmp(string,"SUBF") == 0) ||
       (strcmp(string,"SUBR") == 0) || (strcmp(string,"SVC") == 0) || (strcmp(string,"TD") == 0) || (strcmp(string,"TIO") == 0) ||
       (strcmp(string,"TIX") == 0) || (strcmp(string,"TIXR") == 0) || (strcmp(string,"WD") == 0))
        return 0;
    return 1;
}

void printError(char* line,uint64_t lineNumber, char* error) {
    printf("<%s>\nLine<%llu><%s>\n",line,lineNumber,error);
}

int8_t main(uint8_t argc, char* argv[]) {
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
        printf("Use %s with <filename>\n",argv[0]);
        return 1;
    }

    FILE* inputFile = fopen(argv[1],"r");
    if(!inputFile) {
        printf("Error in opening file\n");
        return 1;
    }

    // Pass 1
    uint64_t lineCount = 0;
    uint32_t address = 0;
    char symbol[7];
    struct {
        uint8_t symbolFlag : 1;
        uint8_t startFlag : 1;
    } Flags;
    Flags.symbolFlag = 0;
    Flags.startFlag = 0;
    char line[1024];
    char nonNullTerminatedStringString[1024];
    while(fgets(line,1024,inputFile)) {
        strcpy(nonNullTerminatedStringString,line);
        for(uint64_t i = 0; i < strlen(line); i++) {
            if(nonNullTerminatedStringString[i] == '\0')
                nonNullTerminatedStringString[i]=' ';
            else if(nonNullTerminatedStringString[i] == '\n')
                nonNullTerminatedStringString[i]='\0';
        }
        lineCount++;
        if(strlen(line) > 0) {
            if(line[0] == '#') // comment case
                continue;

            char* token = strtok(line," \t");
            if((line[0] >= 'A') && (line[0] <= 'Z')) { // Read symbol
                if(findNode(token) != NULL) {
                    printError(line,lineCount,"Duplicate symbol");
                    return 1;
                } else if((strcmp(token,"START") == 0 ) || (strcmp(token,"END") == 0 ) || (strcmp(token,"BYTE") == 0 ) ||
                          (strcmp(token,"WORD") == 0 ) || (strcmp(token,"RESB") == 0 ) || (strcmp(token,"RESW") == 0 ) ||
                          (strcmp(token,"RESR") == 0 ) || (strcmp(token,"EXPORTS") == 0 )) {
                    printError(line,lineCount,"Symbol = directive name");
                    return 1;
                } else if(strlen(token) > 6) {
                    printError(line,lineCount,"Symbol cannot be longer than 6 characters");
                    return 1;
                } else if(stringHasSpecialChars(token) != 0) {
                    printError(line,lineCount,"Symbol cannot contain spaces, $, !, =, +, -, (, ), @");
                    return 1;
                } else {
                    strcpy(symbol,token);
                    Flags.symbolFlag = 1;
                }
            }

            while(token) {
                if(strcmp(token, "START") == 0) {
                    char* temp = strtok(NULL," \t");
                    address = (uint32_t)strtol(temp,NULL,16);
                    insertNode(symbol, address);
                    printf("%s %X\n", symbol, address);
                    Flags.symbolFlag = 0;
                    Flags.startFlag = 1;
                    break;
                }

                if((Flags.symbolFlag == 1) && (Flags.startFlag == 1)) {
                    insertNode(symbol, address);
                    printf("%s %X\n", symbol, address);
                } // print symbol and the address it's located at

                char* newLinelessToken = (char*)malloc(strlen(token) + 1);
                strcpy(newLinelessToken,token);
                removeNewLine(newLinelessToken);
                if(checkIfOpcode(newLinelessToken) == 0) {
                    address+=3;
                    free(newLinelessToken);
                    break;
                } else if(strcmp(token, "WORD") == 0) {
                    address+=3;
                    break;
                } else if(strcmp(token, "RESW") == 0) {
                    address+= (3*atoi(strtok(NULL," \t")));
                    break;
                } else if(strcmp(token, "RESB") == 0) {
                    address+= atoi(strtok(NULL," \t"));
                    break;
                } else if(strcmp(token, "BYTE") == 0) {
                    char* byteString = strtok(NULL," \t");
                    switch(byteString[0]) {
                        case 'C':
                            for(uint64_t i = 2; (byteString[i] != '\'') && (i<strlen(byteString)); i++)
                                address+=1;
                            break;
                        case 'X':
                            for(uint64_t i = 2; (byteString[i] != '\'') && (i<strlen(byteString)); i+=2)
                                address+=1;
                            break;
                        default:
                            printError(line,lineCount,"Invalid type of byte constant");
                            return 1;
                    }
                } else if(strcmp(token, "END") == 0) {
                    goto endPass1;
                } else if(Flags.symbolFlag != 1) {
                    printError(nonNullTerminatedStringString,lineCount,"Invalid operation code");
                    return 1;
                }
                Flags.symbolFlag = 0;
                token = strtok(NULL," \t"); // will be null or the next token
            }
            
        } else {
            printError(line,lineCount,"Empty line");
            return 1;
        }
    }
    endPass1:
    fclose(inputFile);
    return 0;
}