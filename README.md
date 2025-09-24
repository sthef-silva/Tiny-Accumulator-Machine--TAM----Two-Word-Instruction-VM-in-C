# Tiny Accumulator Machine (TAM) — Two-Word Instruction VM in C

A small C **virtual machine** that interprets a two-word instruction format on a minimal accumulator CPU:
- **Instruction Memory (IM):** up to 129 integers (instructions are pairs: `op`, `addr`)
- **Data Memory (DM):** 10 integers
- **Registers:** `PC` (program counter, counts in *words*), `A` (accumulator), `MAR`, `MDR`

Each instruction occupies **two words** in IM: the **opcode** followed by an **address/immediate slot**.

## Instruction Set:

| Op | Mnemonic | Effect (informal)                                    | Notes                                   |
|----|----------|-------------------------------------------------------|-----------------------------------------|
| 0  | `MUL a`  | `A = A * DM[a]`                                       | Multiply by DM at address `a`           |
| 1  | `LOAD a` | `A = DM[a]`                                           | Load into accumulator                   |
| 2  | `ADD a`  | `A = A + DM[a]`                                       | Add from data memory                    |
| 3  | `STORE a`| `DM[a] = A`                                           | Store accumulator to memory             |
| 4  | `SUB a`  | `A = A - DM[a]`                                       | Subtract DM from accumulator            |
| 5  | `IN`     | Read an integer from stdin into `A`                   | `addr` field ignored                    |
| 6  | `OUT`    | Print `A`                                             | `addr` field ignored                    |
| 7  | `HALT`   | Stop execution                                        | `addr` field ignored                    |
| 8  | `JMP a`  | `PC = a * 2`                                          | Jumps to instruction index `a`          |
| 9  | `SKIPZ`  | If `A == 0`, **skip next instruction**                | Else continue normally                  |
| 10 | `SKIPG`  | If `A > 0`, **skip next instruction**                 |                                         |
| 11 | `SKIPL`  | If `A < 0`, **skip next instruction**                 |                                         |

> **PC counts words**, not instructions. That’s why `JMP` multiplies by 2.

## Program File Format:

- Plain text file.
- **Each line contains two integers**: `op addr`
  - For opcodes that don’t use `addr` `(IN, OUT, HALT)`, put `0` as a placeholder.

**Example: double the input and print it**
```txt
5 0   # IN         ; A = user input
3 0   # STORE 0    ; DM[0] = A
1 0   # LOAD 0     ; A = DM[0]
2 0   # ADD 0      ; A = A + DM[0] (double)
6 0   # OUT        ; print A
7 0   # HALT
Save this as prog.txt.

**Example Run**
$ ./tam
Enter file name: prog.txt

PC = 0 | A = 0 | DM: [0,0,0,0,0,0,0,0,0,0,]

op: 5 addr: 0

IN
input value: 7
PC = 2 | A = 7 | DM: [0,0,0,0,0,0,0,0,0,0,]

op: 3 addr: 0

STORE
PC = 4 | A = 7 | DM: [7,0,0,0,0,0,0,0,0,0,]

op: 1 addr: 0

LOAD
PC = 6 | A = 7 | DM: [7,0,0,0,0,0,0,0,0,0,]

op: 2 addr: 0

ADD
PC = 8 | A = 14 | DM: [7,0,0,0,0,0,0,0,0,0,]

op: 6 addr: 0

OUT
Accumulator value = 14 
PC = 10 | A = 14 | DM: [7,0,0,0,0,0,0,0,0,0,]

op: 7 addr: 0

HALT
Final value is: 14
Program has ended

---


## Build & Run:

### Prerequisites:
- C compiler (**gcc** or **clang**)  
- Works on **Windows, macOS, Linux** (terminal)  

### Files:
- `main.c` (the VM)  
- `prog.txt` (your program file)  

### macOS / Linux:
```bash
gcc -std=c11 -Wall -Wextra -O2 -o tam main.c
./tam
# then type the program file name when prompted, e.g., prog.txt

## Notes & Limitations:
- **File parsing assumptions:** Each line must have two space-separated integers (`op addr`).  
- **Bounds:** No explicit checks for invalid addresses (`a` should be 0–9 for DM; jumps should target valid instructions).  
- **IM size & PC:** IM is sized 129 ints; execution stops when `HALT` executes or `PC > 128`.  
- **Two-word instructions:** `PC` advances by 2 after most instructions; *skip* opcodes add 4 when the condition holds (skip one instruction pair).  

## Known Rough Edges (Quick Fixes Suggested)
- In the read loop, change:  
  ```c
  fgets(CodefileBuffer, sizeof(SIZE), Codefile)
    to:
  fgets(CodefileBuffer, sizeof(CodefileBuffer), Codefile)

- Validate malloc/fopen/addresses and handle malformed lines gracefully.

- Consider trimming whitespace & supporting multi-digit fields robustly.

## Possible Enhancements
- Add **input validation** & **bounds checks** for `addr` and `PC`.  
- Support **labels** and an **assembler** (translate symbolic code to `op addr`).  
- Extend **DM beyond 10 cells**; support **immediates** (e.g., `LOADI` / `ADDI`).  
- Add a **step mode** and richer **state dump** (IM window, flags).  
- Provide **error messages with line numbers** for bad programs.  


### Description & Skill Highlights:

**Skills:**
- Implemented **instruction fetch–decode–execute** with a **switch-based opcode dispatcher**.  
- Modeled **instruction/data memory** and **registers**; maintained `PC` in a two-word ISA.  
- Built **file parsing** to load programs (`op addr` per line) and executed with **debug state prints**.  
- Considered **robustness & UX** (bounds, malformed input, stepwise tracing) and outlined extensions (assembler, labels, immediates).

**Tech:** C, gcc/clang, file I/O, arrays, state machines, CLI.
