# Assignment 4 - LR Parser
Ridhiman Dhindsa, 210101088  
Date: 23 April 2024  

## Files included
**Inputs:** *yaccfile.y, lexfile.l, input_wrong.txt, input_correct.txt*  
**Outputs:** *All syntactic and semantic errors are listed on the terminal.*  

The Yacc translator file contains the grammar rules defining the syntax of a programming language or another 
formal language. First, we compile the Yacc translator file to generate a parser. `y.tab.c` represents the C routines 
required by the parser to interpret the grammar specified. It also includes user-defined C routines, if they exist.
Next, we compile the Lex specification to obtain a lexical analyzer `lex.yy.c`. These 2 files are then compiled using 
the C compiler (gcc) to obtain an executable. The executable file functions as the frontend of a compiler, carrying out 
lexical analysis of the input program to generate tokens and then passing these tokens to the parser routine for syntactic 
and semantic analysis. The parsing routine then outputs errors within the input program, if any.

## Steps to Run
1) Open the terminal in the project directory.  
2) Compile the yacc file using: `yacc yaccfile.y -d -Wno-yacc`    
3) Compile the Lex file using: `lex lexfile.l`  
4) Compile to generate the executable using: `gcc lex.yy.c y.tab.c -o executable`  
5) Run the executable using: `./executable < input_wrong.txt`  
> The input of the previous assignment can be run using `./executable < input_correct.txt`. Since it is a syntactically and sematically correct input, there will be no errors displayed.  

## Description
The Yacc file consists of 3 sections:  
* **Declarations-** It specifies the declarations of variables, manifest constants and regular definitions. This section consists of 2 subsections- the first contains header files and global variables required for execution. The second contains declarations of grammar tokens.  
* **Translation rules-** These define the rules of grammar to be followed by the parser for checking errors in input. The grammar production rules are specified as mentioned in the question. Each grammar production is followed by its corresponding semantic action.  
* **Supporting C routines-** Holds auxuilary functions required by the translation rules' actions. 2 types of hash functions are available for hashing in the symbol table for the lexical analyzer, namely- Murmur hashing (64 bit), FNV hashing (64 bit). FNV hashing is used for the current implementation.  
