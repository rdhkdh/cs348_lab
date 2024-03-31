//Ridhiman Dhindsa, 210101088
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//optimal size of hash table for murmur hashing is 2*(expected number of elements)
#define SIZE_HT 400 //size of hash table
#define SIZE_KEY 200 //size of key

//these are defined in lex.yy.c
extern int yylex(); //refered using extern keyword
extern int yylineno;
extern char* yytext;

//structure for node of linked list
struct node 
{
	char key[SIZE_KEY]; //will store token specifier (without ^ and #)
	int value; //will store token type
	struct node *next; //pointer to next node in linked list
};

struct node* hash_table[SIZE_HT]; // array of linked lists
struct node* last_node[SIZE_HT]; //array storing pointers to last node of each linked list
int ll_length[SIZE_HT]; //array storing the length of each linked list

//creates a new node using given parameters and returns a pointer to it
struct node* create_node(char *key, int value)
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));

	strcpy(temp->key, key);
	temp->value = value;
	temp->next=NULL;

	return temp;
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

//uses murmur hashing to compute 64-bit hash of a given string
uint64_t MurmurOAAT64(char *key)
{
	uint64_t h = 525201411107845655ull; //initial value
	for (; *key; ++key) //iterate over all characters of string
    {
		h ^= *key;  //XOR the char with current value of hash
		h *= 0x5bd1e9955bd1e995;  //scramble by multiplying with constant
		h ^= h >> 47;  //right shift 47 places and XOR with original hash
	}
	return h;
}

//Inserts identifier into hash table, if not already present
void install_id(char *key, int value, int *list_index, int *hash_index)
{
	//compute hash index of string using murmur hashing
	uint64_t hash = FNV1a_64(key);
	*hash_index = hash % SIZE_HT;

    //setting iterator to head of linked list corresponding to hash_index
	struct node *itr = hash_table[*hash_index]; 
	int idx = 0; //setting current index to 0

	//iterating over the linked list
	while(itr != NULL)
    {
		//if key is already present, set list_index to idx and return
		if(strcmp(itr->key, key) == 0)
        {
			*list_index = idx;
			return;
		}

		itr = itr->next;
		idx++;
	}

	//identifier wasn't already present in hash table
	struct node *new_node = create_node(key, value); //so, create new node

	//if linked list is empty
	if(ll_length[*hash_index] == 0)
    {
		//set head pointer and last pointer to the current node
		hash_table[*hash_index] = new_node;
		last_node[*hash_index] = new_node;
		ll_length[*hash_index]++; //increment size of linked list
	}
	else //if linked list isn't empty
    {	
		last_node[*hash_index] -> next = new_node;  //next of last node will point to current node
		last_node[*hash_index] = new_node;  //set current node as last node
		ll_length[*hash_index]++; //increment size of linked list
	}

	//list index of added node will be (size of linked list - 1) 
	*list_index = ll_length[*hash_index] - 1;
}

//Inserts integer into hash table, if not already present
void install_num(char *key, int value, int *list_index, int *hash_index)
{
	//compute hash index of string using murmur hashing
	uint64_t hash = FNV1a_64(key);
	*hash_index = hash % SIZE_HT;
	
	//setting iterator to head of linked list corresponding to hash_index
	struct node *itr = hash_table[*hash_index];
	int idx = 0; //setting current index to 0
	
	//iterating over the linked list
	while(itr!=NULL)
    {
		//if key is already present, set list_index to idx and return
		if(strcmp(itr->key, key) == 0)
        {
			*list_index = idx;
			return;
		}

		itr = itr -> next;
		idx++;
	}

	//if integer wasn't already present in hash table
	struct node *new_node = create_node(key, value);

	//if linked list is empty
	if(ll_length[*hash_index] == 0)
    {
		//set head pointer and last pointer to the current node
		hash_table[*hash_index] = new_node;
		last_node[*hash_index] = new_node;
		ll_length[*hash_index]++; //increment size of linked list
	}
	else //if linked list isn't empty
    {
		last_node[*hash_index] -> next = new_node; //next of last node will point to current node
		last_node[*hash_index] = new_node;  //set current node as last node
		ll_length[*hash_index]++;  //increment size of linked list
	}

	//list index of added node will be (size of linked list - 1) 
	*list_index = ll_length[*hash_index] - 1;
}

int main()
{
	FILE *fout = fopen("output.txt", "w");
	memset(ll_length, 0, sizeof(ll_length)); //setting sizes of all linked lists to 0
	int token = yylex(); //yylex() reads a token and returns its type 
	int last_line = 0; //number of the last line that was read
	
	fprintf(fout, "  LINE  TOKEN TYPE         TOKEN SPECIFIER\n");
	fprintf(fout, "  ----  ----------         ---------------\n");

	//loop until the entire file has been read
	while(token)
    {
		int hash_index; //hash index for current node
		int list_index; //list index for current node
		int line_num; //current line number	
		
		char line_num_str[7]; //used for printing line number
		line_num_str[6] = '\0';
		
		char token_type_str[7]; //used for printing token type
		token_type_str[6] = '\0';

		line_num = yylineno; //from lex.yy.c file

        //print line number to file
		if(line_num != last_line)
        {
            //convert line num to string
			int temp = line_num;
			int cur = 5;
			while(temp) {
				line_num_str[cur] = '0' + (temp % 10);
				temp /= 10;
				cur--;
			}
            //give padding
			while(cur>=0) {
				line_num_str[cur] = ' ';
				cur--;
			}
		}
		else
        {   
			int cur = 5;
			while(cur >= 0) {
				line_num_str[cur] = ' '; //empty string
				cur--;
			}
		}
		fprintf(fout, "%s  ", line_num_str);

		last_line = yylineno; //from lex.yy.c file

        //print token type to file
		int temp = token;
		int cur = 5;
		while(temp) 
        {
            //convert token type to string
			token_type_str[cur] = '0' + (temp % 10);
			temp /= 10;
			cur--;
		}
        //give padding
		while(cur >= 0){
			token_type_str[cur] = ' ';
			cur--;
		}
		fprintf(fout, "%s    ", token_type_str);		
		
        //print idetntifier details if token is variable or number
		if(token==22)
        {		
			char *var_name = yytext; //stores name of identifier
			install_id(var_name, 22, &list_index, &hash_index); //insert identifier into hash table
            //print token identifier, hash index, list index to file
			fprintf(fout, "^%s (HASH_INDEX: %d, LIST_INDEX: %d)\n", yytext, hash_index, list_index); 
		}
		else if(token==23)
        {
			char *num = yytext; //stores value of integer
			install_num(num, 23, &list_index, &hash_index); //insert integer into hash table
			//print token identifier, hash index, list index to file
			fprintf(fout, "#%s (HASH_INDEX: %d, LIST_INDEX: %d)\n", yytext, hash_index, list_index);
		}
		else {
			fprintf(fout, "\n");
		}
		
		token = yylex(); //calling yylex() and updating the value of token
	}

	fprintf(fout, "\n");

	//printing non-empty entries in hash table to file
	fprintf(fout, "      HASH TABLE (non-empty entries only)\n");
	fprintf(fout, "----------------------------------\n");
	fprintf(fout, "\n\n");
	fprintf(fout, "  INDEX   TOKEN TYPE   TOKEN SPECIFIER\n");
	fprintf(fout, "  -----   ----------   ---------------\n");

	int last_index = -1; //hash index of last node that was processed
	char hash_index_str[7]; //used for printing hash index
	hash_index_str[6] = '\0';
	
	char token_type_str[7]; //used for printing token type
	token_type_str[6] = '\0';

	//iterate over all linked lists
	for(int i = 0; i < SIZE_HT; i++)
    {
		//if linked list is non-empty
		if(ll_length[i] > 0)
        {
			//setting iterator to head of linked list corresponding to hash_index
			struct node *itr = hash_table[i];
			//iterating over the linked list
			while(itr != NULL)
            {
				//---------------print hash index to file--------------------
				if(last_index != i) {
					if(i == 0){
						strcpy(hash_index_str, "     0");
					}
					else{
						int temp = i;
						int cur = 5;
                        //convert hash index to string
						while(temp){
							hash_index_str[cur] = '0' + (temp%10);
							temp /= 10;
							cur--;
						}
                        //give padding
						while(cur >= 0){
							hash_index_str[cur] = ' ';
							cur--;
						}
					}
				}
				else{
					int cur = 5;
					while(cur >= 0){
						hash_index_str[cur] = ' '; //empty string
						cur--;
					}
				}
				last_index = i;
				fprintf(fout, "%s    ", hash_index_str);

                //-------------print token type to file-----------------
				int temp = itr->value;
				int cur = 5;
                //convert token type to string
				while(temp){
					token_type_str[cur] = '0' + (temp % 10);
					temp /= 10;
					cur--;
				}
                //give padding
				while(cur >= 0){
					token_type_str[cur] = ' ';
					cur--;
				}
				fprintf(fout, "%s          ", token_type_str);
				
                //------------print token identifier to file------------------
				if(itr->value == 22) 
                { //if it's an identifier, write ^ before its name
                    fprintf(fout, "^%s\n",itr->key); 
                }
				else if(itr->value == 23) 
                { //if it's an integer, write # before its name
                    fprintf(fout, "#%s\n",itr->key); 
                }

				//moving to next node in linked list
				itr = itr->next;
			}
		}
	}
	
	fclose(fout); //closing file stream
	return 0;
}