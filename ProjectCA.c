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

//int registerFile[] = {0, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int registerFile [32] = {0};

int indexStart = 0;

int FetchIndex = 0;
int FetchRegister[1024] = {0};

int Execute1[1024] = {0};
int Execute2[1024] = {0};
int Execute3[1024] = {0};
int Execute4[1024] = {0};
int ExecuteIndex = 0;

int MEM1[1024] = {0};
int MEM2[1024] = {0};
int MEM3[1024] = {0};
int MEM4[1024] = {0};
int MemIndex = 0;

int WB1[1024] = {0};
int WB2[1024] = {0};
int WB3[1024] = {0};
int WbIndex = 0;

int PC = 0;
int instructionSize = 0;
int *memory;
int clock = 1;

int executeFlag = 0;
int memoryFlag = 0;
int writeFlag = 0;
int counterE = 0;
int counterM = 0;
int counterW = 0;
int doneFlag = 0;

void fetch()
{
    if (clock % 2 == 1)
    {
        int instruction = 0;
        if (PC < instructionSize)
        {
            instruction = memory[PC];
            PC++;
            printf("Fetch: Instruction %d\n", PC);
            FetchRegister[indexStart] = instruction;
            indexStart++;
        }
    }
}

void decode()
{
    if (FetchIndex < instructionSize)
    {
        if (clock > 1)
        {
            printf("Decode: Instruction %d\n", FetchIndex + 1);
            printf("Input to decode: \n");
            printf("-Instruction: %d \n", FetchRegister[FetchIndex]);

            int instruction = FetchRegister[FetchIndex];
            int opcode = (instruction & 0b11110000000000000000000000000000) >> 28;
            // printf("Opcode: %d\n", opcode);
            int desitinationRegister = (instruction & 0b00001111100000000000000000000000) >> 23;
            // printf("%d dest \n",desitinationRegister);
            int firstRegister = (instruction & 0b00000000011111000000000000000000) >> 18;
            int secondRegister = (instruction & 0b00000000000000111110000000000000) >> 13;
            int valueFirstRegister = registerFile[firstRegister];
            int valueSecondRegister = registerFile[secondRegister];
            if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5)
            {
                Execute1[FetchIndex] = opcode;
                Execute2[FetchIndex] = valueFirstRegister;
                Execute3[FetchIndex] = valueSecondRegister;
                Execute4[FetchIndex] = desitinationRegister;
            }
            else if (opcode == 3 || opcode == 6 || opcode == 10 || opcode == 11)
            {
                int imm = instruction & 0b00000000000000111111111111111111;
                Execute1[FetchIndex] = opcode;
                Execute2[FetchIndex] = valueFirstRegister;
                Execute3[FetchIndex] = imm;
                Execute4[FetchIndex] = desitinationRegister;
            }
            else if (opcode == 4)
            {
                int imm = instruction & 0b00000000000000111111111111111111;
                int valueDestinationRegister = registerFile[desitinationRegister];
                Execute1[FetchIndex] = opcode;
                Execute2[FetchIndex] = valueFirstRegister;
                Execute3[FetchIndex] = valueDestinationRegister;
                Execute4[FetchIndex] = imm;
            }
            else if (opcode == 8 || opcode == 9)
            {
                int shamt = (instruction & 0b00000000000000000001111111111111);
                Execute1[FetchIndex] = opcode;
                Execute2[FetchIndex] = valueFirstRegister;
                Execute3[FetchIndex] = shamt;
                Execute4[FetchIndex] = desitinationRegister;
            }
            else if (opcode == 7)
            {
                int address = instruction & 0b00001111111111111111111111111111;
                Execute1[FetchIndex] = opcode;
                Execute2[FetchIndex] = address;
                Execute3[FetchIndex] = 0;
                Execute4[FetchIndex] = desitinationRegister;
            }
            if (clock % 2 == 1)
            {
                FetchIndex++;
            }
        }
    }
}

void execute()
{
    if (ExecuteIndex < instructionSize)
    {
        if (clock > 3 && executeFlag == 0)
        {
            printf("Execute: Instruction %d\n", ExecuteIndex + 1);
            printf("Input to execute: \n");

            int opcode = Execute1[ExecuteIndex];
            int operandA = Execute2[ExecuteIndex];
            int operandB = Execute3[ExecuteIndex];
            int destRegister = Execute4[ExecuteIndex];
            int output = 0;
            if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 || opcode == 6 || opcode == 8 || opcode == 9)
            {
                output = ALU(operandA, operandB, opcode);
                MEM1[ExecuteIndex] = 0;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = destRegister;
                MEM4[ExecuteIndex] = output;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-OperandA: %d \n", Execute2[ExecuteIndex]);
            printf("-OperandB: %d \n", Execute3[ExecuteIndex]);
            printf("-Destination Register: %d \n", Execute4[ExecuteIndex]);
            }
            else if (opcode == 3)
            {
                MEM1[ExecuteIndex] = 0;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = destRegister;
                MEM4[ExecuteIndex] = operandB;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-Immediate Value: %d \n", Execute3[ExecuteIndex]);
            printf("-Destination Register: %d \n", Execute4[ExecuteIndex]);
            }
            else if (opcode == 4)
            {
                output = ALU(PC, destRegister, opcode);
                if (operandA == operandB && output < 1024)
                {
                    if (clock % 2 == 1)
                    {
                        PC = output - 2;
                        indexStart = PC;
                        executeFlag = 1;
                    }
                }
                MEM1[ExecuteIndex] = 0;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = 0;
                MEM4[ExecuteIndex] = destRegister;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-OperandA: %d \n", Execute2[ExecuteIndex]);
            printf("-OperandB: %d \n", Execute3[ExecuteIndex]);
            printf("-Immediate Value: %d \n", Execute4[ExecuteIndex]);
            }
            else if (opcode == 7)
            {
                int PCbits = (PC & 0b11110000000000000000000000000000);
                int jmp = PCbits | operandA;
                if (jmp < 1024)
                {
                    if (clock % 2 == 1)
                    {
                        PC = jmp - 1;
                        indexStart = jmp - 1;
                        executeFlag = 1;
                    }
                }
                MEM1[ExecuteIndex] = 0;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = 0;
                MEM4[ExecuteIndex] = jmp;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-Address: %d \n", Execute2[ExecuteIndex]);
            }
            else if (opcode == 10)
            {
                output = ALU(operandA, operandB, opcode);
                MEM1[ExecuteIndex] = output;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = destRegister;
                MEM4[ExecuteIndex] = 0;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-OperandA: %d \n", Execute2[ExecuteIndex]);
            printf("-OperandB: %d \n", Execute3[ExecuteIndex]);
            printf("-Destination Register: %d \n", Execute4[ExecuteIndex]);
            }
            else if (opcode == 11)
            {
                output = ALU(operandA, operandB, opcode);
                MEM1[ExecuteIndex] = output;
                MEM2[ExecuteIndex] = opcode;
                MEM3[ExecuteIndex] = destRegister;
                MEM4[ExecuteIndex] = 0;
            printf("-Opcode: %d \n", Execute1[ExecuteIndex]);
            printf("-OperandA: %d \n", Execute2[ExecuteIndex]);
            printf("-OperandB: %d \n", Execute3[ExecuteIndex]);
            printf("-Destination Register: %d \n", Execute4[ExecuteIndex]);
            }
            if (clock % 2 == 1)
            {
                ExecuteIndex++;
            }
        }
        else if (clock > 6)
        {
            counterE++;
            if (counterE == 2)
            {
                if (Execute1[ExecuteIndex - 1] == 4)
                {
                    FetchIndex = indexStart - 1;
                }
                else if (Execute1[ExecuteIndex - 1] == 7)
                {
                    FetchIndex = Execute2[ExecuteIndex - 1] - 1;
                }
            }
            if (counterE == 4)
            {
                if (Execute1[ExecuteIndex - 1] == 4)
                {
                    ExecuteIndex = ExecuteIndex + Execute4[ExecuteIndex - 1];
                }
                else if (Execute1[ExecuteIndex - 1] == 7)
                {
                    ExecuteIndex = Execute2[ExecuteIndex - 1] - 1;
                }
                executeFlag = 0;
                counterE = 0;
            }
        }
    }
}

void memoryInstruction()
{
    if (MemIndex < instructionSize)
    {
        if (clock > 5 && clock % 2 == 0 && memoryFlag == 0)
        {
            printf("Memory Access: Instruction %d\n", MemIndex + 1);

            int index = MEM1[MemIndex];
            int opcode = MEM2[MemIndex];
            int destRegister = MEM3[MemIndex];
            int output = MEM4[MemIndex];

            if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 || opcode == 6 || opcode == 8 || opcode == 9)
            {
                WB1[MemIndex] = destRegister;
                WB2[MemIndex] = output;
                WB3[MemIndex] = opcode;
            }
            else if (opcode == 3)
            {
                WB1[MemIndex] = destRegister;
                WB2[MemIndex] = output;
                WB3[MemIndex] = opcode;
            }
            else if (opcode == 4)
            {
                WB1[MemIndex] = 0;
                WB2[MemIndex] = output;
                WB3[MemIndex] = opcode;
                memoryFlag = 1;
            }
            else if (opcode == 7)
            {
                WB1[MemIndex] = output;
                WB2[MemIndex] = 0;
                WB3[MemIndex] = opcode;
                memoryFlag = 1;
            }
            else if (opcode == 10)
            {
                if (index >= 1024 && index < 2048)
                {
                    int value = memory[index];
                    WB1[MemIndex] = destRegister;
                    WB2[MemIndex] = value;
                    WB3[MemIndex] = opcode;
            printf("Input to memory access: \n");
            printf("-Opcode: %d\n", MEM2[MemIndex]);
            printf("-Memory Index: %d\n", MEM1[MemIndex]);
            printf("-Destination Register: %d\n", MEM3[MemIndex]);
                }
            }
            else if (opcode == 11)
            {
                if (index >= 1024 && index < 2048)
                {
                    memory[index] = registerFile[destRegister];
                    WB1[MemIndex] = 0;
                    WB2[MemIndex] = 0;
                    WB3[MemIndex] = opcode;
            printf("Input to memory access: \n");
            printf("-Opcode: %d\n", MEM2[MemIndex]);
            printf("-Memory Index: %d\n", MEM1[MemIndex]);
            printf("-Input Register: %d\n", MEM3[MemIndex]);
                }
            }
            MemIndex++;
        }
        else if (clock > 5 && clock % 2 == 0)
        {
            counterM++;
            if (counterM == 2)
            {
                if (MEM2[MemIndex - 1] == 4)
                {
                    MemIndex = MemIndex + MEM4[MemIndex - 1];
                }
                else if (MEM2[MemIndex - 1] == 7)
                {
                    MemIndex = MEM4[MemIndex - 1] - 1;
                }
                memoryFlag = 0;
                counterM = 0;
            }
        }
    }
}

void writeBack()
{
    if (WbIndex < instructionSize)
    {
        if (clock > 6 && clock % 2 == 1 && writeFlag == 0)
        {
            printf("Write Back: Instruction %d\n", WbIndex + 1);

            int reg = WB1[WbIndex];
            int num = WB2[WbIndex];
            int opcode = WB3[WbIndex];
            // printf("WB: %d\n, %d\n", reg, num);
            if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 5 || opcode == 6 || opcode == 8 || opcode == 9 || opcode == 3 || opcode == 10)
            {
                if (reg != 0)
                    registerFile[reg] = num;
                    
            printf("Input to write back: \n");
            printf("-Opcode: %d\n", WB3[WbIndex]);
            printf("-Register: %d\n", WB1[WbIndex]);
            printf("-Value: %d\n", WB2[WbIndex]);
            }
            else if (opcode == 4 || opcode == 7)
            {
                writeFlag = 1;
            }
            WbIndex++;
        }
        else if (clock > 6 && clock % 2 == 1)
        {
            counterW++;
            if (counterW == 2)
            {
                if (WB3[WbIndex - 1] == 4)
                {
                    WbIndex = WbIndex + WB2[WbIndex - 1];
                }
                else if (WB3[WbIndex - 1] == 7)
                {
                    WbIndex = WB1[WbIndex - 1] - 1;
                }

                writeFlag = 0;
                counterW = 0;
            }
        }
    }
    if (WbIndex >= instructionSize)
    {
        doneFlag = 1;
    }
}

int ALU(int operandA, int operandB, int operation)
{
    int output = 0;

    if (operation == 0 || operation == 10 || operation == 11 || operation == 4)
    {
        output = operandA + operandB;
    }
    else if (operation == 1)
    {
        output = operandA - operandB;
    }
    else if (operation == 2)
    {
        output = operandA * operandB;
    }
    else if (operation == 5)
    {
        output = operandA & operandB;
    }
    else if (operation == 6)
    {
        output = operandA ^ operandB;
    }
    else if (operation == 8)
    {
        output = operandA << operandB;
    }
    else if (operation == 9)
    {
        output = operandA >> operandB;
    }

    return output;
}

void decimalToBinary(int n, int binary[], int bits)
{
    for (int i = bits - 1; i >= 0; i--)
    {
        binary[i] = n % 2;
        n /= 2;
    }
}

int registerToNumber(char *reg)
{
    return atoi(reg + 1);
}

void instructions()
{
    char filename[] = "instructions.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
    }
    char line[MAX_LENGTH];
    int binaryInstructions[MAX_LINES];
    int lineCount = 0;

    while (fgets(line, sizeof(line), file))
    {
        int binary[32] = {0};

        char *token = strtok(line, " ");
        if (strcmp(token, "ADD") == 0 || strcmp(token, "SUB") == 0 || strcmp(token, "MUL") == 0 ||
            strcmp(token, "AND") == 0)
        {
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
            decimalToBinary(opcode, binary, 4);
            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));
            int reg3 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5); 
            decimalToBinary(reg2, binary + 9, 5);
            decimalToBinary(reg3, binary + 14, 5);
        }
        else if (strcmp(token, "LSL") == 0 || strcmp(token, "LSR") == 0)
        {
            int opcode;
            if (strcmp(token, "LSL") == 0)
                opcode = 8;
            else if (strcmp(token, "LSR") == 0)
                opcode = 9;

            decimalToBinary(opcode, binary, 4);
            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);
            decimalToBinary(reg2, binary + 9, 5);

            char *shamt_str = strtok(NULL, " ");
            if (shamt_str != NULL)
            {
                int shamt = atoi(shamt_str);
                decimalToBinary(shamt, binary + 19, 13);
            }
        }
        else if (strcmp(token, "JEQ") == 0 || strcmp(token, "XORI") == 0 ||
                 strcmp(token, "MOVR") == 0 || strcmp(token, "MOVM") == 0)
        {
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

            decimalToBinary(opcode, binary, 4);

            int reg1 = registerToNumber(strtok(NULL, " "));
            int reg2 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);
            decimalToBinary(reg2, binary + 9, 5);

            int immediate = atoi(strtok(NULL, " "));
            decimalToBinary(immediate, binary + 14, 18);
        }
        else if (strcmp(token, "MOVI") == 0)
        {
            int opcode;
            if (strcmp(token, "MOVI") == 0)
                opcode = 3;
            decimalToBinary(opcode, binary, 4);

            int reg1 = registerToNumber(strtok(NULL, " "));

            decimalToBinary(reg1, binary + 4, 5);

            int immediate = atoi(strtok(NULL, " "));
            decimalToBinary(immediate, binary + 14, 18);
        }
        else if (strcmp(token, "JMP") == 0)
        {
            // J-Format instructions
            int opcode = 7;

            decimalToBinary(opcode, binary, 4);

            int address = atoi(strtok(NULL, " "));
            decimalToBinary(address, binary + 4, 28);
        }
        else
        {
            fprintf(stderr, "Unknown instruction: %s\n", token);
            continue;
        }

        int decimalInstruction = 0;
        instructionSize++;
        for (int i = 0; i < 32; i++)
        {
            decimalInstruction = decimalInstruction * 2 + binary[i];
        }

        memory[lineCount++] = decimalInstruction;
    }
    fclose(file);
}

int main()
{
    memory = (int *)calloc(2048, sizeof(int));
    instructions();
    while (doneFlag == 0)
    {
        printf("Clock Cycle: %d \n", clock);
        fetch();
        decode();
        execute();
        memoryInstruction();
        writeBack();
        printf("\n");
        clock++;
    }

    printf("\n");
    printf("Registers \n");
        printf("PC: %d \n", PC);
    for (int i = 0; i < 32; i++)
        printf("Register: R%d, Value: %d\n ", i, registerFile[i]);

    printf("\n");
    printf("Memory \n");
    for (int i = 0; i < 2048; i++)
    {
        if (i == 0)
            printf("Instructions: \n");
        if (i == 1024)
            printf("\nData: \n");

        printf("%d:%d, ", i, memory[i]);

        if(i % 10 == 0 && i<1024 && i != 0)
             printf("\n");

        if(i % 15 == 0 && i>1023)
             printf("\n");
    }
    printf("\n");
    return 0;
}