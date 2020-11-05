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

#define HASHSIZE 1048576 // 2^20 bytes / possible symbol locations

struct Node* hashArray[HASHSIZE];

uint16_t hashFunction(char* symbol) {
    uint64_t hash = 0;
    for(uint64_t i = 0; i < strlen(symbol); i++) {
        hash += symbol[i];
    }
    return (hash * 60) % HASHSIZE;
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

// This function is redundant if you check opcodes in pass 2, but nobody told me so w/e
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

// i'm sorry, please use a hashtable in your implementation
uint8_t getOpcodeValue(char* string) {
    if(strcmp(string,"ADD") == 0)
        return 0x18;
    if(strcmp(string,"ADDF") == 0)
        return 0x58;
    if(strcmp(string,"ADDR") == 0)
        return 0x90;
    if(strcmp(string,"AND") == 0)
        return 0x40;
    if(strcmp(string,"CLEAR") == 0)
        return 0xB4;
    if(strcmp(string,"COMP") == 0)
        return 0x28;
    if(strcmp(string,"COMPF") == 0)
        return 0x88;
    if(strcmp(string,"COMPR") == 0)
        return 0xA0;
    if(strcmp(string,"DIV") == 0)
        return 0x24;
    if(strcmp(string,"DIVF") == 0)
        return 0x64;
    if(strcmp(string,"DIVR") == 0)
        return 0x9C;
    if(strcmp(string,"FIX") == 0)
        return 0xC4;
    if(strcmp(string,"FLOAT") == 0)
        return 0xC0;
    if(strcmp(string,"HIO") == 0)
        return 0xF4;
    if(strcmp(string,"J") == 0)
        return 0x3C;
    if(strcmp(string,"JEQ") == 0)
        return 0x30;
    if(strcmp(string,"JGT") == 0)
        return 0x34;
    if(strcmp(string,"JLT") == 0)
        return 0x38;
    if(strcmp(string,"JSUB") == 0)
        return 0x48;
    if(strcmp(string,"LDA") == 0)
        return 0x00;
    if(strcmp(string,"LDB") == 0)
        return 0x68;
    if(strcmp(string,"LDCH") == 0)
        return 0x50;
    if(strcmp(string,"LDF") == 0)
        return 0x70;
    if(strcmp(string,"LDL") == 0)
        return 0x08;
    if(strcmp(string,"LDS") == 0)
        return 0x6C;
    if(strcmp(string,"LDT") == 0)
        return 0x74;
    if(strcmp(string,"LDX") == 0)
        return 0x04;
    if(strcmp(string,"LPS") == 0)
        return 0xD0;
    if(strcmp(string,"MUL") == 0)
        return 0x20;
    if(strcmp(string,"MULF") == 0)
        return 0x60;
    if(strcmp(string,"MULR") == 0)
        return 0x98;
    if(strcmp(string,"NORM") == 0)
        return 0xC8;
    if(strcmp(string,"OR") == 0)
        return 0x44;
    if(strcmp(string,"RD") == 0)
        return 0xD8;
    if(strcmp(string,"RMO") == 0)
        return 0xAC;
    if(strcmp(string,"RSUB") == 0)
        return 0x4C;
    if(strcmp(string,"SHIFTL") == 0)
        return 0xA4;
    if(strcmp(string,"SHIFTR") == 0)
        return 0xA8;
    if(strcmp(string,"SSK") == 0)
        return 0xEC;
    if(strcmp(string,"STA") == 0)
        return 0x0C;
    if(strcmp(string,"STB") == 0)
        return 0x78;
    if(strcmp(string,"STCH") == 0)
        return 0x54;
    if(strcmp(string,"STF") == 0)
        return 0x80;
    if(strcmp(string,"STI") == 0)
        return 0xD4;
    if(strcmp(string,"STL") == 0)
        return 0x14;
    if(strcmp(string,"STS") == 0)
        return 0x7C;
    if(strcmp(string,"STSW") == 0)
        return 0xE8;
    if(strcmp(string,"STT") == 0)
        return 0x84;
    if(strcmp(string,"STX") == 0)
        return 0x10;
    if(strcmp(string,"SUB") == 0)
        return 0x1C;
    if(strcmp(string,"SUBF") == 0)
        return 0x5C;
    if(strcmp(string,"SUBR") == 0)
        return 0x94;
    if(strcmp(string,"SVC") == 0)
        return 0xB0;
    if(strcmp(string,"TD") == 0)
        return 0xE0;
    if(strcmp(string,"TIO") == 0)
        return 0xF8;
    if(strcmp(string,"TIX") == 0)
        return 0x2C;
    if(strcmp(string,"TIXR") == 0)
        return 0xB8;
    if(strcmp(string,"WD") == 0)
        return 0xDC;
    return 0xFF;
}

void printError(char* line,uint64_t lineNumber, char* error) {
    printf("<%s>\nLine<%llu><%s>\n",line,lineNumber,error);
}

int main(uint8_t argc, char* argv[]) {
    /* SIC/XE Specifications */
    /* Memory */
    // 8-bit bytes, 3-byte words, 1048K byte limit (2^20 bytes)

    /* Registers (#) */
    // All word sized:
    // A - Accumulator, for arithmetics(0)
    // X - Index register, addressing(1)
    // L - Linkage register, stores the RA from JSUB instruction(2)
    // B - Base register, used for addressing(3)
    // S - General working register, no special use(4)
    // T - Another general register(5)
    // F - Floating-point accumulator, 48 bits(6)
    // ?? (7)
    // PC - Program Counter, contains address of next instruction to be fetched(8)
    // SW - Status Word, contains a variety of info, including Condition Code(CC)(9)

    /* Data/Instruction Formats */
    // All integers are 24-bit signed, Characters are 8-bit ascii coded, SIC has no support for floating point
    // Instruction format (24-bit): opcode(8) - x, addressing mode(1) - address(15)
    //                     (8-bit): op(8)
    //                    (16-bit): op(8) - r1(4) - r2(4)
    //                    (24-bit): op(6) - nixbpe(all 1) - disp(12)
    //                    (32-bit): op(6) - nixbpe(all 1) - address(20)
    //        Data format (48-bit): s(1), sign - exponent(11) - fraction(36)

    /* Addressing Modes */
    //  Direct; if x = 0 or (b = 0 and p = 0), TA = address
    // Indexed; if x = 1 or (b = 0 and p = 0), TA = address + (x)
    //            Base relative; if b = 1 and p = 0, TA = (B) + displacement (0 <= disp <= 4095)
    // Program-counter relative; if b = 0 and p = 1, TA = (PC) + displacement (-2048 <= disp <= 2047)
    
    /* Interrupt Classes */
    //    type    - address of work area - interruption code
    // 1. SVC     - 100 - Code from SVC instruction
    // 2. Program - 130 - Conditional
    // 3. Timer   - 160 - None
    // 4. I/O     - 190 - Channel number

    /* SVC Codes */
    //    mnemonic - register parameters
    // 0. WAIT     - (A) = address of ESB for event
    // 1. SIGNAL   - (A) = address of ESB for event
    // 2. I/O      - (A) = address of channel program
    //               (S) = channel number
    //               (T) = address of ESB for I/O operation
    // 3. REQUEST  - (T) = address of resource name
    // 4. RELEASE  - (T) = address of resource name

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
    struct { // Flags for pass 1 and 2
        uint8_t symbolFlag : 1, // this token is a symbol
                startFlag  : 1, // the program has started
                endFlag    : 1, // there's an end
                xFlag      : 1, // this instruction uses indexed addressing, can be combined with other modes
                bFlag      : 1, // this instruction might use base relative addressing
                pFlag      : 1; // this instruction might use program-counter relative addressing
    } Flags;
    Flags.symbolFlag = 0, Flags.startFlag = 0, Flags.endFlag = 0, Flags.xFlag = 0, Flags.bFlag = 0,
    Flags.pFlag = 0;
    char symbol[7], line[1024], nonNullTerminatedStringString[1024];

    while(fgets(line,1024,inputFile)) {
        // So that our error message doesn't terminate on the first string
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

        if(strlen(line) > 2) { // I guess, alternatively you could take the first token and check if it's null
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
                    char* operand = strtok(NULL," \t\r\n");

                    if(Flags.symbolFlag == 0) {
                        printError(nonNullTerminatedStringString,lineCount,"Missing symbol for START");
                        return 1;
                    }

                    if(operand == NULL) {
                        printError(nonNullTerminatedStringString,lineCount,"No argument provided for START");
                        return 1;
                    }

                    int64_t startAddress = (uint32_t)strtol(operand,NULL,16);

                    if((startAddress >= 32768) || (startAddress < 0)) {
                        printError(nonNullTerminatedStringString,lineCount,"Invalid START address");
                        return 1;

                    } else
                        address = startAddress;

                    insertNode(symbol, address);
                    // printf("%s %X\n", symbol, address);
                    Flags.symbolFlag = 0;
                    Flags.startFlag = 1;
                    break;
                }

                if((Flags.startFlag == 0) && (strcmp(token, "END") == 0)) {
                    printError(nonNullTerminatedStringString,lineCount,"Missing START");
                    return 1;
                }
                
                // print symbol and the address it's located at
                if((Flags.symbolFlag == 1) && (Flags.startFlag == 1)) {
                    insertNode(symbol, address);
                    // printf("%s %X\n", symbol, address);
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
                    char* operand = strtok(NULL," \t");
                    if((atoi(operand) > 8388607) || (atoi(operand) < -8388608)) { // 2^24 Word size
                        printError(nonNullTerminatedStringString,lineCount,"Word constant is invalid");
                        return 1;
                    }
                    if(operand != NULL) {
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
                    char* operand = strtok(NULL," \t");
                    if(operand != NULL) {
                        address+= (3*atoi(operand));
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
                    char* operand = strtok(NULL," \t");
                    if(operand != NULL) {
                        address+= atoi(operand);
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
                                    if((i % 2) == 0) {
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
                    char* operand = strtok(NULL," \t\r\n");
                    if(operand == NULL) {
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

    // Pass 2
    Flags.symbolFlag = 0, Flags.startFlag = 0, Flags.endFlag = 0, lineCount = 0;
    rewind(inputFile);
    uint16_t bytesInObjectCode = 0, bytesInTextRecord = 0, programLengthInBytes = 0;

    int length = strlen(argv[1]) + 5;
    char* outputFilename = malloc(length);
    strcpy(outputFilename,argv[1]);
    strcat(outputFilename,".obj");
    FILE* outputFile = fopen(outputFilename,"w");
    
    while(fgets(line,1024,inputFile)) {
        strcpy(nonNullTerminatedStringString,line);
        for(uint64_t i = 0; i < strlen(line); i++) {
            if(nonNullTerminatedStringString[i] == '\0')
                nonNullTerminatedStringString[i]=' ';
            else if(nonNullTerminatedStringString[i] == '\n')
                nonNullTerminatedStringString[i]='\0';
        }

        lineCount++;

        if(line[0] == '#')
            continue;

        strtok(line,",\r\n");
        char* addressingMode = strtok(NULL,",\r\n");
        if(addressingMode != NULL) {
            if(addressingMode[0] == 'X')
                Flags.xFlag = 1;
            if(addressingMode[0] == 'B')
                Flags.bFlag = 1;
            if(addressingMode[0] == 'P')
                Flags.pFlag = 1;
        }
        

        char* token = strtok(line, " \t");
        if((line[0] >= 'A') && (line[0] <= 'Z')) {
            strcpy(symbol, token);
            Flags.symbolFlag = 1;
        }

        while(token) {
            if(strcmp(token, "START") == 0) {
                char* operand = strtok(NULL, " \t\r\n");
                int64_t startAddress = (uint32_t)strtol(operand, NULL, 16);

                uint16_t programLengthInBytes = address - startAddress;
                address = startAddress;

                fprintf(outputFile, "H%-6s%06X%06X\n",symbol,address,programLengthInBytes);
                Flags.symbolFlag = 0;
                Flags.startFlag = 1;
                break;
            }

            if((Flags.symbolFlag == 0) && (Flags.startFlag == 1) && (strcmp(token,"END") != 0) && (strcmp(token,"RESB") != 0)
            && (strcmp(token,"RESW") != 0))
                fprintf(outputFile,"T%06X",address);

            char* newLinelessToken = (char*)malloc(strlen(token) + 1);
            strcpy(newLinelessToken, token);
            removeNewLine(newLinelessToken);
            uint8_t opcodeValue = getOpcodeValue(newLinelessToken);
            free(newLinelessToken);

            if(strcmp(token, "WORD") == 0) {
                char* operand = strtok(NULL," \t\r\n");
                bytesInObjectCode += 3;
                fprintf(outputFile,"%02X%06X\n",bytesInObjectCode,atoi(operand));

            } else if(strcmp(token, "RESW") == 0) {
                char* operand = strtok(NULL, " \t");
                bytesInObjectCode += (3 * atoi(operand));

            } else if(strcmp(token, "RESB") == 0) {
                char* operand = strtok(NULL, " \t");
                bytesInObjectCode += atoi(operand);

            } else if(strcmp(token, "BYTE") == 0) {
                char* byteString = strtok(NULL, "\t");
                uint64_t startOfByteString = 2;
                switch(byteString[0]) {
                    case 'C':
                        for(uint64_t i = 2; (byteString[i] != '\'') && (i < strlen(byteString)); i++) {
                            bytesInObjectCode += 1;

                            if(bytesInObjectCode == 30) {
                                if(startOfByteString != 2)
                                    fprintf(outputFile,"T%06X",address);

                                fprintf(outputFile,"%02X",bytesInObjectCode);

                                for(uint64_t j = startOfByteString; j <= i; j++) {
                                    fprintf(outputFile,"%02X",(int)byteString[j]);
                                    startOfByteString = j;
                                }

                                fprintf(outputFile,"\n");
                                address += bytesInObjectCode;
                                bytesInObjectCode = 0;
                                startOfByteString++; 
                            }
                        }

                        if(bytesInObjectCode != 0) {
                            if(startOfByteString != 2)
                                fprintf(outputFile,"T%06X%02X",address,bytesInObjectCode);
                            else
                                fprintf(outputFile,"%02X",bytesInObjectCode);

                            for(uint64_t i = startOfByteString;(byteString[i] != '\'') && (i < strlen(byteString)); i++) {
                                fprintf(outputFile,"%02X",(int)byteString[i]);
                                address++;
                                bytesInObjectCode--;
                            };
                            fprintf(outputFile,"\n");
                        }
                        break;
                    case 'X':
                        for(uint64_t i = 2; (byteString[i] != '\'') && (i < strlen(byteString)); i++) {
                            if((i % 2) == 0)
                                bytesInObjectCode += 1;

                            if(bytesInObjectCode == 30) {
                                if(startOfByteString != 2)
                                    fprintf(outputFile,"T%06X",address);
                                fprintf(outputFile,"%02X",bytesInObjectCode);
                                for(uint64_t j = startOfByteString; j <= i; j++) {
                                    fprintf(outputFile,"%c",byteString[j]);
                                    startOfByteString = j;
                                }

                                fprintf(outputFile,"\n");
                                address += bytesInObjectCode;
                                bytesInObjectCode = 0;
                                startOfByteString++; 
                            }
                        }

                        if(bytesInObjectCode != 0) {
                            if(startOfByteString != 2)
                                fprintf(outputFile,"T%06X%02X",address,bytesInObjectCode);
                            else
                                fprintf(outputFile,"%02X",bytesInObjectCode);

                            for(uint64_t i = startOfByteString;(byteString[i] != '\'') && (i < strlen(byteString)); i++)
                                fprintf(outputFile,"%c",byteString[i]);

                            address += bytesInObjectCode;
                            bytesInObjectCode = 0;
                            fprintf(outputFile,"\n");
                        }
                        break;
                }

            } else if(opcodeValue != 0xFF) {
                char* operand = strtok(NULL, " \t\r\n");
                if(operand == NULL) {
                    bytesInObjectCode += 3;
                    fprintf(outputFile,"%02X%X0000\n",bytesInObjectCode,opcodeValue);

                } else {
                struct Node* node = findNode(operand);
                if(node == NULL) {
                    fclose(outputFile);
                    remove(outputFilename);
                    printError(nonNullTerminatedStringString,lineCount,"Invalid symbol passed as an operand");
                    return 1;
                }

                uint16_t objectCodeAddress = node->address;
                if(Flags.xFlag)
                    objectCodeAddress |= 0x8000;
                bytesInObjectCode += 3;
                fprintf(outputFile,"%02X%02X%04X\n",bytesInObjectCode,opcodeValue,objectCodeAddress);
                }

            } else if(strcmp(token, "END") == 0) {
                char* operand = strtok(NULL, " \t\r\n");
                struct Node* node = findNode(operand);
                if(node == NULL) {
                    fclose(outputFile);
                    remove(outputFilename);
                    printError(nonNullTerminatedStringString,lineCount,"Invalid symbol passed as an operand");
                    return 1;
                }

                fprintf(outputFile,"E%06X",node->address);
                break;
            }

            address += bytesInObjectCode;
            if(Flags.startFlag) {
                Flags.symbolFlag = 0;
                Flags.xFlag = 0;
                bytesInObjectCode = 0;
            }
            token = strtok(NULL, " \t");
        }
    }

    fclose(outputFile);
    fclose(inputFile);
    return 0;
}