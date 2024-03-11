/*

Run the commands in the folder contaniing the code and the input file

TO COMPILE:
 g++ 180101038_assign2_Assembler.cpp

TO RUN:
 ./a.out

Following is the assembler for sic xe with provisions for the extended version instructions in the code
pass1 creates the intermediate file
pass2 creates the listing.txt and output.obj

*/

//structures are defined to create tables of location,symbols,ext,literals
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <iomanip>

using namespace std;

struct loctab{
    string section_name;
    int loc;
    int start_addr;
    int length;
}LOCTAB[10];
struct symtab{
	string symbol;
	int addr;
	bool type;
	int control_section;
}SYMTAB[20];
struct ext{
    string symbol;
    int control_section;
    bool type;
}EXT[20];
struct littab{
	string name;
	int operand_value;
	int length;
	int addr;
};

string buffer,operand,opcode,label;
int buf_i, curr_section, lineLength, line_num, loc,line_objectCode,line_opcode,line_index,line_addr,record_index,define_index,part_index,refer_index,modi_index;

//tables of literals register and optable defined in the machine code are stoerd in these maps
map<string,littab> LITTAB;
map<string,int> reg;
map<string,string> optab;

//variables for formatting the output object codes as per various formats fo rhte six xe architechture
/*
two type of addressing modes
first:- |_op -6___|n|i|x|b|p|e|_____disp-12___| - n at 17 th location
second:- |_op -6___|n|i|x|b|p|e|_____disp-20__________| - n at 25 th location
for these we need to initiate the addresses prior so we can use directly
*/
static int format3_n=1<<17,format3_i=1<<16,format3_b=1<<14,format3_x=1<<15,format3_p=1<<13,format3_e=1<<12;
static int format4_i=1<<24,format4_x=1<<23,format4_b=1<<22,format4_n=1<<25,format4_p=1<<21,format4_e=1<<20;
static int BUFFER_SIZE = 256, OPTAB_LENGTH=28, REG_LENGTH=7;

//utilitiy variables to help in execution of the program
int SYMTAB_INDEX, SYMTAB_COUNT, SYMTAB_FOUND, OPTAB_FOUND,LOCTAB_COUNT,LOCTAB_FOUND,LOCTAB_INDEX,LITTAB_COUNT,LITTAB_FOUND,LITTAB_INDEX,EXT_FOUND,EXT_INDEX,EXT_COUNT,REG_FOUND,REG_INDEX;
bool is_extended_format, is_literal;
int format3_neg = (1<<12) - 1, format4_neg=(1<<20) -1;

string record,part,object_code,define_record,refer_record;
string modi_record,tmp_opcode,tmp_operand;

//initialising the machine codes available

void insertValuesOptabReg(){
	optab["LDA"]="00";
	optab["LDX"]="04";
	optab["LDL"]="08";
	optab["LDB"]="68";
	optab["LDT"]="74";
	optab["STA"]="0C";
	optab["STX"]="10";
	optab["STL"]="14";
	optab["LDCH"]="50";
	optab["STCH"]="54";
	optab["ADD"]="18";
	optab["SUB"]="1C";
	optab["MUL"]="20";
	optab["DIV"]="24";
	optab["COMP"]="28";
	optab["COMPR"]="A0";
	optab["CLEAR"]="B4";
	optab["J"]="3C";
	optab["JLT"]="38";
	optab["JEQ"]="30";
	optab["JGT"]="34";
	optab["JSUB"]="48";
	optab["RSUB"]="4C";
	optab["TIX"]="2C";
	optab["TIXR"]="B8";
	optab["TD"]="E0";
	optab["RD"]="D8";
	optab["WD"]="DC";
    reg["A"]=0;
    reg["X"]=1;
    reg["L"]=2;
    reg["B"]=3;
    reg["S"]=4;
    reg["T"]=5;
    reg["F"]=6;
}

//clearing all global variables at the start of the program
void initialize(){
    is_extended_format=is_literal=false;
    operand="";label="";opcode="";buffer="";
	buf_i=line_num=lineLength=SYMTAB_COUNT=LOCTAB_COUNT=EXT_COUNT=0;
	LOCTAB_INDEX=LITTAB_FOUND=SYMTAB_INDEX=EXT_FOUND=EXT_INDEX=REG_INDEX=-1;
    SYMTAB_FOUND=OPTAB_FOUND=LOCTAB_FOUND=REG_FOUND=0;
}

//help in reading a line from file and storing the read label opcode and operand
int readLine(fstream &fptr){
    buf_i=0;
    buffer="";
    getline(fptr,buffer);
    lineLength = buffer.length();
    buffer+='\n';
    lineLength++;
    //Read Label
    label="";
	for(;;){
		label += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    //Skip spaces
    for(;;){
        if(buffer[buf_i]!=' '&&buffer[buf_i]!='\t'){
            break;
        }
        buf_i++;
    }
    //Read Opcode
    opcode="";
    for(;;){
		opcode += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    //Skip spaces
    for(;;){
        if(buffer[buf_i]!=' '&&buffer[buf_i]!='\t'){
            break;
        }
        buf_i++;
    }
    //Read Operand
    operand="";
	for(;;){
		operand += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    line_num++;
    return lineLength;
}

//converting the string to hexinteger
int stringToHex(string str){
    int ans=0;
    for(int i=0;i<str.length();i++){
        ans*=16;
        if('0'<=str[i] && str[i]<='9'){
            ans += (str[i]-'0');
        }
        else if('a' <= str[i] && str[i]<= 'f'){
            ans += (10 + (str[i]-'a') );
        }
        else if('A' <= str[i] && str[i]<= 'F'){
            ans += (10 + (str[i]-'A'));
        }
    }
    return ans;
}

//calculate the value of expression as per the refrences 
void calculate_expr(string str, int *val, bool *expr_type, bool *ref_involved){
    string tmp="";
    bool found=false;
    *expr_type=false;
    int cnt_rel=0;
    if(str=="*"){
        *val=LOCTAB[curr_section].loc;
        cnt_rel++;
    }
    else{
        int prev_sign=1;
        for(int i=0;i<str.length();i++){
            if(str[i]=='-'|| str[i]=='+'){
                found=false;
                int k=0;
                while(k<SYMTAB_COUNT){
                    if(SYMTAB[k].symbol==tmp){
                        if(SYMTAB[k].control_section !=curr_section){
                            *ref_involved = true;
                            return;
                        }
                        *val += prev_sign*SYMTAB[k].addr;
                        cnt_rel += prev_sign;
						found = true;
						break; 
                    }
                    k++;
                }
                tmp="";
                if(str[i]=='-'){
					prev_sign = -1;
				}
				else{
					prev_sign = 1;
				}
            }
            else{
                tmp+=str[i];
            }
        }
        found=false;
        int k=0;
        while(k<SYMTAB_COUNT){
            if(SYMTAB[k].symbol==tmp){
                if(SYMTAB[k].control_section !=curr_section){
                    *ref_involved = true;
                    return;
                }
                *val += prev_sign*SYMTAB[k].addr;
                cnt_rel += prev_sign;
                found = true;
                break; 
            }
            k++;
        }
    }
    if(cnt_rel==0){
		*expr_type = true;
	}
	*ref_involved = false;
}

//used to search the sym table usign symbol name
void search_SYMTAB(string str){
    SYMTAB_INDEX=-1;
    SYMTAB_FOUND=0;
    int i=0;
    while(1){
        if(i>=SYMTAB_COUNT) break;
        if(SYMTAB[i].symbol==str ){
            if(SYMTAB[i].control_section==curr_section){
                SYMTAB_FOUND=1;
			SYMTAB_INDEX=i;
			break;
            }
		}
        i++;
    }
}

//inserting new symbol in the table
void add_SYMTAB(string str){
    SYMTAB_FOUND=0;
    SYMTAB[SYMTAB_COUNT].addr = LOCTAB[curr_section].loc;
    SYMTAB[SYMTAB_COUNT].type=0;
    SYMTAB[SYMTAB_COUNT].control_section=curr_section;
    SYMTAB[SYMTAB_COUNT].symbol=str;
    SYMTAB_INDEX = SYMTAB_COUNT;
    SYMTAB_COUNT++;
}

//used to calculate value of literal for resw resb type instructions
int valOfLiteral(string str){
    int val=0;
    string tmp=str;
    string tmp_ptr="";
    if ( ((int)(tmp[0]-'A')==2 || (int)(tmp[0]-'a')==2) && tmp[1] =='\''){
        int i=2;
        while (i<=str.length()-2){
			val += (int)str[i];
			val<<=8;
            i++;
		}
		val>>=8;
    }
    else if(((int)('Z'-tmp[0]) ==2 || (int)('z'-tmp[0]) ==2) && tmp[1] =='\''){
        int i=2;
        while(i<=str.length()-2){
            tmp_ptr+=str[i];
            i++;
        }
		val += stringToHex(tmp_ptr);
	}
	return val;
}

//used to calculate length of literal for constants type instructions to add to the location counter of the section
int lenOfConstant(string str){
	string tmp;
	int i=0;
	tmp=str;
    if (((int)('Z'-tmp[0]) ==2 || (int)('z'-tmp[0]) ==2) && tmp[1] =='\''){
		i = 1;
	}
	if ( ((int)(tmp[0]-'A')==2 || (int)(tmp[0]-'a')==2) && tmp[1] =='\''){
        i=2;
        while(i<=tmp.size()){
            if(tmp[i]=='\''){
                i=i-2;
                break;
            }
            i++;
        }
	}
	
	return i;
}

//used to add to the literal table vector defined
void add_LIITAB(string str){
    littab l;
    l.name=str;
    string tmp="";
    for(int i=1;i<str.length();i++){
        tmp+=str[i];
    }
    l.operand_value=valOfLiteral(tmp);
    l.addr=-1;
    l.length=lenOfConstant(tmp);
    LITTAB_FOUND=0;
    LITTAB[str]=l;
}

//search the ext table for def or r record as per the is_extdef vairable
void search_EXT(string str, bool is_extdef){
    EXT_FOUND=0;
    EXT_INDEX=-1;
    string token="";
    istringstream iss(str);
    while(getline(iss,token,',')){
        int i=0;
        while(i<EXT_COUNT){
            if(EXT[i].symbol==token ){
                if(EXT[i].control_section==curr_section){
                    EXT_FOUND=1;
                    EXT_INDEX = i;
                    break;
                }
            }
            i++;
        }
        
        if(EXT_FOUND!=1){
            EXT_INDEX = EXT_COUNT;
            EXT[EXT_INDEX].symbol=token;
            EXT[EXT_INDEX].type = is_extdef;
            EXT[EXT_INDEX].control_section = curr_section;
            EXT_COUNT++;
        }
    }
}

//When ORG is encountered, the assembler resets its LOCCTR to the specified value
//For each literal with empty address field, assign the address and update the LOC of curr section accordingly
void handle_LTORG(fstream &fptr){
    for(auto i:LITTAB){
		if(i.second.addr == -1){
            string n=i.second.name;
            LITTAB[n].addr = LOCTAB[curr_section].loc;
            fptr<<setfill('0')<<setw(4)<<right<<uppercase<<hex<<LOCTAB[curr_section].loc<<"\t"<<"*\t\t ";
            fptr<<setfill(' ')<<setw(8)<<left<<n<<"\n";
            LOCTAB[curr_section].loc += lenOfConstant(n.substr(1));
		}
	}
}

//increasing location counter as per the pseudo code for various instructions
void increase_loc(int& n){
	if(OPTAB_FOUND && (opcode=="CLEAR" || opcode=="COMPR" || opcode=="TIXR" )){
		n +=2;
	}
	else if(OPTAB_FOUND){
		if(is_extended_format) n +=4;
        else n +=3;
	}
	else if(opcode=="WORD"){
		n += 3;
	}
	else if(opcode=="RESB"){
		n += stoi(operand);
	}
	else if(opcode=="BYTE"){
		n += lenOfConstant(operand);
	}
    else if(opcode=="RESW"){
		n += 3*stoi(operand);
	}
    //cout<<n<<endl;
}

//breaking a line from the intermediate file into the address label opcode and operand
int readLinePass2(fstream &fptr){
    buffer="";
    getline(fptr,buffer);
    buffer+='\n';
    lineLength = buffer.length();
	buf_i=0;
    //Read LOCCTR
    string tmp="";
    for(;;){
		tmp += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    loc= stringToHex(tmp);
    buf_i+=1;
    //Read Label
    label="";
	for(;;){
		label += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    //Skip spaces
    for(;;){
        if(buffer[buf_i]!=' '&&buffer[buf_i]!='\t'){
            break;
        }
        buf_i++;
    }
    //Read Opcode
    opcode="";
    for(;;){
		opcode += buffer[buf_i++];
        if(buffer[buf_i]==' ') break;
        if(buffer[buf_i]=='\t') break;
        if(buffer[buf_i]=='\n') break;
	}
    //Skip spaces
    for(;;){
        if(buffer[buf_i]!=' '&&buffer[buf_i]!='\t'){
            break;
        }
        buf_i++;
    }
    //Read Operand
    operand="";
	while(buffer[buf_i] != ' ' && buffer[buf_i] !='\t' && buffer[buf_i] !='\n'){
		operand += buffer[buf_i++];
	}
    return lineLength;
}

//finding the format type from sic xe for creating the object code as per instructions
int format_type(string opc){
    if(opc=="COMPR"){
        return 2;
    }
    else if(opc=="CLEAR"){
        return 2;
    }
    else if(opc=="TIXR"){
        return 2;
    }
    else if(opc[0]=='+'){
        return 4;
    }
    else{
        if(optab.find(opc)==optab.end()){
            return 0;
        }
    }
    return 3;
}

//searching the loc table using the names of the sections
void search_LOCTAB(string str){
    LOCTAB_INDEX=-1;
	for(int i=0;i<LOCTAB_COUNT;i++){
		if(LOCTAB[i].section_name==str){
			LOCTAB_FOUND=1;
			LOCTAB_INDEX=i;
			break;
		}
	}
}

//pass1 of the assembler for sic xe
void pass1(){
    cout<<"Pass-1 is running"<<endl;
    string tmp_opcode;
    fstream input, intermediate;
	input.open("test_input.txt",ios::in);
    intermediate.open("intermediate.txt",ios::out);
    if(!intermediate.is_open()||!input.is_open()){
		printf("Cannot open text file\n");
		exit(1);
	}
    readLine(input);
    if(opcode=="START"){
        LOCTAB_COUNT++;
        LOCTAB[curr_section].start_addr = stringToHex(operand);
		LOCTAB[curr_section].loc = LOCTAB[curr_section].start_addr;
		LOCTAB[curr_section].section_name=label;
        intermediate<<setfill('0')<<setw(4)<<right<<uppercase<<hex<<LOCTAB[curr_section].loc<<"\t";
        //for printing in columns as required
        string paddedLabel=label;
        for(int i=label.size();i<8;i++){
            paddedLabel+=' ';
        }
        string paddedOperand=operand;
        for(int i=operand.size();i<8;i++){
            paddedOperand+=' ';
        }
        string paddedOpcode=opcode;
        for(int i=opcode.size();i<8;i++){
            paddedOpcode+=' ';
        }
        intermediate<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
		readLine(input);
	}
	else{
		LOCTAB[curr_section].start_addr=LOCTAB[curr_section].loc=0;
	}
    while(opcode!="END"){
        //if commented line then directly print into the intermediate file
        if(buffer[0]=='.'){
            intermediate<<buffer;
        }
        else if(buffer[0]=='\n'){
            intermediate<<buffer;
        }
        else{ //handle all operations required :
            if(opcode=="EQU"){
				bool expr_type,ref_involved;
				ref_involved = false;
                int val=0;
                expr_type=false;
                calculate_expr(operand, &val, &expr_type, &ref_involved); //calculate the operand
                // search the SYMTAB for the label and add if not present
                search_SYMTAB(label);
                if(!SYMTAB_FOUND){
                    add_SYMTAB(label);
                    SYMTAB[SYMTAB_INDEX].addr = val; // assign the value of operand
                    SYMTAB[SYMTAB_INDEX].type = expr_type; // assign the type of expression
                    
                }
                else{
                    SYMTAB[SYMTAB_INDEX].addr = val; // assign the value of operand
				    SYMTAB[SYMTAB_INDEX].type = expr_type; // assign the type of expression
                }
                intermediate<<setfill('0')<<setw(4)<<right<<hex<<val<<"\t";
                string paddedLabel=label;
                for(int i=label.size();i<8;i++){
                    paddedLabel+=' ';
                }
                string paddedOperand=operand;
                for(int i=operand.size();i<8;i++){
                    paddedOperand+=' ';
                }
                string paddedOpcode=opcode;
                for(int i=opcode.size();i<8;i++){
                    paddedOpcode+=' ';
                }
                intermediate<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
            }
            else if(opcode=="CSECT"){
                LOCTAB[curr_section].length = LOCTAB[curr_section].loc - LOCTAB[curr_section].start_addr;
				LOCTAB[LOCTAB_COUNT].loc=LOCTAB[LOCTAB_COUNT].start_addr=0;
				curr_section = LOCTAB_COUNT;
                LOCTAB[LOCTAB_COUNT].section_name=label;
				LOCTAB_COUNT++;
				intermediate<<setfill('0')<<setw(4)<<right<<uppercase<<hex<<LOCTAB[curr_section].loc<<"\t";
                string paddedLabel=label;
                for(int i=label.size();i<8;i++){
                    paddedLabel+=' ';
                }
                string paddedOperand=operand;
                for(int i=operand.size();i<8;i++){
                    paddedOperand+=' ';
                }
                string paddedOpcode=opcode;
                for(int i=opcode.size();i<8;i++){
                    paddedOpcode+=' ';
                }
                intermediate<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
                
            }
            else{
                if(label!=""){ //if there is a symbol in label
                    search_SYMTAB(label); // search for symbol in SYMTAB and if not present then add
					if(SYMTAB_FOUND==0){
                        add_SYMTAB(label);
                    }
                }
                if(opcode[0]=='+'){
                    is_extended_format=true;
                    tmp_opcode="";
                    for(int i=1;i<opcode.length();i++){
                        tmp_opcode+=opcode[i];
                    }

                }
                else{
					is_extended_format = false;
					tmp_opcode=opcode;
				}
                if(optab.find(tmp_opcode)==optab.end()) {//checking if theopcode exists in the table
                    OPTAB_FOUND=0;
                }
                else{
                    OPTAB_FOUND=1;
                }
                is_literal=false;
                if(operand[0]=='='){//literal
                    is_literal=true;
                    if(LITTAB.find(operand)!=LITTAB.end()){
                        LITTAB_FOUND=1;
                    }
                    else{
                        LITTAB_FOUND=0;
                    }
                    // find the literal, if not present add a entry
					if(LITTAB_FOUND==0){
                        add_LIITAB(operand);
                        LITTAB_INDEX++;
					}
                }
                if(opcode=="EXTDEF" || opcode=="EXTREF"){
                    if(opcode=="EXTDEF"){
                        search_EXT(operand, true);
                    }
                    else{
                        search_EXT(operand, false);
                    }
                    string paddedLabel=label;
                    for(int i=label.size();i<8;i++){
                        paddedLabel+=' ';
                    }
                    string paddedOperand=operand;
                    for(int i=operand.size();i<8;i++){
                        paddedOperand+=' ';
                    }
                    string paddedOpcode=opcode;
                    for(int i=opcode.size();i<8;i++){
                        paddedOpcode+=' ';
                    }
                    intermediate<<"\t\t"<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
				}
                else if(opcode=="LTORG"){
                    string paddedLabel=label;
                    for(int i=label.size();i<8;i++){
                        paddedLabel+=' ';
                    }
                    string paddedOperand=operand;
                    for(int i=operand.size();i<8;i++){
                        paddedOperand+=' ';
                    }
                    string paddedOpcode=opcode;
                    for(int i=opcode.size();i<8;i++){
                        paddedOpcode+=' ';
                    }
                    intermediate<<"\t\t"<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
					handle_LTORG(intermediate);
				}
				else{
					intermediate<<setfill('0')<<setw(4)<<right<<uppercase<<hex<<LOCTAB[curr_section].loc<<"\t";
                    string paddedLabel=label;
                    for(int i=label.size();i<8;i++){
                        paddedLabel+=' ';
                    }
                    string paddedOperand=operand;
                    for(int i=operand.size();i<8;i++){
                        paddedOperand+=' ';
                    }
                    string paddedOpcode=opcode;
                    for(int i=opcode.size();i<8;i++){
                        paddedOpcode+=' ';
                    }
                    intermediate<<paddedLabel<<paddedOpcode<<paddedOperand<<"\n";
				}
            }
            increase_loc(LOCTAB[curr_section].loc); // increase Location counter
        }
        readLine(input); //read next line
    }
    intermediate<<"\t\t"<<setfill(' ')<<setw(8)<<left<<label<<" "<<setw(8)<<left<<opcode<<" "<<setw(8)<<left<<operand<<"\n";// last line
	handle_LTORG(intermediate); //handle ltorg lines finally 
	LOCTAB[curr_section].length = LOCTAB[curr_section].loc - LOCTAB[curr_section].start_addr;
	cout<<"Pass-1 completed\n";
	cout<<"Number of lines read: "<<line_num<<endl;
    input.close();
    intermediate.close();
}


//pass2 of the assembler for sic xe
//creates listing file by appending the calculated object code to the end of the intermeidiate file 
//creates the output.obj which was H,T,R,D,M records for the given asm code
//modified.txt is empty at the end of pass2
void pass2(){
    record=object_code=define_record=refer_record=modi_record=tmp_opcode=tmp_operand="";
    int line_objectCode=line_opcode=line_index=line_addr=record_index=part_index=define_index=refer_index=modi_index=0;
    cout<<"Pass-2 started\n";
    fstream intermediate,output,modi;

    //read from the file created in pass1
    intermediate.open("intermediate.txt",ios::in);

    //creating the 2 output files. modified.txt is empty at the end as it is copied to the output file
    output.open("output.obj",ios::out);
    modi.open("modified.txt",ios::out);
    if(!intermediate.is_open()||!output.is_open()||!modi.is_open()){
        printf("Cannot open file\n");
        exit(1);
    }
    readLinePass2(intermediate);
    if(opcode=="START"){
        readLinePass2(intermediate);
    }
    output<<"H"<<setfill(' ')<<setw(6)<<left<<LOCTAB[curr_section].section_name;
    output<<setfill('0')<<setw(6)<<right<<uppercase<<hex<<LOCTAB[curr_section].start_addr;
    output<<setfill('0')<<setw(6)<<right<<uppercase<<hex<<LOCTAB[curr_section].length<<"\n";
    char buff[100];
    record_index+=sprintf(buff,"%s","T");
    record+=buff;
    record_index+=sprintf(buff,"%06X",loc);
    record+=buff;
    int start_loc = loc;
    for(;;){
        if(opcode=="END") break;
        OPTAB_FOUND=SYMTAB_FOUND=LITTAB_FOUND=0;
        line_objectCode=line_index=line_opcode=0;
        tmp_operand=operand;
        object_code="";
        tmp_opcode=opcode;

        //if commented line then skip
        if(buffer[0]=='.'||buffer[0]=='\n'){ 

        }
        else{
            if(label=="*"){ //creating the opcode as per diff formats of opcode
                if(LITTAB.find(opcode)!=LITTAB.end()){
                    LITTAB_FOUND=1;
                }
                else{
                    LITTAB_FOUND=0;
                }
                if(LITTAB_FOUND){
                    line_objectCode=LITTAB[opcode].operand_value;
                }
                sprintf(buff,"%X",line_objectCode);
                object_code=buff;
            }
            else if(format_type(opcode)==3){
                if(optab.find(opcode)!=optab.end()){
                    OPTAB_FOUND=1;
                }
                line_objectCode=stringToHex(optab[opcode]);
                line_objectCode <<=16;
                
                if(operand!=""){
                    if(operand[0]=='#'){ // Immediate
                        string operand_1="";
                        for(int i=1;i<operand.length();i++){
                            operand_1+=operand[i];
                        }
                        search_SYMTAB(operand_1);
                        if(SYMTAB_FOUND){ // OP #m
                            line_objectCode += SYMTAB[SYMTAB_INDEX].addr - (loc+3) + format3_i + format3_p;
                        }
                        else{ // OP #c
                            line_objectCode += stringToHex(operand_1) + format3_i;
                        }
                    }
                    else if(operand[0]=='@'){ // Indirect
                        string operand_1="";
                        for(int i=1;i<operand.length();i++){
                            operand_1+=operand[i];
                        }
                        search_SYMTAB(operand_1);
                        if(SYMTAB_FOUND){ // OP @m
                            line_index = (SYMTAB[SYMTAB_INDEX].addr-(loc+3));
                            line_index  = line_index & format3_neg;
                            line_objectCode += line_index + format3_p + format3_n;
                        }
                        else{
                             // OP @c
                             line_objectCode += stringToHex(operand_1) + format3_n;
                        }
                    }
                    else if(operand[0]=='='){
                        if(LITTAB.find(operand)!=LITTAB.end()){
                            LITTAB_FOUND=1;
                        }
                        else{
                            LITTAB_FOUND=0;
                        }
                        if(LITTAB_FOUND){
                            line_index = LITTAB[operand].addr - (loc+3);
                            line_index = line_index & format3_neg;
                            line_objectCode += line_index;
                            line_objectCode += format3_p + format3_n + format3_i;
                        }
                        else{
                            line_objectCode += format3_p + format3_n + format3_i;
                        }
                        
                    }
                    else if((int)('Z'-operand[operand.length()-1])==2 ){ // OP m,X
                        if(operand[operand.length()-2]==','){
                            line_objectCode += format3_n + format3_i + format3_x + format3_p;
                            operand[operand.length()-2]='\0';
                            search_SYMTAB(opcode);
                            if(SYMTAB_FOUND){
                                line_index = (SYMTAB[SYMTAB_INDEX].addr-(loc+3));
                                line_index  = line_index & format3_neg;
                                line_objectCode+= line_index;
                            }
                        }
                        else{
                            search_SYMTAB(operand);
                            line_objectCode += format3_p + format3_n + format3_i;
                            if(SYMTAB_FOUND){ // OP m
                                line_index = (SYMTAB[SYMTAB_INDEX].addr-(loc+3));
                                line_index  = line_index & format3_neg;
                                line_objectCode+= line_index;
                            }
                        }
                        
                    }
                    else{ // simple
                        search_SYMTAB(operand);
                        line_objectCode += format3_p + format3_n + format3_i;
                        if(SYMTAB_FOUND){ // OP m
                            line_index = (SYMTAB[SYMTAB_INDEX].addr-(loc+3));
                            line_index  = line_index & format3_neg;
                            line_objectCode+= line_index;
                        }
                    }


                }
                else{
                    line_objectCode += format3_i + format3_n;
                }
                sprintf(buff,"%06X",line_objectCode);
                object_code=buff;
            }
            else if(format_type(opcode)==2){
                if(optab.find(opcode)!=optab.end()){
                    OPTAB_FOUND=1;
                }
                line_objectCode=stringToHex(optab[opcode]);
                line_objectCode <<=8;
                string r1,r2;
                r1=operand[0];
                line_objectCode+= (reg[r1]<<4);
                if(operand[1]==','){
                    r2=operand[2];
                    line_objectCode+=reg[r2];
                }
                sprintf(buff,"%04X",line_objectCode);
                object_code=buff;
                //cout<<line_objectCode<<endl;
            }
            else if(format_type(opcode)==4){
                modi_index=sprintf(buff,"M%06X05+",loc+1);
                modi_record=buff;
                string opcode_1="";
                for(int i=1;i<opcode.size();i++){
                    opcode_1+=opcode[i];
                }
                if(optab.find(opcode_1)!=optab.end()){
                    OPTAB_FOUND=1;
                }
                line_objectCode=stringToHex(optab[opcode_1]);
                line_objectCode <<=24;
                line_objectCode +=format4_n + format4_i + format4_e;
                if(operand!=""){
                    //cout<<operand<<" here"<<endl;
                    if((int)('Z'-operand[operand.length()-1])==2 && operand[operand.length()-2]==','){ // +OP m,X
                        operand[operand.length()-2]='\0';
                        search_SYMTAB(opcode);
                        line_objectCode += format4_x;
                        //cout<<opcode<<endl;
                        char c[100];
                        int i=0;
                        while(i<operand.size()){
                            c[i]=operand[i];
                            i++;
                        }
                        c[i]='\0';
                        modi_index += sprintf(buff,"%-6s",c);
                        modi_record+=buff;
                        if(SYMTAB_FOUND){
                            line_index = (SYMTAB[SYMTAB_INDEX].addr-(loc+3));
                            line_objectCode += line_index & format3_neg;
                            line_index  = line_index & format3_neg;
                        }
                    }
                    else{
                        search_SYMTAB(operand);
                        string tmp="";
                        tmp=operand;
                        for(int i=tmp.size();i<6;i++){
                            tmp+=' ';
                        }
                        modi_record+=tmp;
                        if(SYMTAB_FOUND){
                            line_index += SYMTAB[SYMTAB_INDEX].addr;
                        }
                        else{
                            int i=0;
                            while(i<EXT_COUNT){
                                if(EXT[i].symbol==operand && EXT[i].control_section == curr_section){
                                    line_index=0;
                                }
                                i++;
                            }
                        }
                    }
                    //cout<<modi_record<<endl;
                    modi<<modi_record<<"\n";
                    
                }
                line_index &= format4_neg;
                    line_objectCode += line_index;
                    sprintf(buff,"%08X",line_objectCode);
                    object_code=buff;
            }
            else if(opcode=="CSECT"){ //start of a new section we print E and modification records
                search_LOCTAB(label);
                if(LOCTAB_FOUND){
                    record_index+=sprintf(buff,"%02X",(int)(part.length()/2));
                    record+=buff;
                    string tmp=part;
                    for(int i=part.size();i<6;i++){
                        tmp+=' ';
                    }
                    record+=tmp;
                    if(part!=""){
                        output<<record<<"\n";
                    }
                    modi.close();
                    modi.open("modified.txt",ios::in);
                    if(!modi.is_open()){
                        cout<<"Can't open modified.txt"<<endl;
                        return;
                    }
                    while(getline(modi,modi_record)){
                        output<<modi_record<<"\n";
                    }
                    modi.close();
                    modi.open("modified.txt",ios::out);
                    modi_record="";
                    modi_index=0;
                    if(curr_section==0){
                        output<<"E"<<setfill('0')<<setw(6)<<right<<uppercase<<hex<<LOCTAB[curr_section].start_addr<<"\n\n";
                    }
                    else{
                        output<<"E\n\n";
                    }
                    part="";
                    part_index=0;
                    curr_section++;
                    loc = LOCTAB[LOCTAB_INDEX].start_addr;
                    start_loc = LOCTAB[curr_section].start_addr;
                    record_index = 1;
                    record="T";
                    record_index += sprintf(buff,"%06X",loc);
                    record+=buff;
                    string str=LOCTAB[curr_section].section_name;
                    for(int i=str.size();i<6;i++){
                        str+=' ';
                    }
                    output<<"H"<<str;
                    output<<setfill('0')<<setw(6)<<right<<uppercase<<hex<<LOCTAB[curr_section].start_addr;
                    output<<setfill('0')<<setw(6)<<right<<uppercase<<hex<<LOCTAB[curr_section].length<<"\n";
                }
            }
            else if(opcode=="BYTE"){
                char tmp_ptr[32];
                line_objectCode=0;
                int tmp_index=0;
                if( ((int)(operand[0]-'A'==2) || (int)(operand[0]-'a'==2)) && operand[1]=='\''){
                    int i=2;
                    while(i<=operand.length()-2){
                        line_objectCode += (int)operand[i];
                        line_objectCode<<=8;
                        i++;
                    }
                    line_objectCode>>=8;
                    sprintf(buff,"%X",line_objectCode);
                    object_code+=buff;
                }
                else if( ((int)(operand[0]-'Z'==-2) || (int)(operand[0]-'z'==-2)) && operand[1]=='\''){
                    int i=2;
                    while(i<=operand.length()-2){
                        tmp_ptr[tmp_index++] = operand[i];
                        i++;
                    }
                    tmp_ptr[tmp_index] = '\0';
                    string c=tmp_ptr;
                    line_objectCode += stringToHex(c);
                    object_code+=tmp_ptr;
                }
            }
            else if(opcode=="WORD"){
                bool ref_involved,expr_type;
                ref_involved=false;
                expr_type=false;
                calculate_expr(operand, &line_index, &expr_type, &ref_involved);
                if(ref_involved){
                    modi_record="";
                    modi_index=0;
                    int prev_sign=1;
                    string tmp;
                    char buff[200];
                    bool found=false;
                    string str=operand;
                    for(int i=0;i<str.length();i++){
                        if(str[i]=='-'||str[i]=='+'){
                            found=false;
                            int k=0;
                            while(k<SYMTAB_COUNT){
                                if(SYMTAB[k].symbol==tmp){
                                    if(SYMTAB[k].control_section!=curr_section){
                                        modi_index = sprintf(buff,"M%06X06",loc);
                                        modi_record=buff;
                                        modi_index+=1;
                                        if(prev_sign==0){
                                            modi_record+='-';
                                        }
                                        else{
                                            modi_record+='+';
                                        }
                                        modi_index+=SYMTAB[k].symbol.length();
                                        modi_record+=SYMTAB[k].symbol;
                                        modi<<modi_record<<"\n";
                                    }
                                }
                                k++;
                            }
                            tmp="";
                            if(str[i]=='+'){
                                prev_sign=1;
                            }
                            else {
                                prev_sign=-1;
                            }
                        }
                        else{
                            tmp+=str[i];
                        }
                        
                    }
                    found=false;
                    int k=0;
                    while(k<SYMTAB_COUNT){
                        if(SYMTAB[k].symbol==tmp){
                            if(SYMTAB[k].control_section!=curr_section){
                                modi_index = sprintf(buff,"M%06X06",loc)+1;
                                modi_record=buff;
                                if(prev_sign==-1){
                                    modi_record+='-';
                                }
                                else{
                                    modi_record+='+';
                                }
                                modi_index+=SYMTAB[k].symbol.length();
                                modi_record+=SYMTAB[k].symbol;
                                modi<<modi_record<<"\n";
                            }
                        }
                        k++;
                    }
                }
                line_objectCode += line_index;
                sprintf(buff,"%06X",line_objectCode);
                object_code=buff;
            }
            if(opcode=="EXTDEF"){  //stores record w.r.t EXTDEF as D records
                define_index=1;
                define_record="D";
                string token="";
                istringstream iss(operand);
                while(getline(iss,token,',')){
                    search_SYMTAB(token);
                    if(SYMTAB_FOUND){
                        int n=SYMTAB[SYMTAB_INDEX].symbol.length();
                        define_index+=n;
                        define_record+=SYMTAB[SYMTAB_INDEX].symbol;
                        define_index += sprintf(buff,"%06X",SYMTAB[SYMTAB_INDEX].addr);
                        define_record+=buff;
                    }
                }
                output<<define_record<<"\n";
            }
            else if(opcode=="EXTREF"){ //stores record w.r.t EXTREF as R records
                refer_index=1;
                refer_record="R";
                string token="";
                istringstream iss(operand);
                while(getline(iss,token,',')){
                    string tmp=token;
                    for(int i=token.size();i<6;i++){
                        tmp+=' ';
                    }
                    refer_record+=tmp;
                }
                output<<refer_record<<"\n";
            }
            else if(opcode=="LTORG"|| (object_code.length()+part.length()>60)||(loc-start_loc)>27){
                // If the object code cannot fit into the text record
                //print it out to output and start a new text record
                record_index += sprintf(buff,"%02X",(int)(part.length()/2));
                record+=buff;
                record_index += part.length();
                record+=part;
                if(part.length()){
                    output<<record<<"\n";
                }
                part="";
                record_index=1;
                part_index=0;
                start_loc = loc;
                record="T";
                record_index += sprintf(buff,"%06X",loc);
                record+=buff;
            }
            part_index+=object_code.length();
            part+=object_code;
        }
        readLinePass2(intermediate);
    }
    while(1){
        readLinePass2(intermediate);
        line_objectCode=line_index=line_opcode=0;
        OPTAB_FOUND=LITTAB_FOUND=SYMTAB_FOUND=0;
        object_code="";
        tmp_opcode=opcode;
        tmp_operand=operand;
        if(buffer.length()<=1){
            break;
        }
        if(LITTAB.find(opcode)!=LITTAB.end()){
            LITTAB_FOUND=1;
        }
        else{
            LITTAB_FOUND=0;
        }
        if(LITTAB_FOUND){
            line_objectCode = LITTAB[opcode].operand_value;
            if((int)('z'-LITTAB[opcode].name[1])==2 || (int)('Z'-LITTAB[opcode].name[1])==2){
                char temp_ptr[32];
                int temp_index=0;
                temp_ptr[0] = '\0';
                for(int i=3;i<=opcode.length()-2; ){
                    temp_ptr[temp_index] = opcode[i];
                    temp_ptr[temp_index+1]='\0';
                    temp_index++;
                    i++;
                }
                sprintf(buff,"%s",temp_ptr);
                object_code+=buff;
            }
            else{
                sprintf(buff,"%X",line_objectCode);
                object_code+=buff;
            }
        }
        part_index += object_code.length();
        part+=object_code; // add object code
    }
    record_index += sprintf(buff,"%02X",(int)part.length()/2);
    record+=buff;
    record_index += part.size();
    record+=part;
    output<<record<<"\n";
    modi.close();
    modi.open("modified.txt",ios::in);
    if(!modi.is_open()){
        cout<<"Can't open modified.txt"<<endl;
        return;
    }
    while(getline(modi,modi_record)){
        output<<modi_record<<"\n";
    }
    modi_record="";
    modi_index=0;
    modi.close();
    output<<"E\n";
    output.close();
    intermediate.close();
    cout<<"Pass-2 completed"<<endl;
}

int main(){
    initialize();
    insertValuesOptabReg();
    pass1();
    //re-initialise some variables for pass2
    buffer=operand=opcode=label="";
    buf_i=line_num=lineLength=curr_section=0;
	SYMTAB_INDEX=LOCTAB_INDEX=REG_FOUND=REG_INDEX= -1;
    OPTAB_FOUND=LOCTAB_FOUND=SYMTAB_FOUND=LITTAB_FOUND=0;
	is_extended_format=is_literal=false;
    pass2();
    return 0;
}