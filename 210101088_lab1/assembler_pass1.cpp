// pass 1, Ridhiman Dhindsa, 210101088
#include<bits/stdc++.h>
using namespace std;

map<string,string> symtab; //symbol table - label, address
map<string,string> optab; // opern code table- mnemonic, opcode(numeric)
int LOCCTR=0; //will store the int version of hex address

vector<string> tokenize(const string& input) {
    vector<string> tokens;
    istringstream iss(input);

    string token;
    while (getline(iss >> ws, token, ' ')) {
        tokens.push_back(token);
    }

    return tokens;
}

//returns the size of the constant in bytes
int find_size(string operand)
{
    char type = operand[0];
    string str = "";
    for(int i=2;i<operand.size()-1;i++)
    {
        str += operand[i];
    }

    int l=0;
    switch(type)
    {
        case 'C': //character string
        l = str.size();
        break;

        case 'X': //hexadecimal
        l = str.size()/2;
        break;
    }

    return l;
}

string decToHexa(int n)
{
    string ans = "";
   
    while (n != 0) 
    {
        int rem = 0; //remainder
        char ch;
        rem = n % 16;

        if (rem < 10) { ch = rem + 48; }
        else { ch = rem + 55; }

        ans += ch;
        n = n / 16;
    }
     
    // reversing the ans string to get the final result
    int i = 0, j = ans.size() - 1;
    while(i <= j)
    {
        swap(ans[i], ans[j]);
        i++; j--;
    }
    return ans;
}

int main()
{
    //--------------------------get the optab ready-----------------------------------------
    ifstream np("optab.txt");
    string mnemonic, code;
    while(np>>mnemonic>>code)
    {
        optab[mnemonic] = code;
    }
    np.close();

    // print operation code table
    cout<<"\nOPTAB:\n";
    for(auto u: optab)
    {
        cout<<u.first<<" "<<u.second<<endl;
    }
    cout<<endl;

    //--------------------------start assembler operation-----------------------------------

    ifstream fp("assembler_input.txt"); //input for pass 1
    ofstream op("intermediate.txt"); //output from pass 1, input to pass 2

    string line;
    vector<string> arr; // to store tokens
    string LABEL, OPCODE, OPERAND;
    string filename;
    int i=0;

    while(getline(fp, line)) 
    {
        if(line=="\n" || line=="") {break;}
        LABEL= "", OPCODE = "", OPERAND= "";

        //-------------------------Process each line here--------------------------------------
        cout<<"Line read:"<<line<<endl;

        // tokenize line by whitespace markers
        arr = tokenize(line);
        cout<<"Tokens: ";
        for(auto u: arr) {cout<<u<<" ";}
        cout<<endl;

        if(arr[0]==".") {continue;} //if line read is a comment

        i = arr.size();
        if(i>3) {
            //cout<<arr.size()<<endl;
            printf("Invalid input to assembler pass 1.\n");
            return 0;
        }
        else if(i==1)
        {
            OPCODE = arr[0];
        }
        else if(i==2)
        {
            OPCODE = arr[0];
            OPERAND = arr[1];
        }
        else if(i==3)
        {
            LABEL = arr[0];
            OPCODE = arr[1];
            OPERAND = arr[2];
        }

        cout<<"LABEL: "<<LABEL<<" OPCODE: "<<OPCODE<<" OPERAND: "<<OPERAND<<endl;

        //--------------------parse the label, opcode, operand--------------------------------
        if(OPCODE=="START")
        {
            filename = LABEL;
            symtab[LABEL] = OPERAND; //here label= name of program, operand= starting address
            LOCCTR = stoi(OPERAND,0,16); //locctr stores int value of hex address
            op<<OPERAND<<" "<<LABEL<<" "<<OPCODE<<" "<<OPERAND<<endl;
        }

        if(OPCODE!="END" && OPCODE!="START") // not end of input file
        {
            if(LABEL!="") // if label is not empty
            {
                if(symtab.find(LABEL)!=symtab.end())
                {
                    cout<<"Error: duplicate label\n";
                    return 0;
                }
                else
                {
                    symtab[LABEL] = decToHexa(LOCCTR);
                }
            }

            //-------------------------------update LOCCTR-----------------------------------
            int old = LOCCTR;
            //search optab for OPCODE
            if(optab.find(OPCODE)!=optab.end())
            {
                LOCCTR += 3; //convert to hex when printing
            }
            else if(OPCODE=="WORD") //assembler directive, don't store in optab
            {
                LOCCTR += 3; 
            }
            else if(OPCODE=="RESW") //assembler directive
            {
                LOCCTR += (3*stoi(OPERAND)); 
            }
            else if(OPCODE=="RESB") //assembler directive
            {
                LOCCTR += stoi(OPERAND);
            }
            else if(OPCODE=="BYTE") //assembler directive
            {
                int l = find_size(OPERAND); // find length of constant in bytes
                LOCCTR += l; // add length to LOCCTR 
            }
            else
            {
                cout<<"Error: Invalid operation code.\n";
                return 0;
            }

            //write line to intermediate file
            op<<decToHexa(old)<<" "<<LABEL<<" "<<OPCODE<<" "<<OPERAND<<endl;
        }
        
        if(OPCODE=="END")
        {
            int prog_size = LOCCTR-stoi(symtab[filename],0,16); // 207A-1000= 107A in the given example
            symtab["prog_size"] = decToHexa(prog_size);
            op<<line<<endl; //write last line to intermediate file
        }
    }

    op.close();
    fp.close();

    // print symbol table
    cout<<"\nSYMTAB:\n";
    for(auto u: symtab)
    {
        cout<<u.first<<" "<<u.second<<endl;
    }

    // save symbol table in file
    ofstream xp("symtab.txt");
    for(auto u: symtab)
    {
        xp<<u.first<<" "<<u.second<<endl;
    }
    xp.close();

    return 0;
}