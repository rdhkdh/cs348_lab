// Ridhiman Dhindsa, 210101088
#include <bits/stdc++.h>
using namespace std;

int LOCCTR;  //Location Counter
int start_address; //starting address of program
int programLength; //length of program
string label, opcode, operand;
bool extendedFlag; // =true if extended instr

ifstream fin; //input stream
ofstream fout; //output stream
string input_filename; //name of input file

string line; //string for storing a line of input
string word; //string for storing a word in a line
vector<string> word_list; //vector for storing all words in the input line

map<string,string> OPTAB; //Operation Code Table

//generates list of words present in the line read from input file
void generate_wordlist()
{
    word_list.clear();
    stringstream ss(line);
    while(ss>>word)
	{ word_list.push_back(word); }
    return;
}

//gets the OPTAB ready
void process_optab()
{
    while(!fin.eof())
	{
        getline(fin, line);
        generate_wordlist();
        OPTAB[word_list[0]] = word_list[1];
    }
    return;
}

/*error flags used for indicating error in input,
used in evaluateExpression() */
bool SYNTAX_ERROR;
bool UNDEFINED_SYMBOL;

/* Symbol Table - maps labels to addresses. Each control section has a separate symtab 
hashed by its name. Predefined symbols are registers. */
map<string, map<string,string>> SYMTAB;

//gets the SYMTAB ready
void process_symtab()
{
    while(!fin.eof())
	{
        getline(fin, line);
        generate_wordlist();
        map<string, string> temp;
        temp[word_list[1]]=word_list[2];
        SYMTAB[word_list[0]] = temp;
    }
    return;
}

map<string,string> LITTAB; //literal table
set<string> extref; //external references
vector<string> modif_records; //modifiction records
map<string,int> prog_names; //map storing (Program name, length) pairs
string progName;
int BASE; //Base value (0 by default)

//extends or shortens string to desired length
string make_len(string &str, int len, int posn, char c)
{
    string res = "";
    if(str.length() == len)
    { res = str; } //if string length = len return same string
    else if(str.length() > len)
    { res = str.substr(0, len); }  //if greater, return substring of length len
    else
	{
        res = str;
        if(posn == 0) //append at the beginning
		{
            reverse(res.begin(), res.end());
            while(res.length() < len)
			{ res.push_back(c); }
            reverse(res.begin(), res.end());
        }
        else //append at the end
		{
            while(res.length() < len)
			{ res.push_back(c); }
        }
    }
    return res;
}

//Remove whitespace from beginning and end of string
string stripString(string s)
{
	//Removing spaces from end
	while(!s.empty() && s.back()==' ') {s.pop_back();}
	if(s.empty()) {return s;}

	//Removing spaces from beginning
	reverse(s.begin(), s.end());
	while(!s.empty() && s.back()==' ') {s.pop_back();}
    reverse(s.begin(), s.end());

	return s;
}

//Extract opcode, label, operand from a line
bool extract_line(string &line, string &label, string &opcode, string &operand, bool &extendedFlag)
{
    //if line comprises only of spaces
	if(stripString(line).empty()) {return false;}

	//Checking if valid line and not comment
	if((line.size()>80 || line.size()<21) && stripString(line)[0]!='.')
	{ return false; } 
	else if(stripString(line)[0]=='.')
	{
		return true; //Comments are still printed in intermediate
	}

	line = make_len(line, 80, 1, ' '); //Adding spaces at the end of line to make it 80 characters long.

	label=stripString(line.substr(0,10)); //obtain appropriate substring and then strip whitespace
	opcode=stripString(line.substr(10,10));
	operand=stripString(line.substr(20));

	//if extended format
	if(opcode[0]=='+')
	{
		extendedFlag=true;
		reverse(opcode.begin(), opcode.end());
		opcode.pop_back(); //remove + symbol
		reverse(opcode.begin(), opcode.end());
	}
	else { extendedFlag=false; }

	return true;
}

// hexadecimal digits
char hex_code[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

//converts decimal int to hex string
string dec_to_hex(int num)
{
    if(num == 0) {return "0";}

    string res = "";
    while(num)
	{
        res.push_back(hex_code[num%16]);
        num = num/16;
    }
    reverse(res.begin(), res.end());
    return res;
}

//converts hex string to decimal int
int hex_to_dec(string hex)
{
	int res=0;
	for(int i=0; i<hex.length(); i++)
	{
		if(hex[i]>='A' && hex[i]<='F')
        { res = res*16 + hex[i] - 'A' + 10; }
		else
        { res = res*16 + hex[i] - '0'; }
	}
	return res;
}

//checks if string contains only digits
bool is_number(string s)
{
	for(auto u: s)
	{
		if(u <'0' || u >'9') {return false;}
	}
	return true;
}

//Break a byte into its hexadecimal parts
string getHexBreakup(string s)
{
	string temp="";
	for(int i=0; i<s.length(); i++)
	{
		//Break into two hexadecimal codes
		int x=(s[i]>>4); //obtains the higher 4 bits
		int y=s[i]-(x<<4); //obtains the lower 4 bits
		temp.append(dec_to_hex(x)); //convert x to hex and append
		temp.append(dec_to_hex(y)); ////convert y to hex and append
	}
	return temp;
}

//provides appropriate padding for pretty printing
string format_line(string label, string opcode, string operand)
{
	while(label.length()<10) {label+=' ';}
	while(opcode.length()<10) {opcode+=' ';}
	while(operand.length()<60) {operand+=' ';}
	return label+opcode+operand;
}

/* function for handling errors in input file
TYPE 1 - SYNTAX ERROR
TYPE 2 - UNDEFINED SYMBOL 
TYPE 3 - INVALID OPCODE 
TYPE 4 - COMMA WARNING
TYPE 5 - DUPLICATE SYMBOL 
TYPE 6 - ILLEGAL EXTDEF 
TYPE 7 - DISPLACEMENT OUT OF BOUNDS 
TYPE 8 - LITTAB VALUES NOT DEFINED  */
void error_handling(int type, string line = "")
{
	switch(type)
	{
		case 1: 
		cout<<"SYNTAX ERROR, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        cout<<"Notes for SYNTAX:\n";
        cout<<"1) All lines should be between 21 and 80 columns (inclusive) in length.\n";
        cout<<"2) 10 columns for LABEL, 10 columns for OPCODE and 60 columns for OPERAND.\n";
        cout<<"3) BYTE operand should be inside X\'\' or C\'\'\n";
        exit(0);

		case 2: 
		cout<<"UNDEFINED SYMBOL, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        exit(0);

		case 3: 
		cout<<"INVALID OPERATION CODE, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        exit(0);

		case 4: 
		cout<<"COMMA WARNING, line no."<<line<<endl;
        cout<<"In order to fix it, do indexed addressing as follows: OPCODE <SYMBOL>,X\n";
		exit(0);

		case 5:
		cout<<"DUPLICATE SYMBOL, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        exit(0); 

		case 6: 
		cout<<"ILLEGAL EXTDEF, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        exit(0);

		case 7: 
		cout<<"DISPLACEMENT OUT OF BOUNDS, line no."<<line<<endl;
        cout<<"Execution aborted\n";
        exit(0);

		case 8: 
		cout<<"LITTAB VALUES NOT DEFINED\n";
        cout<<"Execution aborted\n";
        cout<<"Please use LTORG before CSECT\n";
        exit(0);
	}
}

vector<string> postFix; //stores postfix form of expression
stack<char> st; //used in converting expression to its postfix form
string temp;

/* Evaluate expressions for EQU, WORD and BASE. In the case of WORD, 
EXTREF symbols can also be used. Not so in the case of BASE and EQU */
int evaluateExpression(string s,bool sameProgramFlag,int recordStartAddress, int len)
{
	map<char,int> precedence; //map of character precedence
	precedence['*'] = 1;
	precedence['/'] = 1;
	precedence['+'] = 0;
	precedence['-'] = 0;

	//clear global variables
    postFix.clear();
    while(!st.empty()) { st.pop(); }

    //------------------------First converting to postfix-----------------------------------
	int leng = s.length();
	for(int i=0; i<leng; i++)
	{
		if(s[i]!='*' && s[i]!='/' && s[i]!='+' && s[i]!='-')
		{ temp.push_back(s[i]); }
		else 
		{
			postFix.push_back(temp);
    		temp.clear();
			while( st.size()>0 && precedence[s[i]]<precedence[st.top()] )
			{
				char c=st.top();
				st.pop();
				temp.push_back(c);
				postFix.push_back(temp);
    			temp.clear();
			}
			st.push(s[i]);
		}
	}
	if(temp.size() > 0)
	{
		postFix.push_back(temp);
    	temp.clear();
	}
	while(st.size() > 0)
	{
		char c=st.top();
		temp.push_back(c);
		st.pop();
		postFix.push_back(temp);
    	temp.clear();
	}

	//-----------------------------Evaluate postfix---------------------------------
	stack<string> st2;
	for(int i=0;i<postFix.size();i++)
	{
		if(postFix[i]!="*" && postFix[i]!="/" && postFix[i]!="+" && postFix[i]!="-")
		{
			temp=postFix[i];
			st2.push(temp);
		} 
		else 
		{
			string t2= st2.top(); st2.pop();
			string t1= st2.top(); st2.pop();
			int x=0, y=0;
			//To check if they are in the same program or should be externally fetched.
			bool xFound=false, yFound=false;

			//If t1 is a declared symbol
			if(SYMTAB.count(t1))
			{
                //same program
				if(SYMTAB[t1].count(progName))
				{
					xFound=true;
					string hex_num = SYMTAB[t1][progName];
					x=hex_to_dec(hex_num);
				} 
				else if(SYMTAB[t1].count("PREDEFINED"))
				{ //Register values
					xFound=true;
					x=hex_to_dec(SYMTAB[t1]["PREDEFINED"]);
				} 
				else if(extref.find(t1)==extref.end())
				{ //Not present in EXTERNAL REFERENCE symbols. Current program cannot access.
					UNDEFINED_SYMBOL=true;
					return -1;
				}
			} 
			else if(is_number(t1))
			{ //if it is an integer
				xFound=true;
				x=stoi(t1);
			} 
			else 
			{ //It hasn't been declared anywhere nor is it an integer.
				UNDEFINED_SYMBOL=true;
				return -1;
			}

            //If t2 is a declared symbol
			if(SYMTAB.count(t2))
			{
			    //same program
				if(SYMTAB[t2].count(progName))
				{
					yFound=true;
					y=hex_to_dec(SYMTAB[t2][progName]);
				} 
				else if(SYMTAB[t2].count("PREDEFINED"))
				{ //Register values
					yFound=true;
					y=hex_to_dec(SYMTAB[t2]["PREDEFINED"]);
				} 
				else if(extref.find(t2)==extref.end())
				{ //Not present in EXTERNAL REFERENCE symbols. Current program cannot access.
					UNDEFINED_SYMBOL=true;
					return -1;
				}
			} 
			else if(is_number(t2))
			{ //if it is an integer
				yFound=true;
				y=stoi(t2);
			} 
			else 
			{ //It hasn't been declared anywhere nor is it an integer.
				UNDEFINED_SYMBOL=true;
				return -1;
			}
            /*For EQU and BASE, the labels should be declared in the same section.
            Therefore a flag is passed to indicate this. */
			if(sameProgramFlag && (!xFound || !yFound))
			{
				UNDEFINED_SYMBOL=true;
				return -1;
			}

			//------------------evaluate by operator---------------------------
			if(postFix[i][0] == '+')
			{
                st2.push(to_string(x+y));
                //if symbol is not found, make modification records
                if(!xFound) 
				{
                    string m = "M"; //modifn record
                    string m_start_addr = dec_to_hex(recordStartAddress); //get start address
                    m_start_addr = make_len(m_start_addr, 6, 0, '0'); //fix length to 6 places
                    string m_len = dec_to_hex(len); //get length
                    m_len = make_len(m_len, 2, 0, '0'); //fix length to 2 places
                    string str = m_start_addr+m_len+ "+" +t1; //final record
                    m.append(str);
                    modif_records.push_back(m);
                }
                if(!yFound)
				{
                    string m = "M";
                    string m_start_addr = dec_to_hex(recordStartAddress);
                    m_start_addr = make_len(m_start_addr, 6, 0, '0');
                    string m_len = dec_to_hex(len);
                    m_len = make_len(m_len, 2, 0, '0');
                    string str = m_start_addr+m_len+ "+" +t2;
                    m.append(str);
                    modif_records.push_back(m);
                }
            }
            else if(postFix[i][0] == '-')
			{
                st2.push(to_string(x-y));
                //if symbol is not found, make modification records
                if(!xFound){
                    string m = "M";
                    string m_start_addr = dec_to_hex(recordStartAddress);
                    m_start_addr = make_len(m_start_addr, 6, 0, '0');
                    string m_len = dec_to_hex(len);
                    m_len = make_len(m_len, 2, 0, '0');
                    string str = m_start_addr+m_len+ "+" +t1;
                    m.append(str);
                    modif_records.push_back(m);
                }
                if(!yFound){
                    string m = "M";
                    string m_start_addr = dec_to_hex(recordStartAddress);
                    m_start_addr = make_len(m_start_addr, 6, 0, '0');
                    string m_len = dec_to_hex(len);
                    m_len = make_len(m_len, 2, 0, '0');
                    string str = m_start_addr+m_len+ "-" +t2;
                    m.append(str);
                    modif_records.push_back(m);
                }
            }
            else if(postFix[i][0] == '*')
			{
                st2.push(to_string(x*y));
                //EXTREF is not allowed here
                if(!xFound || !yFound)
				{
                    SYNTAX_ERROR=true;
                    return -1;
                }
            }
            else if(postFix[i][0] == '/')
			{
                st2.push(to_string(x/y));
                //EXTREF is not allowed here
                if(!xFound || !yFound)
				{
                    SYNTAX_ERROR=true;
                    return -1;
                }
            }
		}
	}
    //if the top of st2 contains an integer
	if( is_number(st2.top()) )
	{
		int num = stoi(st2.top());
		return num;
	} 
	else 
	{
		//if the value is a symbol
		if(SYMTAB.count(st2.top())==0)
		{ //If not in SYMTAB, UNDEFINED SYMBOL
			UNDEFINED_SYMBOL=true;
			return -1;
		} 
		else if(SYMTAB[st2.top()].count(progName))
		{ //If it is in SYMTAB and in the current section
			return hex_to_dec(SYMTAB[st2.top()][progName]);
		} 
		else if( extref.count(st2.top()) )
		{ 
			//If it is in EXTREF SYMBOLS
			if(sameProgramFlag)
			{ //FOR BASE and EQU, this isn't allowed
				UNDEFINED_SYMBOL=true;
				return -1;
			}
			string m = "M"; //write modification record
            string m_start_addr = dec_to_hex(recordStartAddress);
            m_start_addr = make_len(m_start_addr, 6, 0, '0');
            string m_len = dec_to_hex(len);
            m_len = make_len(m_len, 2, 0, '0');
            string str = m_start_addr + m_len;
            m.append(str);
            m.append("+");
            m.append(st2.top());
            modif_records.push_back(m);
			return 0;
		} 
		else 
		{ //it is outside the scope of current program
			UNDEFINED_SYMBOL=true;
			return -1;
		}
	}
}

//handles errors in file handling
void handle_file_error(int type, string filename)
{
    if(type == 1)
        cout<<"File "<<filename<<" not found.\n";
    else
        cout<<"Creation of "<<filename<<" failed\n";
    cout<<"Exiting program.\n";
    exit(0);
}

//writes given string to output file
void output_line(string &line)
{
    fout<<line<<endl;
    return;
}

//checks if the given line is empty
bool isEmpty()
{
    for(char u: line)
	{
        if(u != ' ') {return false;}
    }
    return true;
}

//reads a line from input file
void input_line()
{
    while(1)
	{
        getline(fin, line);
        if(!isEmpty()) {break;}
    }
    return;
}

//returns true if both strings are equal 
bool isEQUAL(string a, string b)
{
    if(a == b) {return true;}
    return false;
}

//------------------------------------main()------------------------------------------
int main(int argc, char* argv[])
{
    //Initializing global variables
	LOCCTR=0;
    start_address=0; programLength=0;
    label=""; opcode=""; operand=""; progName="";
    extendedFlag=false;
    SYNTAX_ERROR=false; UNDEFINED_SYMBOL=false;
    input_filename = "assembler_input.txt";

	if(argc >= 2) { //if input filename has been provided
        input_filename = argv[1];
        cout<<"Using input filename: "<<input_filename<<"\n";
    }
    else { //if input filename hasn't been provided
        cout<<"Input filename not specified.\n";
        cout<<"Using default input filename: "<<input_filename<<"\n";
    }

    fin.open("opcodes.txt"); //opening opcodes.txt
    if(!fin) { handle_file_error(1, "opcodes.txt"); }
    process_optab(); //fill OPTAB
    fin.close();

    fin.open("symbols.txt"); //opening symbols.txt
    if(!fin) { handle_file_error(1, "symbols.txt"); }
    process_symtab(); //fill SYMTAB
    fin.close();

	fin.open(input_filename); //Open input file passed as argument.
	if(!fin) { handle_file_error(1, input_filename); }
	
	fout.open("assembler_intermediate.txt"); //Intermediate file
	if(!fout) { handle_file_error(2, "assembler_intermediate.txt"); }

    //-------------------------------Pass 1 begins-----------------------------------
	while(getline(fin,line,'\n'))
	{
		//If line is empty, skip it.
		if(stripString(line).empty()) { continue; }

		//If line is a comment, print it.
		else if(stripString(line)[0]=='.') {
		    string line_out = "              " + line;
			output_line(line_out);
			continue;
		}
		//Extract opcode, operand and label from line. If line is invalid, skip it.
		if( !extract_line(line,label,opcode,operand,extendedFlag) ) {
			if(stripString(line).length()!=0) {
				error_handling(1, line); //If input format is not valid
			}
			continue;
		}
		//If the current line contains an END instruction
		if(opcode=="END") {
			string hexa = dec_to_hex(LOCCTR);
			hexa = make_len(hexa, 6, 0, '0');
			fout<<hexa<<" 0      "<<line<<'\n'; //print output
			break;
		}

        //If the current line contains a START instruction
		if(opcode=="START") {
			progName = label; //extract program name
			start_address = hex_to_dec(operand); //extract start address (default=0)
			operand = make_len(operand, 6, 0, '0');
			LOCCTR = start_address;
			fout<<operand<<" 0      "<<line<<'\n';
			continue;
		}

        //CSECT denotes the start of a new section
		if(opcode=="CSECT") { 
			prog_names[progName] = LOCCTR-start_address; //store prev program length
			LOCCTR=0;
			start_address=0; //New start address
			progName=label; //New program name

			string empty_string = "";
			output_line(empty_string);

			//write CSECT line to intermediate file
			string l = "000000 0      " + line;
			output_line(l);

			map<string, string>::iterator itr;
			for(itr = LITTAB.begin(); itr!=LITTAB.end(); itr++) {
                //If any literal from the previous program hasn't been assigned a value show an error message
				if(itr->second=="00000000") { error_handling(8); }
			}
		}

		string hexLOCCTR=dec_to_hex(LOCCTR); //Hex value of LOCCTR
		hexLOCCTR = make_len(hexLOCCTR, 6, 0, '0');

		//If line is not comment
		if(stripString(line)[0]!='.'){
			//If there is a label, add that label to SYMTAB. If duplicate, set error flag.
			if(label.length() > 0){
				if(SYMTAB.count(label) == 0){
					SYMTAB[label][progName]=dec_to_hex(LOCCTR);
				} else {
					if(SYMTAB[label].count(progName) || SYMTAB[label].count("PREDEFINED")){
						//it is a duplicate symbol
						//display an error message
						error_handling(5, line);
					} else {
						if(isEQUAL("EQU",opcode)){
							if(isEQUAL("*", operand)){
                                //the value of LOCCTR has to be used
								SYMTAB[label][progName]=dec_to_hex(LOCCTR);
							} else {
							    //only symbols within the current section are relevant.
								int expressionValue=evaluateExpression(operand,true,0,0);
								//if function encountered some error
								if(expressionValue==-1){
								    //if it is syntax error
									if(SYNTAX_ERROR){
										error_handling(1, line);
									}
                                    //if it is undefined symbol error
									if(UNDEFINED_SYMBOL){
										error_handling(2, line);
									}
								}
								SYMTAB[label][progName]=dec_to_hex(expressionValue);
							}
						} else {
						    //store the address otherwise
							SYMTAB[label][progName]=dec_to_hex(LOCCTR);
						}
					}
				}
			}
			if(OPTAB.count(opcode)){
				if(isEQUAL("COMPR",opcode) || isEQUAL("CLEAR",opcode) || isEQUAL("TIXR",opcode)){
                    //instruction format 2
					LOCCTR+=2;
					fout<<hexLOCCTR;
					fout<<' ';
					fout<<'2';
					for(int i=0;i<6;i++)
                        fout<<' ';
				} else {
					if(extendedFlag){
					    //instruction format 4
						LOCCTR+=4;
						fout<<hexLOCCTR;
                        fout<<' ';
                        fout<<'4';
                        for(int i=0;i<6;i++)
                            fout<<' ';
					} else {
					    //instruction format 3
						LOCCTR+=3;
						fout<<hexLOCCTR;
                        fout<<' ';
                        fout<<'3';
                        for(int i=0;i<6;i++)
                            fout<<' ';
					}
				}
                //literal
				if(operand[0]=='='){
					if(LITTAB.count(operand) == 0){
					    //mapping it to default value (00000000)
						LITTAB[operand]="00000000";
					}
				}

			} else {
				if(isEQUAL("WORD",opcode)){
                    //A word takes up 3 bytes
					LOCCTR+=3;
					fout<<hexLOCCTR;
                    fout<<' ';
                    fout<<'3';
                    for(int i=0;i<6;i++)
                        fout<<' ';
				}
				else if(isEQUAL("RESW", opcode)){
				    //A word takes up 3 bytes
                    int num = 3*stoi(operand);
					LOCCTR+=num;
					string lengthOfOperand = to_string(num);
					lengthOfOperand = make_len(lengthOfOperand, 7, 1, ' ');
					fout<<hexLOCCTR;
					fout<<' ';
					fout<<lengthOfOperand;
				}
				else if(isEQUAL("RESB", opcode)){
                    int num = stoi(operand);
					LOCCTR+=num;
					string lengthOfOperand = to_string(num);
					lengthOfOperand = make_len(lengthOfOperand, 7, 1, ' ');
					fout<<hexLOCCTR;
					fout<<' ';
					fout<<lengthOfOperand;
				}
				else if(isEQUAL("BYTE", opcode)){
					//Minimum 3 characters (C'' or X'')
					if(operand.size()<3){
						//display syntax error
						error_handling(1, line);
					}
					//if operand is hexadecimal string
                    else if(operand[0]=='X' && operand[1]=='\'' && operand.back()=='\''){
						int intermediate=operand.length()-3;
						//cannot have odd length
						//apply a 0 at the end to make the length even
						//display a message to the user showing the changes
						if(intermediate&1){
							cout<<"HEXADECIMAL BYTE OPERAND HAS ODD LENGTH. PADDING WITH 0.\n";
							operand.pop_back();
							operand+="0\'";
							intermediate++;
							line=format_line(label,opcode,operand);
						}
						int num = intermediate>>1;
                        LOCCTR+=num;
                        string lengthOfOperand = to_string(num);
						lengthOfOperand = make_len(lengthOfOperand, 7, 1, ' ');
						fout<<hexLOCCTR;
						fout<<' ';
						fout<<lengthOfOperand;
						intermediate = num;
					}
					//if operand is character string
					else if(operand[0]=='C' && operand[1]=='\'' && operand.back()=='\''){
						//Length of the character string is its size in bytes
						int num = operand.length()-3;
						LOCCTR+=num;
						string lengthOfOperand = to_string(num);
						lengthOfOperand = make_len(lengthOfOperand, 7, 1, ' ');
						fout<<hexLOCCTR;
						fout<<' ';
						fout<<lengthOfOperand;
					} else {
					    //First character should either be C or X.
					    //display syntax error
						error_handling(1, line);
					}
				} else if(isEQUAL("LTORG", opcode)){
					string line_out = hexLOCCTR + " 0      " + line;
					output_line(line_out);
					//Assign addresses to all Literals in LITTAB
					map<string, string>::iterator itr;
					for(itr = LITTAB.begin(); itr!= LITTAB.end(); itr++){
						if(!isEQUAL("00000000", itr->second)){
							continue;
						}
						operand=itr->first;
						reverse(operand.begin(), operand.end());
						operand.pop_back();
						reverse(operand.begin(), operand.end());
						if(operand.length() <= 2){
                            //Minimum characters=3 (C'' or X'')
                            //display syntax error
							error_handling(1, line);
						} else if(operand[0]=='X' && operand[1]=='\'' && operand.back()=='\''){
							int intermediate=operand.length()-3;
                            //cannot have odd length
                            //apply a 0 at the end to make the length even
                            //display a message to the user showing the changes
							if(intermediate&1){
								cout<<"HEXADECIMAL BYTE OPERAND HAS ODD LENGTH. PADDING WITH 0.\n";
								operand.pop_back();
								operand+="0\'";
								intermediate++;
							}
							//Make an entry in LITTAB
							LITTAB[itr->first]=dec_to_hex(LOCCTR);
							line=format_line("*",itr->first,"");
							int num = intermediate/2;
							string hex=dec_to_hex(LOCCTR);
							hex = make_len(hex, 6, 0, '0');
							string len=to_string(num);
							len = make_len(len, 7, 1, ' ');
							string line_out = hex + " " + len + line;
							output_line(line_out);
							LOCCTR+=num;
							intermediate = num;
						} else if(operand[0]=='C' && operand[1]=='\'' && operand.back()=='\''){
						    //Make an entry in LITTAB
							LITTAB[itr->first]=dec_to_hex(LOCCTR);
							line=format_line("*",itr->first,"");
							string hex=dec_to_hex(LOCCTR);
							hex = make_len(hex, 6, 0, '0');
							int num = operand.length()-3;
							LOCCTR+=num;
							string len=to_string(num);
							len = make_len(len, 7, 1, ' ');
							string line_out = hex + " " + len + line;
							output_line(line_out);
						} else {
							//First character should either be C or X.
							//display syntax error
							error_handling(1, line);
						}
					}
					continue;
				} else if(isEQUAL("EQU", opcode) || isEQUAL("EXTREF", opcode) ||
                            isEQUAL("EXTDEF", opcode) || isEQUAL("BASE", opcode)){
					fout<<hexLOCCTR<<" 0      ";
				} else if(isEQUAL("CSECT", opcode)){
                    continue;
                } else {
                    //opcode is not known
					error_handling(3, line);
				}
			}
		}
		//If comment, remove leading and trailing spaces.
		else
            line=stripString(line);
		output_line(line);
	}

    //Literal pool at the end of the program
    map<string, string>::iterator itr;
	for(itr = LITTAB.begin(); itr!=LITTAB.end(); itr++){
		if(!isEQUAL("00000000", itr->second)){
			continue;
		}
		operand=itr->first;
		reverse(operand.begin(), operand.end());
		operand.pop_back();
		reverse(operand.begin(), operand.end());
		if(operand.length() <= 2){
			//Minimum 3 characters (C'' or X'')
			//display syntax error
			error_handling(1, line);
		} else if(operand[0]=='X' && operand[1]=='\'' && operand.back()=='\''){
			int intermediate=operand.length()-3;
			//cannot have odd length
            //apply a 0 at the end to make the length even
            //display a message to the user showing the changes
			if(intermediate&1){
				cout<<"HEXADECIMAL BYTE OPERAND HAS ODD LENGTH. PADDING WITH 0.\n";
				operand.pop_back();
				operand+="0\'";
				intermediate++;
			}
			//Make an entry in LITTAB
			LITTAB[itr->first]=dec_to_hex(LOCCTR);
			line=format_line("*",itr->first,"");
			string hex=dec_to_hex(LOCCTR);
			hex = make_len(hex, 6, 0, '0');
			int num = intermediate/2;
			string len=to_string(num);
			len = make_len(len, 7, 1, ' ');
			string line_out = hex + " " + len + line;
			output_line(line_out);
			LOCCTR+=num;
		} else if(operand[0]=='C' && operand[1]=='\'' && operand.back()=='\''){
		    //Make an entry in LITTAB
			LITTAB[itr->first]=dec_to_hex(LOCCTR);
			line=format_line("*",itr->first,"");
			string hex=dec_to_hex(LOCCTR);
			hex = make_len(hex, 6, 0, '0');
			int num = operand.length()-3;
			string len=to_string(num);
			len = make_len(len, 7,  1, ' ');
			string line_out = hex + " " + len + line;
			output_line(line_out);
			LOCCTR+=num;
		} else {
		    //First character should either be C or X.
		    //display syntax error
			error_handling(1, line);
		}
	}
	prog_names[progName] = LOCCTR-start_address; //store prev program length

	fin.close(); //Close input and output file streams
	fout.close(); //Pass 1 ends

	//------------------------------------Pass 2 begins------------------------------------------------

	//Read from intermediate file and write object code to output file.
	fin.open("assembler_intermediate.txt");
	if(!fin){
        handle_file_error(1, "assembler_intermediate.txt");
	}
	fout.open("assembler_output.txt");
	if(!fout){
        handle_file_error(2, "assembler_output.txt");
	}

    //Flag used to write end symbol
	bool CSECT_flag=false;
	//Current start address
	int cur_start_addr=0;
	//string for storing current text record
	string textRecord;

    while(getline(fin,line,'\n')){
        //if current line is empty, skip it.
		if(stripString(line).empty()){
			continue;
		}
        //if current line is a comment, skip it.
		if(stripString(line)[0]=='.'){
            continue;
		}
		//address of instruction
		string addr=line.substr(0,6);
		//length of instruction
		string len=line.substr(7,6);
		//instruction
		line=line.substr(14);

		//extract label, opcode, operand, extendedFlag from line
		extract_line(line,label,opcode,operand,extendedFlag);

        //if current line contains START instruction
        //Get the name of first section and the starting address
		if(opcode=="START"){
			progName=label;
			//Create Header record;
			string newProgName=label;
			newProgName = make_len(newProgName, 6, 1, ' ');

			extref.clear();
			//set LOCCTR to starting address
			LOCCTR=hex_to_dec(operand);
			cur_start_addr=hex_to_dec(operand);
			start_address=hex_to_dec(operand);

			string startAddressString=operand;
			startAddressString = make_len(startAddressString, 6, 0, '0');
			//length of first section
			int progLength=prog_names[progName];
			string progLengthString=dec_to_hex(progLength);
			progLengthString = make_len(progLengthString, 6, 0, '0');
			string line_out = "H" + newProgName + startAddressString + progLengthString;
			output_line(line_out);
		}
		//CSECT denotes the beginning of a new section
		if(opcode=="CSECT"){
            //If text record contains some information, write it to object file
            //then erase its contents
            int current_len = textRecord.length();
			if(current_len){
				string cur_start_addr_string = dec_to_hex(cur_start_addr);
				cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
				int cur_record_len = LOCCTR-cur_start_addr;
				string record_len=dec_to_hex(cur_record_len);
				record_len = make_len(record_len, 2, 0, '0');
				string line_out = "T" + cur_start_addr_string + record_len + textRecord;
				output_line(line_out);
				textRecord.clear();
			}

            //write modification records to file, if any
            //then erase the records
			for(int i=0; i<modif_records.size(); i++){
				output_line(modif_records[i]);
			}
			modif_records.clear();

            //if previous section was the first section
            //write the END record
			if(CSECT_flag == false){
				string startAddressString=to_string(start_address);
				startAddressString = make_len(startAddressString, 6, 0, '0');
				string line_out = "E" + startAddressString;
				output_line(line_out);
			} else {
			    string line_out = "E";
				output_line(line_out);
			}
			string empty_string = "";
			output_line(empty_string);
			//clear set of EXTREF symbols
			extref.clear();
			//set start address and current start address to 0
			start_address=0;
			cur_start_addr=0;
			//set values of progName and newProgName to value contained in label
			progName=label;
			string newProgName=label;
			//set CSECT flag to true
			CSECT_flag=true;
			//set value of LOCCTR to 0
			LOCCTR=0;
			//write header record
			newProgName = make_len(newProgName, 6, 1, ' ');
			int progLength=prog_names[label];
			string progLengthString=dec_to_hex(progLength);
			progLengthString = make_len(progLengthString, 6, 0, '0');
			string line_out = "H" + newProgName + "000000" + progLengthString;
			output_line(line_out);
		}

		if(isEQUAL("EXTDEF", opcode)){
            //Identify individual symbols
			vector<string> definitions;
			string temporary="";
			for(int i=0;i<operand.length();i++){
				if(operand[i]==','){
                    definitions.push_back(stripString(temporary));
					temporary.clear();
				} else {
				    temporary.push_back(operand[i]);
				}
			}
			if(temporary.size() > 0){
				definitions.push_back(stripString(temporary));
				temporary.clear();
			}

            //Write Define records
			fout<<'D';
			for(int i=0;i<definitions.size();i++){
				if(SYMTAB.count(definitions[i])==0 || SYMTAB[definitions[i]].count(progName)==0){
					//The symbol must be defined in this program.
					//Display ILLEGAL EXTDEF error
					error_handling(6, line);
				} else {
				    //Write into Define record
					string temp=definitions[i];
					temp = make_len(temp, 6, 1, ' ');
					string addrString = SYMTAB[temp][progName];
					addrString = make_len(addrString, 6, 0, '0');
					fout<<temp;
					fout<<addrString;
				}
			}
			string empty_string = "";
			output_line(empty_string);
			continue;
		} else if(isEQUAL("EXTREF",opcode)){
		    //Identify individual symbols
			vector<string> definitions;
			string temporary="";
			for(int i=0;i<operand.length();i++){
				if(operand[i]==','){
					definitions.push_back(stripString(temporary));
					temporary.clear();
				} else {
				    temporary.push_back(operand[i]);
				}
			}
			if(temporary.size() > 0){
				definitions.push_back(stripString(temporary));
				temporary.clear();
			}
			//Write Refer records
			fout<<'R';
			for(int i=0;i<definitions.size();i++){
				temporary=definitions[i];
				extref.insert(definitions[i]);
				temporary = make_len(temporary, 6, 1, ' ');
				fout<<temporary;
			}
			string empty_string = "";
			output_line(empty_string);
			continue;
		} else if(opcode=="BASE"){
		    //Assists the assembler in base relative displacement computation
			int expressionValue=evaluateExpression(operand,true,0,0);
			//if evaluateExpression() encounters an error
			if(expressionValue==-1){
                //if it is SYNTAX ERROR
				if(SYNTAX_ERROR){
                    //display error message
					error_handling(1, line);
				}
                //if it is UNDEFINED SYMBOL ERROR
				if(UNDEFINED_SYMBOL){
				    //display error message
                    error_handling(2, line);
				}
			}
			BASE=expressionValue;
		} else if(OPTAB.count(opcode)){
			if(isEQUAL("COMPR",opcode) || isEQUAL("CLEAR",opcode) || isEQUAL("TIXR",opcode)){
                //instruction format 2
				string instruction=OPTAB[opcode];
				vector<string> registers;
				string temporary;
				for(int i=0;i<operand.length();i++){
					if(operand[i]==','){
						registers.push_back(temporary);
						temporary.clear();
					} else {
						temporary.push_back(operand[i]);
					}
				}
				if(temporary.size() > 0){
					registers.push_back(temporary);
				}
				if(isEQUAL("COMPR",opcode) && (registers.size()!=2 || SYMTAB.count(registers[0])==0 \
					|| SYMTAB.count(registers[1])==0 || SYMTAB[registers[0]].count("PREDEFINED")==0 \
					|| SYMTAB[registers[1]].count("PREDEFINED")==0)){
					//COMPR has two operands and both must be predefined registers
					//display SYNTAX ERROR
					error_handling(1, line);
				} else if(!isEQUAL("COMPR",opcode) && (registers.size()!=1 || SYMTAB.count(registers[0])==0 \
					|| SYMTAB[registers[0]].count("PREDEFINED")==0)){
					//For the other two, one register operand
					//display SYNTAX ERROR
					error_handling(1, line);
				}
				if(isEQUAL("COMPR",opcode)){
				    //Add the register identification values to the instruction
					instruction+=SYMTAB[registers[0]]["PREDEFINED"] + SYMTAB[registers[1]]["PREDEFINED"];
				} else {
				    //Add the register identification values to the instruction
					instruction+=SYMTAB[registers[0]]["PREDEFINED"] + "0";
				}
				if(hex_to_dec(addr)-cur_start_addr>28){
				    //Write text record
					string cur_start_addr_string = dec_to_hex(cur_start_addr);
					cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
					string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
					record_len = make_len(record_len, 2, 0, '0');
					string out_line = "T" + cur_start_addr_string + record_len + textRecord;
					cur_start_addr=hex_to_dec(addr);
					textRecord=instruction;
				} else {
					textRecord.append(instruction);
				}
                //because it is a 2 byte instruction
				LOCCTR+=2;
			} else {
				if(extendedFlag){
					string instruction=OPTAB[opcode];
					//To help with flags
					int integer_instruction=hex_to_dec(instruction)*(1<<24);
					//Extended
					integer_instruction+=(1<<20);
					if(operand[0]=='#'){
                        //Immediate i=1, n=0
						reverse(operand.begin(), operand.end());
						operand.pop_back();
						reverse(operand.begin(), operand.end());
						integer_instruction+=(1<<24);
					} else if(operand[0]=='@'){
					    //Indirect i=0, n=1
						reverse(operand.begin(), operand.end());
						operand.pop_back();
						reverse(operand.begin(), operand.end());
						integer_instruction+=(1<<25);
					} else {
					    //Otherwise i=0 and n=0
						integer_instruction+=(1<<25)+(1<<24);
					}
					string tmp = "";
					for(int i=0;i<operand.length();i++){
						if(operand[i]==','){
						    //Indexed e=1
							integer_instruction+=(1<<23);
							operand = tmp;
							break;
						}
                        else{
                            tmp.push_back(operand[i]);
                        }
					}
					if(SYMTAB.count(operand)){
						if(SYMTAB[operand].count(progName)){
							integer_instruction+=hex_to_dec(SYMTAB[operand][progName]);
						}
                        //if operand is not in EXTREF
                        else if(extref.count(operand)==0){
							//display UNDEFINED SYMBOL ERROR
							error_handling(2, line);
						}
						//if operand is in EXTREF
						else if(extref.count(operand)){
                            //Make modification record in new format since symbol is in EXTREF
							string modifRecord="M";
							string modifStartAddress=dec_to_hex(LOCCTR+1);
							modifStartAddress = make_len(modifStartAddress, 6, 0, '0');
							modifRecord+=modifStartAddress+"05+"+operand;
							modif_records.push_back(modifRecord);
						}
					}
					//convert instruction to hexadecimal string
					instruction=dec_to_hex(integer_instruction);
					instruction = make_len(instruction, 8, 0, '0');
					if(hex_to_dec(addr)-cur_start_addr>26){
                        //write previous text record and make a new text record
						string cur_start_addr_string = dec_to_hex(cur_start_addr);
						cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
						string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
						record_len = make_len(record_len, 2, 0, '0');
						string line_out = "T" + cur_start_addr_string + record_len + textRecord;
						output_line(line_out);
						textRecord=instruction;
						cur_start_addr=hex_to_dec(addr);
					} else {
					    //append to previous record
						textRecord+=instruction;
					}
					//because it is 4 byte instruction
					LOCCTR+=4;
				} else {
				    //For program counter relative addressing, PC value is required
					int PC_value=hex_to_dec(addr)+stoi(len);
					string instruction = OPTAB[opcode];
					//to help with flags
					int integer_instruction=hex_to_dec(instruction)*(1<<16);
					if(operand.length()){
						if(operand[0]=='#'){
                            //Immediate i=1, n=0
							integer_instruction+=(1<<16);
							reverse(operand.begin(), operand.end());
							operand.pop_back();
							reverse(operand.begin(), operand.end());
						} else if(operand[0]=='@'){
						    //Indirect i=0, n=1
							integer_instruction+=(1<<17);
							reverse(operand.begin(), operand.end());
							operand.pop_back();
							reverse(operand.begin(), operand.end());
						} else {
						    //Otherwise i=1, n=1
							integer_instruction+=(1<<16)+(1<<17);
						}
						string tmp = "";
						for(int i=0;i<operand.length();i++){
						    //Indexed
							if(operand[i]==','){
								integer_instruction+=(1<<15);
								operand=tmp;
								break;
							}
							else tmp.push_back(operand[i]);
						}
					} else {
					    //otherwise i=1, n=1
						integer_instruction+=(1<<16)+(1<<17);
					}

					if(operand.empty()){
                        //for instructions like RSUB
						instruction=dec_to_hex(integer_instruction);
						instruction = make_len(instruction, 6, 0, '0');
					}
					//if operand is integer
					else if(is_number(operand)){
						integer_instruction+=stoi(operand);
						instruction=dec_to_hex(integer_instruction);
						//while(instruction.length()<6)instruction='0'+instruction;
						instruction = make_len(instruction, 6, 0, '0');
					}
					//if operand is a literal
					else if(LITTAB.count(operand)){
                        //Get the address of the literal
						int value=hex_to_dec(LITTAB[operand]);
						if((value>PC_value && value-PC_value<2047) || (PC_value>value && PC_value-value<2048)){
							//Check for PC relative
							int disp=value-PC_value;
							if(disp<0){
								//Two's complement (12 bit)
								disp=-disp;
								disp=((((~disp)&(0xFFF))+1)&(0xFFF));
							}
							integer_instruction+=disp;
                            //p=1
							integer_instruction+=(1<<13);
						} else if(value>BASE && value-BASE<4096){
						    //If PC didn't work, try base
							int disp=value-BASE;
							integer_instruction+=disp;
							//b=1
							integer_instruction+=(1<<14);
						} else {
							//if none worked, display DISPLACEMENT OUT OF BOUNDS ERROR
							error_handling(7, line);
						}
						instruction=dec_to_hex(integer_instruction);
						instruction = make_len(instruction, 6, 0, '0');
					}
					//if operand is found in SYMTAB
					else if(SYMTAB.count(operand) && SYMTAB[operand].count(progName)){
						//Attempt with PC
						int value=hex_to_dec(SYMTAB[operand][progName]);
						if((value>=PC_value && value-PC_value<2047) || (PC_value>value && PC_value-value<2048)){
							int disp=value-PC_value;
							if(disp<0){
								//Two's complement
								disp=-disp;
								disp=((((~disp)&(0xFFF))+1)&(0xFFF));
							}
							integer_instruction+=disp;
							//p=1
							integer_instruction+=(1<<13);
						} else if(value>BASE && value-BASE<4096){
						    //Try base register
							int disp=value-BASE;
							integer_instruction+=disp;
							//b=1
							integer_instruction+=(1<<14);
						} else {
							//if none worked, display DISPLACEMENT OUT OF BOUNDS ERROR
							error_handling(7, line);
						}
						instruction=dec_to_hex(integer_instruction);
						instruction = make_len(instruction, 6, 0, '0');
					} else {
						//In instruction format 3, EXTREF isn't possible
						//display UNDEFINED SYMBOL ERROR
						error_handling(2, line);
					}

					if(hex_to_dec(addr)-cur_start_addr>27){
                        //write previous record into object file and generate new record
						string cur_start_addr_string = dec_to_hex(cur_start_addr);
						cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
						string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
						record_len = make_len(record_len, 2, 0, '0');
						string line_out = "T" + cur_start_addr_string + record_len + textRecord;
						output_line(line_out);
						textRecord=instruction;
						cur_start_addr=hex_to_dec(addr);
					} else {
					    //append into previous record
						textRecord+=instruction;
					}

					LOCCTR+=3;
				}
			}
		} else {
			if(isEQUAL("RESW",opcode) || isEQUAL("RESB",opcode)){
                //compute the required space
				int total;
				if(isEQUAL("RESW",opcode))
                    total = stoi(operand)*3;
                else total = stoi(operand);
				if(textRecord.size() > 0){
				    //write previous record into object file and generate new record
					string cur_start_addr_string = dec_to_hex(cur_start_addr);
					cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
					string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
					record_len = make_len(record_len, 2, 0, '0');
					string line_out = "T" + cur_start_addr_string + record_len + textRecord;
					output_line(line_out);
					textRecord.clear();
				}
                //increase the value of LOCCTR by total (required space)
				LOCCTR+=total;
				cur_start_addr=LOCCTR;
			} else if(opcode=="WORD"){
				if(operand.empty()){
					operand="0";
				}
				int expressionValue=evaluateExpression(operand,false,LOCCTR,6);
				//if evaluateExpression() encounters error
				if(expressionValue==-1){
					if(SYNTAX_ERROR){
						//display SYNTAX ERROR
						error_handling(1, line);
					}
				}
				string instruction = dec_to_hex(expressionValue);
				instruction = make_len(instruction, 6, 0, '0');
				if(hex_to_dec(addr)-cur_start_addr>27){
                    //write record to object file
					string cur_start_addr_string = dec_to_hex(cur_start_addr);
					cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
					string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
					record_len = make_len(record_len, 2, 0, '0');
					string line_out = "T" + cur_start_addr_string + record_len + textRecord;
					output_line(line_out);
					textRecord=instruction;
					cur_start_addr=hex_to_dec(addr);
				} else {
					textRecord+=instruction;
				}
				LOCCTR+=instruction.length()/2;
			} else if(isEQUAL("BYTE",opcode) || isEQUAL("*",label)){
				//If it is a literal
				if(isEQUAL("*",label)){
					operand=opcode;
					reverse(operand.begin(), operand.end());
                    operand.pop_back();
                    reverse(operand.begin(), operand.end());
				}
				string instruction=operand;
				if(operand[0]=='X'){
					//If operand is a hexadecimal string, just write the hexadecimal string to file.
					//Remove X'' from string.
					instruction.pop_back();	//Remove last '
                    //Remove X'
					if(instruction.length()>=2)
                    {
                        reverse(instruction.begin(), instruction.end());
                        instruction.pop_back();
                        instruction.pop_back();
                        reverse(instruction.begin(), instruction.end());
                    }
					else instruction.clear();
				} else if(operand[0]=='C'){
					//If operand is a character string, break into hexadecimal parts
					//use getHexBreakup
					instruction.pop_back();
					if(instruction.length()>=2){
						reverse(instruction.begin(), instruction.end());
                        instruction.pop_back();
                        instruction.pop_back();
                        reverse(instruction.begin(), instruction.end());
						instruction=getHexBreakup(instruction);
					}
					else instruction.clear();
				}
				if(hex_to_dec(addr)+instruction.length()/2-cur_start_addr > 30){
                    //write text record to object file
					string cur_start_addr_string = dec_to_hex(cur_start_addr);
					cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
					string record_len=dec_to_hex(hex_to_dec(addr)-cur_start_addr);
					record_len = make_len(record_len, 2, 0, '0');
					string line_out = "T" + cur_start_addr_string + record_len + textRecord;
					output_line(line_out);
					textRecord=instruction;
					cur_start_addr=hex_to_dec(addr);
				} else {
					textRecord+=instruction;
				}
				//a hexadecimal character is equivalent to a half-byte
				LOCCTR+=instruction.length()/2;
			}
		}
	}

    //write final text record
	if(textRecord.size() > 0){
		string cur_start_addr_string = dec_to_hex(cur_start_addr);
		cur_start_addr_string = make_len(cur_start_addr_string, 6, 0, '0');
		string record_len=dec_to_hex(LOCCTR-cur_start_addr);
		record_len = make_len(record_len, 2, 0, '0');
		string line_out = "T" + cur_start_addr_string + record_len + textRecord;
        output_line(line_out);
		textRecord.clear();
	}
	//write final modification records
	for(int i =0;i<modif_records.size();i++){
		output_line(modif_records[i]);
	}
	modif_records.clear();

    //write END record
	if(!CSECT_flag){
		string startAddressString=to_string(start_address);
		startAddressString = make_len(startAddressString, 6, 0, '0');
		string line_out = "E" + startAddressString;
	    output_line(line_out);
	} else {
	    string line_out = "E";
	    output_line(line_out);
	}
	//Pass 2 ends
}