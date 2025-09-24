#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct IR {
  int op;
  int addr;
};
typedef struct IR IR;

int pc = 0;
int a = 0;
int mar = 0;
int IM[129];
int DM[10];
int mdr = 0;
int SIZE = 0;
int flag = 0;

// Function to print 
void print() {
  int i;
  printf("\nPC = %d | A = %d | DM: [", pc, a);

  // Print data into memory
  for (i = 0; i < 10; i += 1) {
    printf("%d,", DM[i]);
  }
  printf("]\n");
}

// Function to get the number of lines in the file
int getNumberOfLineInFile(FILE *Codefile) {
  char asmCodeBuffer[10];
  int count = 0;

  if (Codefile == NULL) {
    printf("File is not opening");
    exit(0);
  }

  // Calculate the number of lines
  while (fgets(asmCodeBuffer, 10, Codefile) != NULL)
    count++; // Increment count

  fclose(Codefile);
  return count; // Return count
}

// Main
int main() {
  IR IR;
  char fname[20]; // File name holder
  printf("Enter file name: ");
  scanf("%s", fname);

  // Opening and reading the file
  FILE *Codefile = fopen(fname, "r");
  SIZE = getNumberOfLineInFile(fopen(fname, "r"));

  char CodefileBuffer[10];
  int i = 0;

  // Catching if the file is null 
  if (Codefile == NULL) {
    printf("Executable Linkable File is not opening...");
    exit(0);
  }

  // Read lines from the input file and check whether each line contains two values
  while (fgets(CodefileBuffer, sizeof(SIZE), Codefile) != NULL) {
    // Check if it contains two digits or not
    if (isdigit(CodefileBuffer[0]) && (int)CodefileBuffer[2] != 0) {
      IM[i] = atoi(&CodefileBuffer[0]);
      i++;
      IM[i] = atoi(&CodefileBuffer[2]);
      i++;
    }
  }

  // Need to close the Codefile
  fclose(Codefile);

  // Function to print outputs
  print();

  // While loop to execute the instructions
  while (pc <= 128) {
    // Fetch
    IR.op = IM[pc];
    IR.addr = IM[pc + 1];
    printf("\nop: %d addr: %d\n", IR.op, IR.addr);
    switch(IR.op){
      case 0:
        printf("\nMUL");
        mar = IR.addr;
        mdr = DM[mar];
        a *= mdr;
        print();
        pc += 2;
        break;
    }
    switch (IR.op) {
      case 1: // Load
        printf("\nLOAD");
        mar = IR.addr;
        mdr = DM[mar];
        a = mdr;
        print();
        pc += 2;
        break;
      case 2: // Add
        printf("\nADD");
        mar = IR.addr;
        mdr = DM[mar];
        a += mdr;
        print();
        pc += 2;
        break;
      case 3: // Store
        printf("\nSTORE");
        mar = IR.addr;
        mdr = a;
        DM[mar] = mdr;
        print();
        pc += 2;
        break;
      case 4: // Sub
        printf("\nSUB");
        mar = IR.addr;
        mdr = DM[mar];
        a -= mdr;
        printf("Subtracting %d from AC, from address %d", mdr, mar);
        print();
        pc += 2;
        break;
      case 5: // Input
        printf("\nIN\n");
        printf("input value: ");
        scanf("%d", &a);
        print();
        pc += 2;
        break;
      case 6: // Out
        printf("\nOUT\n");
        printf("Accumulator value = %d ", a);
        print();
        pc += 2;
        break;
      case 7: // Halt
        printf("\nHALT\n");
        printf("Final value is: %d\n", a); // Show final value at the end of the program
        printf("Program has ended\n");
        exit(0);
      case 8: // Jmp
        printf("\nJMP");
        pc = IR.addr * 2; // addr*2 bc can't be a double-digit
        flag = 1;
        print();
        break;
      case 9: // SkipZ
        printf("\nSKIPZ");
        if (a == 0)
          pc += 4;
        else
          pc += 2;
        print();
        break;
      case 10: //SkipG
        printf("\nSKIPG");
        if (a > 0)
          pc += 4;
        else
          pc += 2;
        print();
        break;
      case 11: //SkipL
        printf("\nSKIPL");
        if (a < 0)
          pc += 4;
        else
          pc += 2;
        print();
        break;
      default:
        printf("Error while reading the file");
        exit(0);
    }
  }
  printf("Program has ended");
  return 0;
}
