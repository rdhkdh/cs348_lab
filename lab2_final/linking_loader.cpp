// Ridhiman DHindsa, 210101088
#include<bits/stdc++.h>
using namespace std;

map<string,string> ESTAB; //external symbol table - extdef symbol, secn name
string intermediate = "assembler_intermediate.txt";
string assembler_output_filename = "assembler_output.txt";
ifstream fin; //input file stream
ofstream fout; //output file stream
int PROGADDR; //First section starting address, provided by OS
int CSADDR; //Current section starting address

string progName; //string for storing current section name
int progLength; //length of current section
int progStartAddr; //starting address of current section

string line; //string for storing a line of input
vector<string> word_list; //vector for storing all words in the input line
string word; //string for storing a word in a line

//strings for label, opcode and operand respectively
string label;
string opcode;
string operand;

bool extendedFlag; //indicates extended format
map<string,string> OPTAB; //operation code table

//generates list of words present in the line read from input file
void generate_wordlist(){
    word_list.clear();
    stringstream ss(line);
    while(ss>>word){
        word_list.push_back(word);
    }
    return;
}

//gets OPTAB ready
void process_optab(){
    while(!fin.eof()){
        getline(fin, line);
        generate_wordlist();
        OPTAB[word_list[0]] = word_list[1];
    }
    return;
}

//Remove whitespace from beginning and end of string
string stripString(string s){
	//Removing spaces from end
	while(!s.empty() && s.back()==' ')
        s.pop_back();
	if(s.empty())
        return s;
	//Removing spaces from beginning
	reverse(s.begin(), s.end());
	while(!s.empty() && s.back()==' ')
        s.pop_back();
    reverse(s.begin(), s.end());
	return s;
}

//extends or shortens string to desired length
string make_len(string &str, int len, int posn, char c) {
    string res = "";
    if((int)str.length() == len)
        res = str;
    else if((int)str.length() > len)
        res = str.substr(0, len);
    else{
        res = str;
        if(posn == 0){
            reverse(res.begin(), res.end());
            while((int)res.length() < len){
                res.push_back(c);
            }
            reverse(res.begin(), res.end());
        }
        else{
            while((int)res.length() < len){
                res.push_back(c);
            }
        }
    }
    return res;
}

//Extract opcode, label and operand from a line based on the input file format
bool extract_line(string &line, string &label, string &opcode, string &operand, bool &extendedFlag) {
    //if line comprises only of spaces
	if(stripString(line).empty()){
		return false;
	}
	//Checking if valid line and not comment
	if((line.size()>80 || line.size()<21) && stripString(line)[0]!='.'){
		return false;
	} else if(stripString(line)[0]=='.'){
		//Comments are still printed in intermediate.txt
		return true;
	}
	//Adding spaces at the end of line to make it 80 characters long.
	line = make_len(line, 80, 1, ' ');
	//extracting label, opcode and operand fields from input based on input file format
	//and then removing spaces at the beginning and end of these fields
	label=stripString(line.substr(0,10));
	opcode=stripString(line.substr(10,10));
	operand=stripString(line.substr(20));

	//Handle extended format
	if(opcode[0]=='+'){
		extendedFlag=true;
		reverse(opcode.begin(), opcode.end());
		opcode.pop_back();
		reverse(opcode.begin(), opcode.end());
	}
	else {
		extendedFlag=false;
	}

	return true;
}

//Convert hexadecimal string to decimal integer
int hex_to_dec(string hex){
	int res=0;
	for(int i=0;i<(int)hex.length();i++){
		if(hex[i]>='A' && hex[i]<='F')
            res = res*16 + hex[i] - 'A' + 10;
		else
            res = res*16 + hex[i] - '0';
	}
	return res;
}

//hex digits 0-15
char hex_code[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

//converts the given number into hexadecimal string
string dec_to_hex(int num){
    if(num == 0)
        return "0";
    string res = "";
    while(num){
        res.push_back(hex_code[num%16]);
        num = num/16;
    }
    reverse(res.begin(), res.end());
    return res;
}

//function for handling errors in input file
//TYPE 1 - INCORRECT FORMAT
//TYPE 2 - DUPLICATE EXTERNAL SYMBOL
//TYPE 3 - UNDEFINED EXTERNAL SYMBOL
void handle_errors(int type, string line){
    if(type == 1){
        cout<<"INCORRECT FORMAT in the line: "<<line<<endl;
        cout<<"Aborting execution.\n";
        exit(0);
    }
    else if(type == 2){
        cout<<"DUPLICATE EXTERNAL SYMBOL in the line: "<<line<<endl;
        cout<<"Aborting execution.\n";
        exit(0);
    }
    else if(type == 3){
        cout<<"UNDEFINED EXTERNAL SYMBOL in the line: "<<line<<endl;
        cout<<"Aborting execution.\n";
        exit(0);
    }
}

//function for extracting a substring out of a string and returning it
//if length is not specified, the end of string is taken as the right end of the substring
string extractSubstring(string& str, int start_idx, int len = -1){
    string res = "";
    if(len == -1){
        for(int i = start_idx; i<str.length(); i++){
            res.push_back(str[i]);
        }
    }
    else{
        for(int i = start_idx; i<start_idx + len; i++){
            res.push_back(str[i]);
        }
    }
    return res;
}

//function which fills second string into the first
//the specified starting address works as the offset
void fill_code(string& s, string& c, int start_addr){
    for(int i=0;i<c.size();i++){
        s[start_addr+i]=c[i];
    }
    return;
}

//returns true if a is divisible by b
//returns false otherwise
bool divisible(int a, int b){
    return a==((a/b)*b);
}

//writes given string to output file
void output(string s){
    fout<<s;
}

//writes given character to output file
void output(char c){
    fout<<c;
}

//returns true if both the characters are equal
//returns false otherwise
bool isEQUAL(char a, char b){
    if(a == b)
        return true;
    else return false;
}

//returns true if both the strings are equal
//returns false otherwise
bool isEQUAL(string a, string b){
    if(a == b)
        return true;
    else return false;
}

//handles errors in file handling
void handle_file_error(int type, string filename){
    if(type == 1)
        cout<<"FILE "<<filename<<" NOT FOUND\n";
    else
        cout<<"CREATION OF "<<filename<<" FAILED\n";
    cout<<"EXITING PROGRAM\n";
    exit(0);
}

int main(int argc, char* argv[]){
	if(argc >= 3){
        intermediate = argv[1];
        assembler_output_filename = argv[2];
        cout<<"Using assembler intermediate filename: "<<intermediate<<"\n";
        cout<<"Using assembler output filename: "<<assembler_output_filename<<"\n";
    }
    else{
        cout<<"Filenames of assembler intermediate and assembler output files not specified\n";
        cout<<"Using default assembler intermediate filename: "<<intermediate<<"\n";
        cout<<"Using default assembler output filename: "<<assembler_output_filename<<"\n";
    }

    //Initializing global variables
	PROGADDR=0;
    progStartAddr=0;
    progName.clear();
    progLength=0;
	CSADDR=PROGADDR;

    //opening opcodes.txt
	fin.open("opcodes.txt");
    if(!fin){
        handle_file_error(1, "opcodes.txt");
    }
    //populating Operation Code Table
    process_optab();
    //closing file stream
    fin.close();

    //opening assembler output file
	fin.open(assembler_output_filename);
	if(!fin){
        handle_file_error(1, assembler_output_filename);
	}

    //---------------------------Pass 1 begins------------------------------------------

	while(getline(fin,line,'\n')) {
        //If line is empty, skip it.
		if(stripString(line).empty()){
			continue;
		}

		line=stripString(line); //Remove spaces from the beginning and end of line

		//it has to be a Header record. if it is not, display INCORRECT FORMAT ERROR
		if(!isEQUAL('H', line[0])){
			handle_errors(1, line);
		}

		//length of program
		int CSLTH=hex_to_dec(extractSubstring(line, 13, 6));
		//starting address of program
		progStartAddr=hex_to_dec(extractSubstring(line, 7, 6));
		//Program Name
		progName=stripString(extractSubstring(line, 1, 6));

        //if the program name has been already defined, display DUPLICATE EXTERNAL SYMBOL ERROR
		if(ESTAB.count(progName)) { handle_errors(2, line); }

		//Make an entry in ESTAB for the current program
		//map it to the hexadecimal string containing its starting address
		ESTAB[progName]=dec_to_hex(CSADDR);

		while(getline(fin,line,'\n')) {
            //If line is empty, skip it.
			if(stripString(line).empty())
				continue;
            //Remove spaces from the beginning and end of line
			line=stripString(line);
			//if END of section
			if(isEQUAL('E',line[0])){
				break;
			}
			//if it is a Define Record, add every symbol to ESTAB
			if(isEQUAL('D',line[0])) {
				if(line.length() >= 2) {
                    //removing the first character since it is 'D'
					string symbolString=extractSubstring(line, 1);

					for(int i=0;i<symbolString.length();i+=12) {
					    //first 6 characters are for symbol
						string symbol=extractSubstring(symbolString, i, 6);
                        //next 6 characters are for value
						string addr=extractSubstring(symbolString, i+6, 6);
						//if symbol already exists in ESTAB
						//display DUPLICATE EXTERNAL SYMBOL ERROR
						if(ESTAB.count(symbol)){
							handle_errors(2, line);
						}
						//add symbol to ESTAB
						ESTAB[symbol]=dec_to_hex(hex_to_dec(addr)+CSADDR);
					}
				}
			}
		}
		CSADDR+=CSLTH; //moving onto the next section
	}

    //closing file stream
	fin.close();

	//Pass 1 ends

	//Maximum now addr is CSADDR-1
	//Hence, memory size required is CSADDR
	//This makes it divisible by 32
	int memSize=(CSADDR*2/32+(CSADDR*2%32!=0))*32;	//ceil(CSADDR/32)*32. Hexadecimal, therefore *2

	//Simulating memory in the form of a string of size memSize
	//initially, all blocks of memory contain '.' indicating blank space
	string RAM(memSize, '.');

	//-------------------------------Pass 2 begins------------------------------------------

    //opening assembler output file
	fin.open(assembler_output_filename);
	if(!fin){
        handle_file_error(1, assembler_output_filename);
	}

	int EXECADDR=PROGADDR; //Execution Starting Address
	CSADDR=PROGADDR; //Current section Starting Address

	while(getline(fin,line,'\n')) {
        //If line is empty, skip it.
		if(stripString(line).empty()) {continue;}
        
		line=stripString(line); //Remove spaces from the beginning and end of line	
		int CSLTH=hex_to_dec(extractSubstring(line, 13, 6)); //Length of current section

		//start reading current section
		while(getline(fin,line,'\n')) {
            //If line is empty, skip it.
			if(stripString(line).empty())
				continue;
            //Remove spaces from the beginning and end of line
			line=stripString(line);

			//Text Record
			if(isEQUAL('T',line[0])) {
                //effective code
				string code=extractSubstring(line, 9);
                //start address to insert code in half-bytes
				int start_address=2*(hex_to_dec(extractSubstring(line, 1, 6))+CSADDR);
				fill_code(RAM, code, start_address);
			}

			//modification record
			else if(isEQUAL('M',line[0])) {
                //sign to be used
				char sign=line[9];
                //symbol of modification record
				string symbol=extractSubstring(line, 10);
				//if symbol not found in ESTAB
				if(ESTAB.count(symbol)==0){
					//display UNDEFINED EXTERNAL SYMBOL ERROR
					handle_errors(3, line);
				}
				//value to be added
				int valueToAdd=hex_to_dec(ESTAB[symbol]);
				//memory address to start modification in half-bytes
				int addressForModification=(CSADDR+hex_to_dec(extractSubstring(line, 1, 6)))*2;
				//length of modification in half-bytes
				int lengthOfModification=hex_to_dec(extractSubstring(line, 7, 2));
				//then last lengthOfModification half-bytes will be modified
				if(lengthOfModification&1)
					addressForModification++;
                //the current value at that address
				string valueToModify;
				for(int i=addressForModification;i<addressForModification+lengthOfModification;i++){
					valueToModify+=RAM[i];
				}
				//easier to use
				int integer_helper=hex_to_dec(valueToModify);
				//modify value
				if(isEQUAL('+',sign))
                    integer_helper = integer_helper + valueToAdd;
                else integer_helper = integer_helper - valueToAdd;
				valueToModify=dec_to_hex(integer_helper);
				valueToModify = make_len(valueToModify, lengthOfModification, 0, '0');
				//write back to memory location
				for(int i=addressForModification;i<addressForModification+lengthOfModification;i++){
					RAM[i]=valueToModify[i-addressForModification];
				}
			}
			//these records contain start address of execution
			else if(isEQUAL('E',line[0])){
				if(line.length() >= 2){
					EXECADDR=CSADDR+hex_to_dec(extractSubstring(line, 1));
				}
				break;
			}
		}
		CSADDR+=CSLTH; //moving onto the next section
	}
	//closing file stream
	fin.close();

	//---------------------------------------Pass 2 ends----------------------------------------------

    //Writing all the opcodes to the output file
	fin.open(intermediate);
	if(!fin){
        handle_file_error(1, intermediate);
	}
	fout.open("linking_loader_output.txt");
	if(!fout){
        handle_file_error(2, "linking_loader_output.txt");
	}

    //set starting address of current section to 0
	CSADDR=0;
	while(getline(fin,line,'\n')) {
        //If line is empty, skip it.
		if(stripString(line).empty())
			continue;
        //If line is a comment, skip it.
        if(stripString(line)[0]=='.')
            continue;

        //set extendedFlag to false
		extendedFlag=false;
		string instruction_line=extractSubstring(line, 14);
		//Get label, opcode and operand from line
		extract_line(instruction_line,label,opcode,operand,extendedFlag);
		//Used to set section address
		if(isEQUAL("START", opcode) || isEQUAL("CSECT", opcode)){
			CSADDR=hex_to_dec(ESTAB[stripString(extractSubstring(line, 14, 10))]);
			continue;
		}
		if(OPTAB.count(opcode) || isEQUAL("WORD", opcode) || isEQUAL("BYTE", opcode) || isEQUAL('=', opcode[0])){
            //if important instruction, get its address and length and write it to output file
			int address=2*(hex_to_dec(stripString(extractSubstring(line, 0, 6)))+CSADDR);
			int len=2*stoi(stripString(extractSubstring(line, 7, 6)));
			for(int i=address;i<address+len;i++){
				output(RAM[i]);
			}
			output('\n');
		}
	}

    //closing file streams
	fout.close();
	fin.close();

    //opening linking_loader_memory_map.txt
	fout.open("linking_loader_memory_map.txt");
	if(!fout){
        handle_file_error(2, "linking_loader_memory_map.txt");
	}

    //Write memory status to file
	for(int i=0;i<memSize;i++) {
		if(divisible(i, 32)){
			if(i)
                output('\n');
            int num = i/2;
			string hexAddr=dec_to_hex(num);
			hexAddr = make_len(hexAddr, 6, 0, '0');
			output(hexAddr);
			output(" ");
		} else if(divisible(i,8)){
			output(" ");
		}
		output(RAM[i]);
	}

    //closing file stream
	fout.close();
}