# Assignment 2 - Linking Loader
Ridhiman Dhindsa, 210101088  
Date: 12 March 2024  

## Files included
**SIC/XE Assembler**  
Inputs: *assembler_input.txt, opcodes.txt, symbols.txt, assembler.cpp*  
Outputs: *symtab.txt, intermediate.txt*  

**Linking Loader**  
Inputs: *intermediate.txt, symtab.txt, optab.txt, linking_loader.cpp*  
Outputs: *assembler_output.txt, assembly_listing.txt*

## Steps to run
#### SIC/XE Assembler:
The CPP file `assembler.cpp` takes inputs from `assembler_input.txt`, OPTAB reference from 
`opcodes.txt` and register name references from `symbols.txt`. On running pass 1, it creates 2 output files: `symtab.txt` and `intermediate.txt`
1) Open terminal in the directory containing the file `assembler.cpp`  
2) Run the command `g++ assembler.cpp -o assembler`
3) Run the command `./assembler assembler_input.txt`  

#### Linking Loader:
The CPP file takes inputs from `intermediate.txt`, SYMTAB reference from `symtab.txt` and OPTAB reference from `optab.txt`. On running pass 2, it creates 2 output files: `assembler_output.txt` and `assembly_listing.txt`
1) Open terminal in the directory containing the file `assembler_pass2.cpp`  
2) Run the command `g++ assembler_pass2.cpp -o assembler_pass2`
3) Run the command `./assembler_pass2` 

## Description
Developed a C++ program to implement a 2 pass assembler for SIC (Simple Instructional Computer) machine assembly language. It converts assembly language code to machine code.
The Instruction Set includes: LDA, LDX, LDL, STA, STX, STL, LDCH, STCH, ADD, SUB, MUL, DIV, COMP, J, JLT, JEQ, JGT, JSUB, RSUB, TIX, TD, RD, WD, RESW, RESB, WORD, BYTE.  

Here RESW, RESB, WORD, BYTE are **Assembler Directives**, hence they are not stored in the opcode table (OPTAB). Instead they are dealt with separately.   

In pass 1 of the assembler operation, input consisting of 3 columns is taken, namely- LABEL, OPCODE, OPERAND. This information is used to generate the addresses (LOC) for each line. Also the SYMTAB is prepared in this pass by storing labels and the corresponding addresses they refer to. Program size = (LOCCTR - starting address) is also stored in symtab.  

In pass 2 of the assembler operation, input input consisting of 4 columns is taken from the intermediate file, namely- LOC, LABEL, OPCODE, OPERAND. The assembly listing contains the object code corresponding to each line, thus 5 columns- LOC, LABEL, OPCODE, OPERAND, OBJECT_CODE. The assembler output contains Header record, Text records and End record. 