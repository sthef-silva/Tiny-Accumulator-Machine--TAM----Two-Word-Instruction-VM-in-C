
/*
NAMES
Sthefanny Silva
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
struct IR {
  int op;
  int addr;
};

typedef struct IR IR;
int PC = 0;
int a = 0;
int MAR = 0;
int IM[129];
int DM[10];
int MDR = 0;
int SIZE = 0;
int vmstate = 0; // Variable to hold the state of the VM (0 for "no" and 1 for "yes")

// Function to print the state of the machine
void print() {
  int i;
  printf("\nPC = %d | A = %d | DM = [", PC, a);

  // Print data into memory
  for (i = 0; i < 10; i++) {
    printf("%d", DM[i]);
    if (i < 9) {
      printf(", ");
    }
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
  char response[5]; // User response buffer
  printf("Enter file name: ");
  scanf("%s", fname);

  // Ask the user if they want to see the state of the VM during execution
  printf("Would you like to see the state of the VM when executing the program? (yes/no): ");
  scanf("%s", response);

  // Check the user's response and set the vmstate variable accordingly
  if (strcmp(response, "yes") == 0) {
    vmstate = 1; // Set vmstate to 1 if the user wants to see the state
  }

  // Opening and reading the file
  FILE *Codefile = fopen(fname, "r");
  SIZE = getNumberOfLineInFile(fopen(fname, "r"));

  char CodefileBuffer[10];
  int i = 0;

  
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
DM[9] = 1;
  // Need to close the Codefile
  fclose(Codefile);

  // Function to print outputs
  if (vmstate)
    print();

  // While loop to execute the instructions
  while (PC <= 128) {
    // Fetch
    IR.op = IM[PC];
    IR.addr = IM[PC + 1];
    PC+=2;
    switch (IR.op) {
      case 1: // Load
        printf("\nLOAD\n");
        MAR = IR.addr;
        MDR = DM[MAR];
        a = MDR;
        
        if (vmstate)
          print();
        break;
      case 2: // Add
        printf("\nADD\n");
        MAR = IR.addr;
        MDR = DM[MAR];
        a += MDR;
        
        if (vmstate)
          print();
        break;
      case 3: // Store
        printf("\nSTORE\n");
        MAR = IR.addr;
        MDR = a;
        DM[MAR] = MDR;
        if (vmstate)
          print();
        break;
      case 4: // Sub
        printf("\nSUB\n");
        MAR = IR.addr;
        MDR = DM[MAR];
        a -= MDR;
        printf("Subtracting %d from AC, from address %d", MDR, MAR);
        if (vmstate)
          print();
        break;
      case 5: // Input
        printf("\nIN\n");
        printf("input value: ");
        scanf("%d", &a);
        
        if (vmstate)
          print();
        break;
      case 6: // Out
        printf("\nOUT\n");
        printf("Final value is: %d\n", a); // Show final value at the end of the program
        if (vmstate)
          print();
        break;
      case 7: // Halt
        printf("\nHALT\n");
        printf("Program has ended\n");
        if (vmstate)
          print();
        exit(0);
      case 8: // Jmp
        printf("\nJMP\n");
        PC = IR.addr;
        if (vmstate)
          print();
        break;
      case 9: // SkipZ
        printf("\nSKIPZ\n");
        if (a == 0)
          PC += 2;
        else
          PC += 0;
        if (vmstate)
          print();
        break;
      case 10:
        printf("\nSKIPG\n");
        if (a > 0){
          PC += 2;
        }
        if (vmstate)
          print();
        break;
      case 11:
        printf("\nSKIPL\n");
        if (a < 0){
          printf("\nINVALID INPUT\n");
          printf("\nFinal value = 0\n");
          exit(0); // End the program
        }
        if (vmstate)
          print();
        break;
      default:
        printf("Error while reading the file\n");
        exit(0);
    }
  }
}
