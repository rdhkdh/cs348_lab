/*

Run the commands in the folder contaniing the code and the input file

TO COMPILE:
 g++ 180101038_assign2_linker_loader.cpp

TO RUN:
 ./a.out

Following is the 2 pass linker loader

pass 1 gives the estab.txt as output
pass2 gives the memory_map.txt as output as shown in the book

*/



#include <bits/stdc++.h>
using namespace std;

string inputfile;  					//stores the name of the input file containing the object code of various sections
int cs_addr,prog_addr,exec_addr;    // control_section_address , program_address and execution address respectively;



ifstream infile;

//file pointers to create outputs
ofstream estab_file; //output of pass1 
ofstream memory_map; //output of pass2


//struct to store the External Symbols and Control Sections in a table
struct es
{
	int is_cs;
	int address;
	int length;

};
map<string,es> estab;   			//stores the external symbol table
map<int,string> objectcode; 		//stores the memory content byte wise : address mapped to objectcode

long int neg = (long int)0xFFFFFFFF000000; //used as a borrow for doing negative calculatioins with hexadecimal numbers

//converts integer to a minimum 4 digit hexadecimal string
string to_hex_string4(int i)
{
    stringstream ss;
    ss<<hex<<i;
    string res=ss.str();
    if(res.length()<4)res.insert(0,4-res.length(),'0');
    transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

//converts integer to a minimum 6 digit hexadecimal string
string to_hex_string6(int i)
{
    stringstream ss;
    ss<<hex<<i;
    string res=ss.str();
    if(res.length()<6)res.insert(0,6-res.length(),'0');
    transform(res.begin(), res.end(), res.begin(), ::toupper);
    return res;
}

//convert hex string to decimal int
int hex_to_int(string hexa)
{
	int x;
	std::stringstream ss;
	transform(hexa.begin(), hexa.end(), hexa.begin(), ::tolower);
	ss << std::hex << hexa;
	ss >> x;
	return x;

}


//converts a given line string to a vector of words seperated as per spaces
//extra spaces are skipped
vector<string> breaker(string line)
{
	vector<string> res;
	string temp="";

	for(char ch:line)
	{
		if(ch!=' ')
			temp+=ch;
		else if(temp!="")
		{
			res.push_back(temp);
			temp="";
		}
	}
	if(temp!="")res.push_back(temp);
	return res;

}


//pass 1 of the assembler to handle H and D records
void pass1()
{
	infile.open(inputfile);


	//this file will contain the external symbol table as contructed after pass1;
	estab_file.open("estab.txt");
	estab_file<<"C-section\tSymbol Name  Address \t Length\n";

	
	cs_addr=prog_addr; //assign starting address for relocation

	string curr_cs=""; //stores the name of the current control section
	int cs_length=0;  

	string line="";


	while(getline(infile,line,'\n'))
	{
		vector<string> words=breaker(line);	
		if(words[0][0]=='H') //header record indicate start of new control section
		{
			string cs_len="",cs_addr_obj=""; //reading the address and length from the next word
			for(int i=0;i<6;i++)
				cs_addr_obj+=words[1][i];
			for(int i=0;i<6;i++)
				cs_len+=words[1][6+i];

			words[0].erase(words[0].begin()); //to remove H and now words[0] is the name of the control section

			if(estab.find(words[0])!=estab.end())
			{
				cout<<"ERROR: DUPLICATE SECTION FOUND\n";
			}
			else
			{
				
				curr_cs=words[0];

				//insert the control section in estab;
				es new_es;
				new_es.is_cs=1;
				new_es.address=hex_to_int(cs_addr_obj) + cs_addr;
				new_es.length=hex_to_int(cs_len);
				estab[words[0]]=new_es;

				//print it to the table
				estab_file<<words[0]<<" \t\t  \t\t\t "<< to_hex_string4(new_es.address) <<" \t\t "<< to_hex_string4(new_es.length)<<" \t\t \n";

			}
			while(getline(infile,line,'\n')) //reading the current section till E is found
			{
				vector<string> records=breaker(line);
				if(records[0][0]=='E') //end record
				{
					break;
				}
				if(records[0][0]=='D') //define records
				{
					int no_of_es=records.size()-1;

					records[0].erase(records[0].begin()); //remove D so records[0] has name of external symbol;
					string es_name=records[0];
					
					for(int i=0;i<no_of_es;i++) //for each symbol in the record do
					{
						string es_addr=""; //reading address from next word
						for(int j=0;j<6;j++)
							es_addr+=records[i+1][j];

						if(estab.find(es_name)!=estab.end())
						{
							cout<<"ERROR : DUPLICATE EXTERNAL SYMBOL FOUND\n";
						}
						else
						{
							//insert the new external symbol in the estab
							es new_es;
							new_es.is_cs=0;
							new_es.address=hex_to_int(es_addr) + cs_addr;
							new_es.length=0;
							estab[es_name]=new_es;

							//print it to the estab file 
							estab_file<<"    \t\t"<<es_name<<" \t\t "<< to_hex_string4(estab[es_name].address)<<" \t\t \n";

						}

						if(i<no_of_es-1)
						{
							//reading the next name
							es_name="";
							for(int j=6;j<records[i+1].length();j++)
							es_name+=records[i+1][j];
						
						}
						

					}

					
				}
				
			}
			cs_addr+=hex_to_int(cs_len); //for starting address for next section

		}


	}
	infile.close();
	estab_file.close();

}


//pass 2 of the loader to handle T and M records
void pass2()
{
	infile.open(inputfile);
	cs_addr=prog_addr;
	exec_addr=prog_addr;
	int cs_len;

	string curr_cs="";
	string line="";

	//reading the input file again
	while(getline(infile,line,'\n'))
	{
		vector<string> words=breaker(line);
		if(words[0][0]=='H') //header record indicate start of new control section
		{
			words[0].erase(words[0].begin());
			curr_cs=words[0];
			cs_len=estab[curr_cs].length;


			while(getline(infile,line,'\n'))
			{
				vector<string> records=breaker(line);
				if(records[0][0]=='E') //end of section
				{
					break;
				}

				if(records[0][0]=='T') //text record
				{
					string specified_addr="";
					for(int i=1;i<7;i++)
					{
						specified_addr+=records[0][i];
					}
					int location = cs_addr + hex_to_int(specified_addr);
					
					//put the record in the memory location calculated
					for(int i=9;i<records[0].length();i+=2)
					{
						objectcode[location]="";
						objectcode[location]+=records[0][i];
						objectcode[location]+=records[0][i+1];
						location++;

					}


				}
				if(records[0][0]=='M') //modification record
				{
					
					string symbol_name="";
					string sym_loc="";
					string sym_len="";

					for(int v=1;v<records[0].length();v++)
					{
						if(v<7)sym_loc+=records[0][v];
						else if(v<9)sym_len+=records[0][v];
						else if(v<10)sym_len+=records[0][v];
						else symbol_name+=records[0][v];
					}

					symbol_name.erase(remove(symbol_name.begin(), symbol_name.end(), ' '), symbol_name.end()); //remove spaces if any
					int g=hex_to_int(sym_loc) + cs_addr;

					//reading old value from memory
					string old_val="";
					old_val+=objectcode[g];
					old_val+=objectcode[g+1];
					old_val+=objectcode[g+2];

					
					int new_val=0;

					if(old_val[0]=='F') //handle negative values of the memory
					{
						new_val=neg;

					}

					//add or subtract as per + or - in modification records
					if(sym_len.back()=='+')
					{
						new_val += hex_to_int(old_val) + estab[symbol_name].address;
					}
					else
					{
						new_val +=  hex_to_int(old_val) - estab[symbol_name].address ;
					} 

					string new_hex=to_hex_string6(new_val);

					if(new_hex.length()>6 && new_hex[0]=='F' && new_hex[1]=='F')
					{
						new_hex.erase(new_hex.begin());
						new_hex.erase(new_hex.begin());
					}

					//store new value in the memory
					objectcode[g]="";
					objectcode[g]+=new_hex[0];
					objectcode[g]+=new_hex[1];

					objectcode[g+1]="";
					objectcode[g+1]+=new_hex[2];
					objectcode[g+1]+=new_hex[3];

					objectcode[g+2]="";
					objectcode[g+2]+=new_hex[4];
					objectcode[g+2]+=new_hex[5];



				}
			}
			if(line[0]=='E' && line.length()>1) //if address specified in the END record then make exec addr = given_addr + cs_addr
			{
				line.erase(line.begin());
				exec_addr = cs_addr + hex_to_int(line);
			}
			cs_addr +=cs_len; // go to next section


		}

		//jump to location exec addr for execution


	}
	infile.close();

}

//prints the contents of memory to the terminal
void print_obj()
{
	cout<<"Addr objectcode\n";
	for(auto it=objectcode.begin();it!=objectcode.end();it++)
		cout<<to_hex_string4(it->first)<<" "<<it->second<<"\n";
}

//prints the memory content in the file memory_map.txt
//16 bytes at a time for better readability
void print_memory()
{
	
	memory_map.open("memory_map.txt");
	memory_map<<"Mem_addr  _________________Contents______________________\n";

	int i=0;
	int last_addr = objectcode.rbegin()->first;
	int paddr=prog_addr - 16;


	for(; ;i++)
	{
		if(paddr > last_addr)
		{
			while((prog_addr-paddr)%16!=0)
			{
				if((i+1)%4==0)
				memory_map<<"xx\t";
				else 
				memory_map<<"xx";
				i++;
				paddr++;
			}
			break;
		}
		if(i%16==0)
		{
			memory_map<<to_hex_string4(paddr)<<" \t ";
		}

		if(paddr<prog_addr)
		{
			if((i+1)%4==0)
				memory_map<<"xx\t";
			else 
				memory_map<<"xx";
		}
		else if(objectcode.find(paddr)!=objectcode.end() && objectcode[paddr]!="")
		{
			if((i+1)%4==0)
				memory_map<<objectcode[paddr]<<" \t";
			else 
				memory_map<<objectcode[paddr];
		}
		else
		{
			if((i+1)%4==0)
				memory_map<<"..\t";
			else 
				memory_map<<"..";
		}

		if((i+1)%16==0)memory_map<<"\n";

		paddr++;

	}
	memory_map.close();


}


int main()
{
	estab.clear();
	inputfile="inp_ass2.txt";
	string pa="4000"; //starting address usually received from the os
	prog_addr=hex_to_int(pa);
	pass1();
	cout<<"pass1 done. estab.txt created\n";
	pass2();
	print_memory();
	cout<<"pass2 done. memory_map.txt created\n";
	print_obj();

}