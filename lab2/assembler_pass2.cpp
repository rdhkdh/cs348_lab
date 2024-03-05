// pass 2, Ridhiman Dhindsa, 210101088
#include<bits/stdc++.h>
using namespace std;
#define BASE "2000"

map<string,string> symtab; //symbol table - label, address
map<string,string> optab; // opern code table- mnemonic, opcode(numeric)

vector<string> tokenize(const string& input) 
{
    vector<string> tokens;
    istringstream iss(input);

    string token;
    while (getline(iss >> ws, token, ' ')) {
        tokens.push_back(token);
    }

    return tokens;
}

string decToHexa(int n)
{
    if(n==0) {return "0";}
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

string hexToBinary(string input)
{
  /*Using stoul() to get the decimal value of the given String*/
  unsigned int x =  stoul(input, nullptr, 16) ;

  /*Using bitset<16>(x).to_string() to get the binary String of given integer*/
  string result = bitset<16>(x).to_string(); 

  return result;
}

//convert the conventional 8bit opcode to 12bit extended opcode
string extended_format(string OPCODE, string OPERAND)
{
    string object_code;
    if(OPCODE[0]=='+') //extended instr
    {
        string op_val = optab[OPCODE.substr(1)]; //remove the initial + and find opcode value from symtab
        string bin = hexToBinary(op_val); //convert to binary
        bin = bin.substr(0, 6); //remove last 2 chars

        //determine flag bits
        string flag_bits="";
        if(OPERAND.back()=='X')
        {
            flag_bits = "111001"; //+ and X present
        }
        else{
            flag_bits = "110001"; //only + present
        }

        string temp = bin + flag_bits;
        
        //stoi converts temp from binary to decimal, then we convert decimal to hexa
        object_code = decToHexa(stoi(temp, nullptr, 2));
    }
    else // PC relative addressing
    {
        string op_val = optab[OPCODE];
        int d = stoi(op_val, nullptr, 16);
        if(OPERAND[0]=='#')
        {
            d += 1;
        }
        else if(OPERAND[0]=='@')
        {
            d += 2;
        }
        else //normal case
        {
            d += 3;
        }

        object_code = decToHexa(d);
    } 

    return object_code;
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

    //----------------------get the symtab ready------------------------------
    ifstream mp("symtab.txt");
    string label, address;
    while(mp>>label>>address)
    {
        symtab[label] = address;
    }
    mp.close();

    //----------------------start assembler pass 2------------------------------
    ifstream fp("intermediate.txt");
    ofstream op("assembler_output.txt");
    ofstream al("assembly_listing.txt");
    string line;
    vector<string> arr; //to store tokens
    string LOC, LABEL, OPCODE, OPERAND;
    int i=0, byte_length=0, extended=0;
    string object_code="", addr1="", addr2="";
    vector<string> obj_codes;
    string start_addr, old_loc="0", filename;

    while(getline(fp,line))
    {
        LOC="", LABEL="", OPCODE="", OPERAND="", object_code="", addr1="", addr2="";
        extended=0;

        //----------------------process each line here------------------------------
        cout<<"Line read:"<<line<<endl;
        if(line=="\n" || line=="") {break;}

        // tokenize line by whitespace markers
        arr = tokenize(line);
        cout<<"Tokens: ";
        for(auto u: arr) {cout<<u<<" ";}
        cout<<endl;

        if(arr[0]==".") {continue;} //if line read is a comment

        i = arr.size();
        if(i>4) {
            printf("Invalid input to assembler pass 2.\n");
            return 0;
        }
        else if(i==2)
        {
            LOC = arr[0];
            OPCODE = arr[1];
        }
        else if(i==3)
        {
            LOC = arr[0];
            OPCODE = arr[1];
            OPERAND = arr[2];
        }
        else if(i==4)
        {
            LOC = arr[0];
            LABEL = arr[1];
            OPCODE = arr[2];
            OPERAND = arr[3];
        }

        if(arr[0]=="END") 
        {
            LOC = "";
            LABEL = "";
            OPCODE = arr[0];
            OPERAND = arr[1];
        }

        cout<<"LOC: "<<LOC<<" LABEL: "<<LABEL<<" OPCODE: "<<OPCODE<<" OPERAND: "<<OPERAND<<endl;

        //if extended format
        if(OPCODE[0]=='+')
        {
            extended=1;
            OPCODE = OPCODE.substr(1); //remove 1st char
            
        }
    
        //------------------------parse the loc, label, opcode, operand--------------------------
        if(OPCODE=="START")
        {
            filename = LABEL;
            object_code="";
            al<<line<<" "<<object_code<<"\n";

            op<<"H^";
            op<<setw(6)<<setfill(' ')<<std::left<<filename<<"^";
            op<<setw(6)<<setfill('0')<<std::right<<LOC<<"^";
            op<<setw(6)<<setfill('0')<<std::right<<symtab["prog_size"]; 
            op<<"\n";

            old_loc = LOC;
            continue;
        }
        if(OPCODE!="END" && OPCODE!="START")
        {
            if(byte_length==0) {start_addr = LOC;}

            //-----------------------opcodes which are NOT assembler directives-------------------
            if(optab.find(OPCODE)!=optab.end())
            {
                if(extended==0) {addr1 = extended_format(OPCODE,OPERAND);}
                if(OPERAND!="")
                {
                    if(OPERAND[0]=='#')
                    {
                        OPERAND = OPERAND.substr(1); //remove 1st char
                        int remaining = 4 - OPERAND.length();
                        while(remaining>0)
                        {
                            addr2 = '0' + addr2;
                        }
                        addr2 = addr2 + OPERAND;
                    }
                    else
                    {
                        if(OPERAND.back()=='X')
                        {
                            OPERAND = OPERAND.substr(0, OPERAND.length()-2);
                        }
                        if(OPERAND[0]=='@')
                        {
                            OPERAND = OPERAND.substr(1);
                        }

                        //lookup operand
                        if(symtab.find(OPERAND)!=symtab.end())
                        {
                            int x = stoi(symtab[OPERAND], nullptr, 16) + stoi(BASE, nullptr, 16);
                            addr2 = decToHexa(x);
                        }
                        else
                        {
                            addr2 = "0";
                            cout<<"Error: undefined symbol.\n";
                            return 0;
                        }
                    }
                }
                else
                { //if no operand
                    addr2 = "0000";
                }
            }

            //assemble the object code instruction
            if(extended==0) {object_code = addr1 + addr2;}
            else if(extended==1)
            {
                int remaining = 5 - addr2.length();
                while(remaining>0)
                {
                    addr1 = '0' + addr1; //add the reqd no of 0s in the starting
                    remaining--;
                }
                object_code = addr1 + addr2;
            }


            //-------------------------dealing with assembler directives-------------------------
            if(OPCODE=="BYTE")
            {
                if(OPERAND=="C'EOF'")
                { // type C- character string
                    object_code= "454F46";
                }
                else
                { // type X - hexadecimal
                    string str = "";
                    for(int i=2;i<OPERAND.size()-1;i++)
                    {
                        str += OPERAND[i];
                    }
                    object_code = str;
                }
            }
            if(OPCODE=="WORD")
            {
                string str = decToHexa(stoi(OPERAND));
                if(str.length()<6) //object_code might be shorter than 6 places
                {
                    int n = 6-str.length();
                    for(int i=1;i<=n;i++)
                    {
                        object_code += '0';
                    }
                    object_code += str;
                }
            }
            if(OPCODE=="RESW" || OPCODE=="RESB")
            {
                object_code = "";
                old_loc = LOC;
                continue;
            }

            al<<line<<" "<<object_code<<"\n";
        }
        if(OPCODE=="END")
        {
            object_code = "";
            al<<line<<" "<<object_code<<"\n";

            //print the last stored text record
            op<<"T^";
            op<<setw(6)<<setfill('0')<<start_addr<<"^";
            op<<setw(2)<<setfill('0')<<decToHexa(byte_length)<<"^";
            for(auto u: obj_codes) {op<<u<<"^";}
            op<<"\n";    

            //print the ending record
            op<<"E^";
            op<<setw(6)<<setfill('0')<<symtab[filename];
            op<<"\n";
            break;
        }
        
        //-------------------------------starting new text records------------------------------------
        if(stoi(LOC,0,16)> stoi(old_loc,0,16)+3)
        { //if there's a big jump in address
            op<<"T^";
            op<<setw(6)<<setfill('0')<<start_addr<<"^";
            op<<setw(2)<<setfill('0')<<decToHexa(byte_length)<<"^";
            for(auto u: obj_codes) {op<<u<<"^";}
            op<<"\n";

            obj_codes.clear();
            byte_length = 0; 
            start_addr = LOC;
            obj_codes.push_back(object_code);
            byte_length += (object_code.length()/2);
            old_loc = LOC;

            continue;
        }

        obj_codes.push_back(object_code);
        byte_length += (object_code.length()/2);

        if(obj_codes.size()==10)
        {
            op<<"T^";
            op<<setw(6)<<setfill('0')<<start_addr<<"^";
            op<<setw(2)<<setfill('0')<<decToHexa(byte_length)<<"^";
            for(auto u: obj_codes) {op<<u<<"^";}
            op<<"\n";

            obj_codes.clear();
            byte_length = 0;
        }

        old_loc = LOC;
    }

    al.close();
    op.close();
    fp.close();

    return 0;
}