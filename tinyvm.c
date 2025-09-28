#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ===== Config / Limits ===== */
#define IM_SIZE 129   /* instruction memory holds op/addr pairs (flat int array) */
#define DM_SIZE 10    /* data memory size */

/* ===== Machine State ===== */
typedef struct {
    int op;
    int addr;
} IR_t;

static int PC  = 0;                 /* program counter (index into IM; advances by 2) */
static int AC  = 0;                 /* accumulator */
static int MAR = 0;                 /* memory address register (for DM) */
static int MDR = 0;                 /* memory data register (for DM) */
static int IM[IM_SIZE] = {0};       /* instruction memory */
static int DM[DM_SIZE] = {0};       /* data memory */
static int vmstate = 0;             /* 0 = don't print state; 1 = print state */

/* ===== Utilities ===== */
static void print_state(void) {
    int i;
    printf("\nPC=%d | AC=%d | DM=[", PC, AC);
    for (i = 0; i < DM_SIZE; i++) {
        printf("%d", DM[i]);
        if (i < DM_SIZE - 1) printf(", ");
    }
    printf("]\n");
}

static int yes_response(const char *s) {
    /* Treat "yes", "y", "Y", "Yes" as yes (case-insensitive) */
    if (!s || !*s) return 0;
    if ((s[0] == 'y' || s[0] == 'Y') && (s[1] == '\0' || s[1] == '\n' || s[1] == 'e' || s[1] == 'E'))
        return 1;
    return strcasecmp ? (strcasecmp(s, "yes") == 0) : 0; /* if strcasecmp exists */
}

/* Load program: each non-blank line must contain "op addr" (two ints) */
static int load_program(FILE *fp) {
    char line[128];
    int im_i = 0;

    /* start clean */
    memset(IM, 0, sizeof(IM));

    while (fgets(line, sizeof line, fp)) {
        int op, addr;

        /* skip blank lines and comment lines */
        const char *p = line;
        while (isspace((unsigned char)*p)) p++;
        if (*p == '\0' || *p == '#' || *p == ';' || (*p == '/' && p[1] == '/')) {
            continue;
        }

        if (sscanf(line, "%d %d", &op, &addr) == 2) {
            if (im_i + 1 >= IM_SIZE) {
                fprintf(stderr, "Program too large for IM_SIZE=%d\n", IM_SIZE);
                exit(1);
            }
            IM[im_i++] = op;
            IM[im_i++] = addr;
        } else {
            fprintf(stderr, "Ignoring malformed line: %s", line);
        }
    }

    return im_i; /* count of ints written into IM (should be even) */
}

/* Safe DM read */
static int DM_read(int addr) {
    if (addr < 0 || addr >= DM_SIZE) {
        fprintf(stderr, "DM read OOB at %d\n", addr);
        exit(1);
    }
    return DM[addr];
}

/* Safe DM write */
static void DM_write(int addr, int value) {
    if (addr < 0 || addr >= DM_SIZE) {
        fprintf(stderr, "DM write OOB at %d\n", addr);
        exit(1);
    }
    DM[addr] = value;
}

/* ===== Main ===== */
int main(void) {
    char fname[256];
    char answer[32];

    /* Optional: constant 1 at DM[9], if your programs assume it */
    memset(DM, 0, sizeof(DM));
    if (DM_SIZE > 9) DM[9] = 1;

    printf("Enter file name: ");
    if (scanf("%255s", fname) != 1) {
        fprintf(stderr, "No filename provided.\n");
        return 1;
    }

    /* consume the rest of the line before asking yes/no */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { /* flush */ }

    printf("Would you like to see the state of the VM when executing the program? (yes/no): ");
    if (fgets(answer, sizeof answer, stdin)) {
        /* strip newline */
        size_t L = strlen(answer);
        if (L && answer[L-1] == '\n') answer[L-1] = '\0';
        vmstate = yes_response(answer);
    }

    FILE *fp = fopen(fname, "r");
    if (!fp) {
        perror("open");
        return 1;
    }

    int im_count = load_program(fp);
    fclose(fp);

    if (im_count % 2 != 0) {
        fprintf(stderr, "Warning: instruction stream has odd length; last addr missing?\n");
        /* We’ll still run until PC+1 < im_count */
    }

    if (vmstate) print_state();

    /* ===== Execute =====
       IM layout: [op0, addr0, op1, addr1, ...]
       PC indexes IM; fetch reads IM[PC], IM[PC+1]; then PC += 2.
    */
    PC = 0;
    while (PC + 1 < im_count && PC <= IM_SIZE - 2) {
        IR_t ir;
        ir.op   = IM[PC];
        ir.addr = IM[PC + 1];
        PC += 2;

        switch (ir.op) {
            case 1: { /* LOAD */
                printf("\nLOAD\n");
                MAR = ir.addr;
                MDR = DM_read(MAR);
                AC  = MDR;
                if (vmstate) print_state();
            } break;

            case 2: { /* ADD */
                printf("\nADD\n");
                MAR = ir.addr;
                MDR = DM_read(MAR);
                AC += MDR;
                if (vmstate) print_state();
            } break;

            case 3: { /* STORE */
                printf("\nSTORE\n");
                MAR = ir.addr;
                MDR = AC;
                DM_write(MAR, MDR);
                if (vmstate) print_state();
            } break;

            case 4: { /* SUB */
                printf("\nSUB\n");
                MAR = ir.addr;
                MDR = DM_read(MAR);
                AC -= MDR;
                printf("Subtracting %d from AC, from address %d\n", MDR, MAR);
                if (vmstate) print_state();
            } break;

            case 5: { /* IN */
                printf("\nIN\n");
                printf("input value: ");
                if (scanf("%d", &AC) != 1) {
                    fprintf(stderr, "Invalid input.\n");
                    return 1;
                }
                if (vmstate) print_state();
            } break;

            case 6: { /* OUT */
                printf("\nOUT\n");
                printf("Final value is: %d\n", AC);
                if (vmstate) print_state();
            } break;

            case 7: { /* HALT */
                printf("\nHALT\n");
                printf("Program has ended\n");
                if (vmstate) print_state();
                return 0;
            }

            case 8: { /* JMP */
                printf("\nJMP\n");
                /* Expect addr to be a valid IM index (even) */
                if (ir.addr < 0 || ir.addr >= im_count) {
                    fprintf(stderr, "JMP target OOB: %d\n", ir.addr);
                    return 1;
                }
                PC = ir.addr;
                if (vmstate) print_state();
            } break;

            case 9: { /* SKIPZ (skip next instruction pair if AC == 0) */
                printf("\nSKIPZ\n");
                if (AC == 0) {
                    PC += 2;
                }
                if (vmstate) print_state();
            } break;

            case 10: { /* SKIPG (skip if AC > 0) */
                printf("\nSKIPG\n");
                if (AC > 0) {
                    PC += 2;
                }
                if (vmstate) print_state();
            } break;

            case 11: { /* SKIPL (your original logic treated AC<0 as invalid) */
                printf("\nSKIPL\n");
                if (AC < 0) {
                    printf("\nINVALID INPUT\n");
                    printf("Final value = 0\n");
                    return 0;
                }
                if (vmstate) print_state();
            } break;

            default:
                fprintf(stderr, "Unknown opcode %d at PC=%d\n", ir.op, PC - 2);
                return 1;
        }
    }

    /* If we fall out of the loop without HALT */
    printf("\nProgram terminated without HALT (PC=%d).\n", PC);
    if (vmstate) print_state();
    return 0;
}
