# Assignment 3 - Lexical Analyzer
Ridhiman Dhindsa, 210101088  
Date: 2 April 2024  

## Files included
**Inputs:** *analyzer.c, input.txt, lex.l*    
**Outputs:** *output.txt*  
The Lex specification `lex.l` is compiled using the Lex compiler to generate the code for the corresponding lexical analyzer `lex.yy.c`. The C file `analyzer.c` takes input tokens from `input.txt` using the lexical analyzer and generates `output.txt` which specifies the hash structure obtained after reading the input.  

## Steps to Run
1) Open the terminal in the project directory.  
2) Compile the Lex file using: `lex lex.l`  
3) Compile the C file using: `gcc analyzer.c lex.yy.c -o analyzer`  
4) Run the executable using: `./analyzer < input.txt`  

## Description
The Lex file consists of 3 sections:   
* **Declarations-** It specifies the declarations of variables, manifest constants and regular definitions. No declarations are required in this case, hence the section is left blank.
* **Translation rules-** These describe regular expressions and the corresponding actions to be taken if a lexeme matches a particular pattern. In the given file the token code for the identifier is returned.  
* **Auxiliary procedures-** Holds auxuilary functions required by the actions.

The C file uses the lexical analyzer to read the input file word by word. The tokens are returned to the C file by the lexical analyzer, and they are subsequently stored in a hash table.  
The hash table is designed as an array of linked lists, with 2 types of hashing algorithms available, namely- MurmurOAAT64 (Murmur One-at-a-Time) hashing and FNV-1 (Fowler-Noll-Vo) hashing. The hash values are calculated and then used to determine the hash index within the table.