#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996) // For Visual Studio
#endif

struct Node {
    char* symbol;
    uint16_t address;
};

#define HASHSIZE 8000 // 2^15 bytes, or 32768 = 8000 in hex

struct Node* hashArray[HASHSIZE];

uint16_t hashFunction(char* symbol) {
    uint64_t hash = 0;
    for(uint64_t i = 0; i < strlen(symbol); i++) {
        hash += symbol[i];
    }
    return (hash * 3) % HASHSIZE;
}

void insertNode(char* symbol, uint16_t address) {
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

uint8_t checkIfDirective(char* string) {
    if((strcmp(string,"START") == 0 ) || (strcmp(string,"END") == 0 ) || (strcmp(string,"BYTE") == 0 ) ||
       (strcmp(string,"WORD") == 0 ) || (strcmp(string,"RESB") == 0 ) || (strcmp(string,"RESW") == 0 ) ||
       (strcmp(string,"RESR") == 0 ) || (strcmp(string,"EXPORTS") == 0 ))
        return 0;
    return 1;
}

uint8_t checkIfConstant() {

}

void printError(char* line,uint64_t lineNumber, char* error) {
    printf("<%s>\nLine<%llu><%s>\n",line,lineNumber,error);
}

int main(uint8_t argc, char* argv[]) {
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
    uint16_t address = 0;
    char symbol[7];
    struct {
        uint8_t symbolFlag : 1; // this token is a symbol
        uint8_t startFlag : 1; // the program has started
        uint8_t endFlag : 1; // there's an end
    } Flags;
    Flags.symbolFlag = 0;
    Flags.startFlag = 0;
    Flags.endFlag = 0;
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

        if(line[0] == '#') // comment case
            continue;

        if(strlen(line) > 2) {
            char* token = strtok(line," \t");
            if((line[0] >= 'A') && (line[0] <= 'Z')) { // Read symbol
                if(findNode(token) != NULL) {
                    printError(nonNullTerminatedStringString,lineCount,"Duplicate symbol");
                    return 1;
                } else if(checkIfDirective(token) == 0) {
                    printError(nonNullTerminatedStringString,lineCount,"Symbol = directive name");
                    return 1;
                } else if(strlen(token) > 6) {
                    printError(nonNullTerminatedStringString,lineCount,"Symbol cannot be longer than 6 characters");
                    return 1;
                } else if(stringHasSpecialChars(token) != 0) {
                    printError(nonNullTerminatedStringString,lineCount,"Symbol cannot contain spaces, $, !, =, +, -, (, ), @");
                    return 1;
                } else {
                    strcpy(symbol,token);
                    Flags.symbolFlag = 1;
                }
            }

            while(token) {
                if(strcmp(token, "START") == 0) {
                    char* temp = strtok(NULL," \t");

                    if(Flags.symbolFlag == 0) {
                        printError(nonNullTerminatedStringString,lineCount,"Missing symbol for START");
                        return 1;
                    }

                    if(temp == NULL) {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for START");
                        return 1;
                    }

                    int64_t startAddress = (uint32_t)strtol(temp,NULL,16);

                    if((startAddress >= 32768) || (startAddress < 0)) {
                        printError(nonNullTerminatedStringString,lineCount,"Invalid START address");
                        return 1;
                    } else {
                        address = startAddress;
                    }

                    insertNode(symbol, address);
                    printf("%s %X\n", symbol, address);
                    Flags.symbolFlag = 0;
                    Flags.startFlag = 1;
                    break;
                }

                if((Flags.startFlag == 0) && Flags.symbolFlag == 0) {
                    printError(nonNullTerminatedStringString,lineCount,"Missing START");
                    return 1;
                }
                
                // print symbol and the address it's located at
                if((Flags.symbolFlag == 1) && (Flags.startFlag == 1)) {
                    insertNode(symbol, address);
                    printf("%s %X\n", symbol, address);
                }

                char* newLinelessToken = (char*)malloc(strlen(token) + 1);
                strcpy(newLinelessToken,token);
                removeNewLine(newLinelessToken);
                if(checkIfOpcode(newLinelessToken) == 0) {
                    address+=3;
                    free(newLinelessToken);
                    if(address > 32768) {
                        printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                        return 1;
                    }
                    break;
                } else {
                    if((Flags.symbolFlag == 0) &&
                       (checkIfDirective(newLinelessToken) == 1) &&
                       (token[0] != '\n') &&
                       (token[0] != '\r')) {
                        printError(nonNullTerminatedStringString,lineCount,"Invalid instruction");
                        return 1;
                    }
                    free(newLinelessToken);
                } 
                
                if(strcmp(token, "WORD") == 0) {
                    char* temp = strtok(NULL," \t");
                    if((atoi(temp) > 8388607) || (atoi(temp) < -8388608)) { // 2^24 Word size
                        printError(nonNullTerminatedStringString,lineCount,"Word constant is invalid");
                        return 1;
                    }
                    if(temp != NULL) {
                        address+=3;
                        if(address > 32768) {
                            printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                            return 1;
                        }
                    } else {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for WORD");
                        return 1;
                    }
                    break;
                } else if(strcmp(token, "RESW") == 0) {
                    char* temp = strtok(NULL," \t");
                    if(temp != NULL) {
                        address+= (3*atoi(temp));
                        if(address > 32768) {
                            printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                            return 1;
                        }
                    } else {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for RESW");
                        return 1;
                    }
                    break;
                } else if(strcmp(token, "RESB") == 0) {
                    char* temp = strtok(NULL," \t");
                    if(temp != NULL) {
                        address+= atoi(temp);
                        if(address > 32768) {
                            printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                            return 1;
                        }
                    } else {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for RESB");
                        return 1;
                    }
                    break;
                } else if(strcmp(token, "BYTE") == 0) {
                    char* byteString = strtok(NULL,"\t");
                    if(byteString == NULL) {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for BYTE");
                        return 1;
                    }
                    switch(byteString[0]) {
                        case 'C':
                            for(uint64_t i = 2; (byteString[i] != '\'') && (i<strlen(byteString)); i++) {
                                if((byteString[i] >= 0) && (byteString[i] < 256)) {
                                    address+=1;
                                    if(address > 32768) {
                                        printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                                        return 1;
                                    }
                                } else {
                                    printError(nonNullTerminatedStringString,lineCount,"Invalid char constant");
                                    return 1;
                                }
                            }
                            break;
                        case 'X':
                            for(uint64_t i = 2; (byteString[i] != '\'') && (i<strlen(byteString)); i++) {
                                if(strchr("0123456789abcdefABCDEF", byteString[i]) != NULL) {
                                    if((i % 2) == 1) {
                                        address+=1;
                                        if(address > 32768) {
                                            printError(nonNullTerminatedStringString,lineCount,"Out of memory");
                                            return 1;
                                        }
                                    }
                                } else {
                                    printError(nonNullTerminatedStringString,lineCount,"Invalid hex constant");
                                    return 1;
                                }
                            }
                            break;
                        default:
                            printError(nonNullTerminatedStringString,lineCount,"Invalid type of byte constant");
                            return 1;
                    }
                } else if(strcmp(token,"START") == 0) {
                    printError(nonNullTerminatedStringString,lineCount,"Duplicate START");
                    return 1;
                } else if(strcmp(token, "END") == 0) {
                    char* temp = strtok(NULL," \t");
                    if(temp == NULL) {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for END");
                        return 1;
                    }
                    if(Flags.endFlag != 1)
                        Flags.endFlag = 1;
                    else {
                        printError(nonNullTerminatedStringString,lineCount,"Duplicate END");
                        return 1;
                    }
                }
                if(Flags.startFlag)
                    Flags.symbolFlag = 0;
                token = strtok(NULL," \t"); // will be null or the next token
            }
            
        } else {
            printError(nonNullTerminatedStringString,lineCount,"Empty line");
            return 1;
        }
    }

    fclose(inputFile);
    return 0;
}