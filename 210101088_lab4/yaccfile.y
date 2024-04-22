%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// macros for size of hash table and key
#define size_of_hash_table 400
#define size_of_key 200
#define identifier_does_not_exist_error_code -200

extern int yylex(); //external function, defined in lex.yy.c
extern int yylineno;
void yyerror(const char *s); // for handling syntax errors in the program

// node of linked list used in symbol table
struct node {
	char key[size_of_key]; //symbol
	int type; //symbol type
	struct node *next;
};

int last_expression_type = -1; // stores the type of last expression. -1 if no prev expr
int mismatched_type = 0; // =1 if mismatched type error
int line_number = -1; 
int error_index = 1; //total no of errors found

struct node* hash_table[size_of_hash_table]; // array of linked lists
struct node* last_node[size_of_hash_table]; // tails of each linked list
int num_elements[size_of_hash_table];

void get_identifier_type(char key[], int *type); //returns type of identifier, if present
void insert_identifier(char key[], int type); //inserts identifier into hash table
void insertall_identifiers(char s[], int type); //parse a string into component identifiers and insert all
void verify_list_of_identifiers(char s[]); //parse a string into component identifiers and check for existence of all
void concatenate(char a[], char b[], char result[]); // concatenate string a, b into result
void reset_variables(); //reset error indicators

%}
// the different types of grammar tokens are listed below
%union {int int_num; float real_num; char id[500];}
%define parse.error verbose
%start prog
%token PROGRAM
%token VAR
%token BEGIN_TOKEN
%token END
%token PROGRAM_END
%token INTEGER
%token REAL
%token FOR
%token READ
%token WRITE
%token TO
%token DO
%token ASSIGNMENT
%token DIVIDE
%token <id> IDENTIFIER
%token <int_num> INTEGER_NUMBER
%token <real_num> REAL_NUMBER
%type <int_num> type
%type <id> id_list

%%
// Pascal Grammar
prog : PROGRAM prog_name VAR dec_list BEGIN_TOKEN stmt_list PROGRAM_END {exit(EXIT_SUCCESS);}
		;
prog_name : IDENTIFIER
		;
dec_list : dec | dec_list ';' dec | error
		;
dec : id_list ':' type {insertall_identifiers($1,$3);} // a list of identifiers of a certain type will be added to the symbol table
		;
type : INTEGER { $$ = 6; } | REAL { $$ = 7; }
		;
id_list : IDENTIFIER | id_list ',' IDENTIFIER {concatenate($1, $3, $$);} // a comma separated list of identifiers wil be stored in $$
		;
stmt_list : stmt | stmt_list ';' stmt
		;
stmt : assign_stmt | read_stmt | write_stmt | for_stmt | error
		;
assign_stmt : IDENTIFIER ASSIGNMENT expression {
			int type;
			get_identifier_type($1, &type);
			// if identifier exists in the symbol table
			if(type != identifier_does_not_exist_error_code){
				// if type of identifier is different from previous expression or mismatched type flag is set
				if((type != last_expression_type) || mismatched_type == 1){
					// print an error message on the screen
					fprintf(stderr, "ERROR %d: TYPE MISMATCH ERROR IN ASSIGNMENT STATEMENT FOR THE IDENTIFIER %s ON LINE NUMBER %d\n", error_index, $1, line_number);
					error_index++;
				}

			}
			//reset last_expression_type, mismatched_type and line_number
			reset_variables(1);
		}
		;
expression : term
	| expression '+' term
	| expression '-' term
	;
term : factor
	| term '*' factor
	| term DIVIDE factor
	;
factor : IDENTIFIER {
			line_number = yylineno;
			int type;
			get_identifier_type($1, &type);
			// set mismatched type flag if the type of identifier is different from previous expression
			if(type != identifier_does_not_exist_error_code && last_expression_type != -1 && last_expression_type != type)
				mismatched_type = 1;
			// takes care of the case where there is no previous expression
			else if(type != identifier_does_not_exist_error_code)
				last_expression_type = type;
		}
		| INTEGER_NUMBER {
			line_number = yylineno;
			// we use a type 6 to indicate integer number
			// in case type of last expression was non-integral, set the mismatched type flag
			if(last_expression_type != 6 && last_expression_type != -1){
				mismatched_type = 1;
			}
			else
				last_expression_type = 6;
		}
		| REAL_NUMBER {
			line_number = yylineno;
			// we use a type 7 to indicate integer number
			// in case type of last expression was non-real, set the mismatched type flag
			if(last_expression_type != 7 && last_expression_type != -1){
				mismatched_type = 1;
			}
			else
				last_expression_type = 7;
		}
		;
read_stmt : READ '(' id_list ')' {verify_list_of_identifiers($3);} // verify that all the identifiers are already present in the symbol table
		;
write_stmt : WRITE '(' id_list ')' {verify_list_of_identifiers($3);}  // verify that all the identifiers are already present in the symbol table
		;
for_stmt : FOR index_exp DO body
		;
index_exp : IDENTIFIER ASSIGNMENT expression TO expression {
			int type;
			get_identifier_type($1,&type);
			// if identifier exists in the symbol table
			if(type != identifier_does_not_exist_error_code){
				// if type of identifier is different from previous expression or mismatched type flag is set
				if(type != last_expression_type || mismatched_type == 1){
					// print an error message on the screen
					fprintf(stderr, "ERROR %d: TYPE MISMATCH ERROR IN FOR LOOP ITERATOR %s ON LINE NUMBER %d\n", error_index, $1, yylineno);
					error_index++;
				}
			}
			//reset last_expression_type and mismatched_type
			reset_variables(0);
		}
		;
body : stmt | BEGIN_TOKEN stmt_list END
		;

%%

// used to reset the values of variables used as error indicators
void reset_variables(int code){
	last_expression_type = -1;
	mismatched_type = 0;
	if(code == 1){
		line_number = -1;
	}
}

// used to concatenate strings and put the resultant string in the third argument.
void concatenate(char a[], char b[], char result[]){
	int i = 0;
	for(int j=0; a[j]; j++){
		result[i] = a[j];
		i++;
	}
	result[i] = ',';
	i++;
	for(int j=0; b[j]; j++){
		result[i] = b[j];
		i++;
	}
	result[i] = '\0';
}

// function which creates a new node and fills in data according to the provided arguments
struct node* create_node(char *key, int type)
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));

	strcpy(temp->key, key);
	temp->type = type;
	temp->next=NULL;

	return temp;
}

//Murmur Hashing
uint64_t MurmurOAAT64(char *key)
{
	uint64_t h = 525201411107845655ull;
	for (; *key; ++key) {
		h ^= *key;
		h *= 0x5bd1e9955bd1e995;
		h ^= h >> 47;
	}
	return h;
}

//uses FNV hashing to compute 64-bit hash of a given string
uint64_t FNV1a_64(char* key) 
{
    uint64_t hash = 14695981039346656037ULL; //initial value
    while (*key != '\0') //iterate over all characters of string
    {
        hash ^= (uint64_t)(*key++); //XOR the char with current value of hash
        hash *= 1099511628211ULL; //scramble by multiplying with constant
    }
    
    return hash;
}

//adding new_identifier to the symbol table
void insert_identifier(char key[], int type){

	uint64_t hash = FNV1a_64(key);

	int hash_index = hash % size_of_hash_table;

	struct node *itr = hash_table[hash_index];

	//if identifier is already present in the symbol table, display an error message
	while(itr != NULL){
		if(strcmp(itr->key, key) == 0){
			fprintf(stderr, "ERROR %d: IDENTIFIER %s ALREADY EXISTS IN SYMBOL TABLE ON LINE NUMBER %d\n", error_index, key, yylineno);
			error_index++;
			return;
		}

		itr = itr->next;
	}

	//if identifier is not present in the symbol table, add it to the symbol table
	struct node *new_node = create_node(key, type);

	if(num_elements[hash_index] == 0){
		hash_table[hash_index] = new_node;
		last_node[hash_index] = new_node;
		num_elements[hash_index]++;
	}
	else{
		last_node[hash_index] -> next = new_node;
		last_node[hash_index] = new_node;
		num_elements[hash_index]++;
	}

}

//extracts the substring s[left...right] and puts it in key
void extractSubstring(char s[], char key[], int left, int right){
	
	for(int i = left; i <= right; i++){
		key[i-left] = s[i];
	}
	int endd = right + 1 - left;
	key[endd] = '\0';

}

//function for adding a comma separated list of identifiers
void insertall_identifiers(char s[], int type){
	
	char delimiter = ',';
	char key[size_of_key];
	int left = -1;
	int right = 0;

	for(; s[right]; right++){
		if(s[right] == delimiter){
			extractSubstring(s, key, left + 1, right - 1);
			insert_identifier(key,type);
			left = right;
		}
	}
	//taking care of the last identifier
	extractSubstring(s, key, left + 1, right - 1);
	insert_identifier(key,type);
}

//function which returns the type of a symbol in symbol table, if present
//if it isn't present, it returns identifier_does_not_exist_error_code 
void get_identifier_type(char key[], int *type){
	uint64_t hash = FNV1a_64(key);

	int hash_index = hash % size_of_hash_table;

	struct node *itr = hash_table[hash_index];

	while(itr != NULL){
		//identifier found
		if(strcmp(itr->key, key) == 0){
			*type = itr->type;
			return;
		}

		itr = itr->next;
	}
	//identifier not found in symbol table
	*type = identifier_does_not_exist_error_code;

	if(line_number == -1){
		fprintf(stderr, "ERROR %d: IDENTIFIER %s DOES NOT EXIST ON LINE NUMBER %d\n", error_index, key, yylineno);
		error_index++;
	}
	else{
		fprintf(stderr, "ERROR %d: IDENTIFIER %s DOES NOT EXIST ON LINE NUMBER %d\n", error_index, key, line_number);
		error_index++;
	}
	
}

//function which verifies whether the comma separated list of integers is present in the symbol table or not
//if it is not, then it displays an error message
void verify_list_of_identifiers(char s[]){

	char delimiter = ',';
	char key[size_of_key];
	int left = -1;
	int right = 0;

	for(; s[right]; right++){
		if(s[right] == delimiter){
			extractSubstring(s, key, left + 1, right - 1);
			int type;
			get_identifier_type(key,&type);
			left = right;
		}
	}
	//taking care of the last identifier
	extractSubstring(s, key, left + 1, right - 1);
	int type;
	get_identifier_type(key,&type);

}

int main(void){
	//setting number of elements in hash table to zero for all hash values
	memset(num_elements, 0, sizeof(num_elements));

	return yyparse();
}

//function for printing syntax errors in program
void yyerror (const char *s) {

	fprintf (stderr, "ERROR %d: %s ON LINE NUMBER %d\n", error_index, s, yylineno);
	error_index++;

}