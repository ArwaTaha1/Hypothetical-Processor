#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 1024
#define MAX_LENGTH 100
void fetch();
void decode();
void execute();
int ALU(int operandA, int operandB, int operation);
void memoryInstruction();
void writeBack();

int registerFile [] = {0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
int FetchRegister = 0;
int Execute1 = 0;
int Execute2 = 0;
int Execute3 = 0;
int Execute4 = 0;

int MEM1 = 0;
int MEM2 = 0;
int MEM3 = 0;
int MEM4 = 0;

int WB1 = 0;
int WB2 = 0;
int WB3 = 0;

int PC = 0;
int instructionSize=0;
int* memory;
int clock = 1;
// int maxClock =0;

void fetch() {
    if(clock % 2 == 1){
        int instruction = 0;
        if(PC<instructionSize){
            instruction = memory[PC];
            printf("Fetch: Instruction %d\n", PC+1);
            PC++;
            if(clock > 1)
            printf("Input Instruction: %d \n", FetchRegister);
            FetchRegister = instruction;
        }
    }
    else{
        printf("Input Instruction: %d \n", FetchRegister);
    }
}
void decode(){
    if(clock > 1 ){
        if(clock % 2 == 0)
        printf("Decode: Instruction %d\n", PC);
        else
        printf("Decode: Instruction %d\n", PC-1);

    int instruction = FetchRegister;
        int opcode = (instruction & 0b11110000000000000000000000000000)>>28; 
        // printf("Opcode: %d\n", opcode);
        int desitinationRegister = (instruction & 0b00001111100000000000000000000000)>>23;
        // printf("%d dest \n",desitinationRegister);
        int firstRegister = (instruction & 0b00000000011111000000000000000000)>>18;
        int secondRegister = (instruction &0b00000000000000111110000000000000)>>13;
        int valueFirstRegister = registerFile[firstRegister];
        int valueSecondRegister = registerFile[secondRegister];
        if(opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5){
            Execute1 = opcode;
            Execute2 = valueFirstRegister;
            Execute3 = valueSecondRegister;
            Execute4 = desitinationRegister;
        }
        else if (opcode == 3 || opcode == 6 || opcode == 10 || opcode ==11 ){
            int imm = instruction& 0b00000000000000111111111111111111;
             Execute1 = opcode;
            Execute2 = valueFirstRegister;
            Execute3 = imm;
            Execute4 = desitinationRegister;
        }
        else if(opcode == 4){
            int imm = instruction& 0b00000000000000111111111111111111;
            int valueDestinationRegister = registerFile[desitinationRegister];
             Execute1 = opcode;
            Execute2 = valueFirstRegister;
            Execute3 = valueDestinationRegister;
            Execute4 = imm;
        }
        else if (opcode == 8 || opcode ==9) {
            int shamt = (instruction& 0b00000000000000000001111111111111);
             Execute1 = opcode;
            Execute2 = valueFirstRegister;
            Execute3 = shamt;
            Execute4 = desitinationRegister;
        }
        else if(opcode == 7){
            int address =  instruction & 0b00001111111111111111111111111111;
             Execute1 = opcode;
            Execute2 = address;
            Execute3 = 0;
            Execute4 = desitinationRegister;
        }
        if(clock % 2 == 0 && clock > 3){
        printf("Input Opcode: %d \n", Execute1);
        printf("Input OperandA: %d \n", Execute2);
        printf("Input OperandB: %d \n", Execute3);
        printf("Input Destination Register: %d \n", Execute4);
        }
    }
}
void execute() { 
    if(clock > 3){
        if(clock % 2 == 0)
        printf("Execute: Instruction %d\n", PC-1);
        else
        printf("Execute: Instruction %d\n", PC-2);

    int opcode = Execute1;
    int operandA = Execute2;
    int operandB = Execute3;
    int destRegister = Execute4;
    int output = 0;
    if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 
        || opcode == 6 || opcode == 8 || opcode == 9) {
        output = ALU(operandA,operandB,opcode);
        MEM1 = 0;
        MEM2 = opcode;
        MEM3 = destRegister;
        MEM4 = output;
    }
    else if (opcode ==3) {
         MEM1 = 0;
        MEM2 = opcode;
        MEM3 = destRegister;
        MEM4 = operandB;
    }
    else if (opcode == 4) {
        output = ALU(PC, destRegister, opcode);
        if (operandA == operandB && output<1024) {
            PC = output;
        }
        MEM1 = 0;
        MEM2 = opcode;
        MEM3 = 0;
        MEM4 = 0;
    }
    else if (opcode ==7) {
        int PCbits = (PC & 0b11110000000000000000000000000000);
        int jmp = PCbits | operandA;
        if (jmp<1024)
            PC = jmp;
        MEM1 = 0;
        MEM2 = opcode;
        MEM3 = 0;
        MEM4 = 0;
    }
    else if (opcode ==10) {
           output = ALU(operandA,operandB,opcode);
        MEM1 = output;
        MEM2 = opcode;
        MEM3 = destRegister;
        MEM4 = 0;
    }
    else if (opcode ==11) {
        output = ALU(operandA,operandB,opcode);
        MEM1 = output;
        MEM2 = opcode;
        MEM3 = destRegister;
        MEM4 = 0;
    }
    if(clock > 5){
    printf("Input Index: %d\n", MEM1);
    printf("Input Opcode: %d\n", MEM2);
    printf("Input Destination Register: %d\n", MEM3);
    printf("Input ALU Output: %d\n", MEM4);
    }
    }
}

void memoryInstruction(){
    if(clock > 5 && clock % 2 == 0){
    printf("Memory Access: Instruction %d\n", PC-2);
    
    int index = MEM1;
    int opcode = MEM2;
    int destRegister = MEM3;
    int output = MEM4;
   
    if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 
        || opcode == 6 || opcode == 8 || opcode == 9) {
            WB1 = destRegister;
            WB2 = output;
            WB3 = opcode;
    }
    else if (opcode == 3) {
            WB1 = destRegister;
            WB2 = output;
            WB3 = opcode;
    }
    else if (opcode == 4) {
            WB1 = 0;
            WB2 = 0;
            WB3 = opcode;
    }
    else if (opcode == 7) {
            WB1 = 0;
            WB2 = 0;
            WB3 = opcode;
    }
    else if (opcode == 10) {
        if (index>=1024 && index<2048 ){
            int value = memory[index];
            WB1 = destRegister;
            WB2 = value;
            WB3 = opcode; 
        }
    }
    else if (opcode ==11) {
    if (index>=1024 && index<2048 ){
        memory[index]= registerFile[destRegister];
            WB1 = 0;
            WB2 = 0;
            WB3 = opcode;
    }
    }
    }
    else if (clock > 6){
    printf("Input Register: %d\n", WB1);
    printf("Input Value: %d\n", WB2);
    printf("Input Opcode: %d\n", WB3);
    }
}

void writeBack() {
    if(clock > 6 && clock % 2 == 1){
    printf("Write Back: Instruction %d\n", PC-3);
   
    int reg = WB1;
    int num = WB2;
    int opcode = WB3;
    // printf("WB: %d\n, %d\n", reg, num);
        if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 
        || opcode == 6 || opcode == 8 || opcode == 9 || opcode == 3 || opcode == 10) {
        registerFile[reg] = num;
        }
    }
}

int ALU(int operandA, int operandB, int operation){
    int output = 0;

    if (operation == 0 || operation == 10 || operation == 11 || operation == 4) {
    	output = operandA + operandB;
    }
    else if (operation == 1) {
    	output = operandA - operandB;
    }
    else if (operation == 2) {
    	output = operandA * operandB;
    }
    else if (operation == 5) {
    	output = operandA & operandB;
    }
    else if (operation == 6) {
        output = operandA ^ operandB;
    }
    else if (operation == 8) {
        output = operandA << operandB;
    }
    else if (operation == 9){    
    	output = operandA >> operandB;
    }

    return output;
}

// Function to convert decimal to binary
void decimalToBinary(int n, int binary[], int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        binary[i] = n % 2;
        n /= 2;
    }
}

// Function to convert register name to number
int registerToNumber(char *reg) {
    return atoi(reg + 1);
} 

void instructions(){
    char filename[] = "instructions.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", filename);
}
    char line[MAX_LENGTH];
    int binaryInstructions[MAX_LINES];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file)) {
        int binary[32] = {0}; // Initialize binary representation for each instruction

        // Parsing instruction type
        char *token = strtok(line, " ");
        if (strcmp(token, "ADD") == 0 || strcmp(token, "SUB") == 0 || strcmp(token, "MUL") == 0 ||
            strcmp(token, "AND") == 0) {
            // R-Format instructions
            int opcode;
            if (strcmp(token, "ADD") == 0)
                opcode = 0;
            else if (strcmp(token, "SUB") == 0)
                opcode = 1;
            else if (strcmp(token, "MUL") == 0)
                opcode = 2;
            else if (strcmp(token, "AND") == 0)
                opcode = 5;
            decimalToBinary(opcode,binary,4);
            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));
            int reg3 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5); //destination = R1
            decimalToBinary(reg2, binary + 9, 5);
            decimalToBinary(reg3, binary + 14, 5);
            // char *shamt_str = strtok(NULL, " ");
            // if (shamt_str != NULL) {
            //     int shamt = atoi(shamt_str);
            //     decimalToBinary(shamt, binary + 19, 13);
            // }
        } else if(strcmp(token, "LSL") == 0 || strcmp(token, "LSR") == 0 ){
            int opcode;
            if (strcmp(token, "LSL") == 0)
                opcode = 8;
            else if (strcmp(token, "LSR") == 0)
                opcode = 9;

            decimalToBinary(opcode,binary,4);
            // Parsing registers
            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);
            decimalToBinary(reg2, binary + 9, 5);

            // Parsing shift amount if available
            char *shamt_str = strtok(NULL, " ");
            if (shamt_str != NULL) {
                int shamt = atoi(shamt_str);
                decimalToBinary(shamt, binary + 19, 13);
            }

        } else if (strcmp(token, "JEQ") == 0 || strcmp(token, "XORI") == 0 ||
            strcmp(token, "MOVR") == 0 || strcmp(token, "MOVM") == 0) {
            // I-Format instructions
            int opcode;
            if (strcmp(token, "JEQ") == 0)
                opcode = 4;
            else if (strcmp(token, "XORI") == 0)
                opcode = 6;
            else if (strcmp(token, "MOVR") == 0)
                opcode = 10;
            else if (strcmp(token, "MOVM") == 0)
                opcode = 11;

            decimalToBinary(opcode,binary,4);


            // Parsing registers
            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);
            decimalToBinary(reg2, binary + 9, 5);

            // Parsing immediate value
            int immediate = atoi(strtok(NULL, " "));
            decimalToBinary(immediate, binary + 14, 18);
        } else if(strcmp(token, "MOVI") == 0){
            int opcode;
            if (strcmp(token, "MOVI") == 0)
                opcode = 3;
                decimalToBinary(opcode,binary,4);

            // Parsing registers
            int reg1 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);

            // Parsing immediate value
            int immediate = atoi(strtok(NULL, " "));
            decimalToBinary(immediate, binary + 14, 18);

        } else if (strcmp(token, "JMP") == 0) {
            // J-Format instructions
            int opcode = 7;

            decimalToBinary(opcode,binary,4);

            // Parsing address
            int address = atoi(strtok(NULL, " "));
            decimalToBinary(address, binary + 4, 28);
        } else {
            fprintf(stderr, "Unknown instruction: %s\n", token);
            continue;
        }


        //Convert binary array to decimal representation
        int decimalInstruction = 0;
        instructionSize++;
        for (int i = 0; i < 32; i++) {
            decimalInstruction = decimalInstruction * 2 + binary[i];
        }
        memory[lineCount++] = decimalInstruction;
    }
    // maxClock = 7 + ((instructionSize-1)*2);
    fclose(file);
}



int main() {
    memory = (int*)calloc(2048, sizeof(int));
    instructions();
    while(clock < 19){
       printf("\n");
       printf("Clock Cycle: %d \n", clock);
       fetch();
       decode();
       execute();
       memoryInstruction();
       writeBack();

       clock++;
    }
    
    // for (int i =0;i<32;i++)
    //     printf("%d\n",registerFile[i]);
    return 0;
}
