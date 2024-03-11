
/*
A file named "Source Code.txt" is included with the program.
This file contains the Assembly code provided in the book.
The program assumes such a file (Source Code.txt) is present in the same directory as itself.
*/

#include <bits/stdc++.h>
using namespace std;

#define ZERO 0
#define TWO 2
#define FOUR 4
#define SIX 6
#define SEVEN 7
#define NINE 9
#define ARRAY_SIZE 1001
#define TRUE true
#define FALSE false

string padStr;
int pad_len;

string pad(string str, int len) {                                               // make string of appropriate length
    string temp;
    for(int i=0; i<len-str.size(); i++)
        temp.push_back('0');
    temp = temp + str;
    return temp;
}


string OPTAB(string a){
    if(a == "TIX") return "2C";
    if(a == "TIXR") return "B8";
    if(a == "TD") return "E0";
    if(a == "RD") return "D8";
    if(a == "WD") return "DC";
    if(a == "LDA") return "00";
    if(a == "LDX") return "04";
    if(a == "LDL") return "08";
    if(a == "LDB") return "68";
    if(a == "STCH") return "54";
    if(a == "ADD") return "18";
    if(a == "SUB") return "1C";
    if(a == "MUL") return "20";
    if(a == "DIV") return "24";
    if(a == "LDT") return "74";
    if(a == "STA") return "0C";
    if(a == "STX") return "10";
    if(a == "STL") return "14";
    if(a == "LDCH") return "50";
    if(a == "COMP") return "28";
    if(a == "COMPR") return "A0";
    if(a == "CLEAR") return "B4";
    if(a == "J") return "3C";
    if(a == "JLT") return "38";
    if(a == "JEQ") return "30";
    if(a == "JGT") return "34";
    if(a == "JSUB") return "48";
    if(a == "RSUB") return "4C";

    return "!";
}

string REGTAB(string a){
    if(a == "A") return "0";
    if(a == "X") return "1";
    if(a == "L") return "2";
    if(a == "B") return "3";
    if(a == "S") return "4";
    if(a == "T") return "5";
    if(a == "F") return "6";
    if(a == "PC") return "8";
    if(a == "SW") return "9";
    return "";
}



class objCode {                                                     // Class storing complete information about Format 3 and Format 4 Object codes.
    public:
        string opcode;                                              // 8bit Opcode     // Indirect Flag         // Immediate addressing flag   // Indexed Addressing flag   // Base-indexing flag   // PC - indexing flag      
        bool n,i,x,b,p,e;                                           // Extended Format flag
        int target;

        objCode(string op) {
            opcode = OPTAB(op);
            n=1; i=1; x=ZERO; b=ZERO; p=1; e=ZERO;
        }


        string getObj() {
            int instruction;
            istringstream(opcode) >> hex >> instruction;
            bitset<8> bin(instruction);
            string obj = bin.to_string().substr(ZERO,SIX);               //Getting first 6-bits of Opcode
            
            if(n){
                obj += "1";
            }else{
                obj += "0";
            }

            if(i){
                obj += "1";
            }else{
                obj += "0";
            }

            if(x){
                obj += "1";
            }else{
                obj += "0";
            }

            if(b){
                obj += "1";
            }else{
                obj += "0";
            }

            if(p){
                obj += "1";
            }else{
                obj += "0";
            }

            if(e){
                obj += "1";
            }else{
                obj += "0";
            }

            ostringstream tar;
            tar << hex << target;                                   
            string temp;
            int len;
            if(e){
                len = 5;
            }

            else{
                len = 3;
            }

            if(target<ZERO) {                                                  //target can be negative. 
                temp=tar.str();
                temp=temp.substr(temp.size()-len, len);             //C++ handles 2'complement conversion by default.
            }

            else if(target >= ZERO) {                                       //target can be positive
                temp="";
                int i=0;
                while(i<len - tar.str().size())
                {
                    temp += "0";
                    i++;
                }
                temp+=tar.str();
            }

            bitset<12> binary(obj);
            ostringstream out;
            out<<hex<<binary.to_ulong();
            padStr = out.str();
            pad_len = 3;
            string str = ::pad(padStr, pad_len);
            str+=temp;                                              //Object Code ready
            return str;                                             
        }

        int getLen() {
            if(e){
                return FOUR;
            }            

            else{
                return 3;
            }                          //Length of the Instruction (Bytes)
        }


};



class codeLine {     
    public:                      //Width (Assuming single column for space in between them)
        string label, opcode, operands;              //10 Columns //10 Columns //30 Columns            
        char preOpcode, preOperand;

    codeLine(){

    };

    codeLine(string label, string opcode, string operands, char preOpcode, char preOperand){
        this->label = label;
        this->opcode = opcode;
        this->operands = operands;
        this->preOpcode = preOpcode;
        this->preOperand = preOperand;
    }

    void setLabel(string label){
        this->label = label;
    }

    void setOpcode(string opcode){
        this->opcode = opcode;
    }

    void setOperands(string operands){
        this->operands = operands;
    }

    void setPreOpcode(char preOpcode){
        this->preOpcode = preOpcode;
    }

    void setPreOperand(char preOperand){
        this->preOperand = preOperand;
    }

    // ---------

    string getLabel(){
        return this->label ;
    }

    string getOpcode(){
        return this->opcode ;
    }

    string getOperands(){
        return this->operands;
    }

    char getPreOpcode(){
        return this->preOpcode ;
    }

    char getPreOperand(){
        return this->preOperand ;
    }


    
};
int computeLength(const string&);
void read(const string&, codeLine&);
void passOne();
void prune(string&);
void capitalize(string&);

void makeHeader(ofstream&, codeLine&, int);
void makeDefs(ofstream&, codeLine&, int);
void endRec(ofstream&, string&, int);
void makeEndRec(ofstream&, int);
void writeRecord(ofstream&, objCode&, string&, string&, int&);
void writeRecordTwo(ofstream&, string&, int, string&, string&, int&);
int evalExpression(string&, int, string&, int);
void dumpLiterals(ofstream&, int, int&);
void makeRefs(ofstream&, codeLine&, int);
void makeModRecs(ofstream&, string&);
void passTwo();

bool blank(const string&);
void split(vector<string>&, const string&);

bool isEquals(string a, string b){
    if(a.size() != b.size()){
        return FALSE;
    }
    if(a==b){
        return TRUE;
    }
    return FALSE;
}


void split_array(string labels[], const string& ops,int &size) {                        // Split Comma separated strings into Vectors
    int start = ZERO;
    size = -1;
    int n = ops.size();
    for(int i=ZERO; i<n; i++) {
        if(ops[i]==',') {
            size++;
            labels[size] =ops.substr(start, i-start);
            start = i+1;
        } 
        if(i==n-1){
            size++;
            labels[size] = ops.substr(start, ops.size() - start);
        }  
    }
}

map <string, int> twoByte;
//Extension of OPTAB to specify which instructions are 2 byte

void init_1(){
    twoByte["COMPR"]++;
    twoByte["CLEAR"]++; 
    twoByte["TIXR"]++; 
}


vector<map<string,int>> SYMTAB(1);                     //Symbol Table to map Labels to Addresses
vector<map<string, int>> LITTAB(1);                    //Literal Table
vector<set<string>> extrefs(1);                        //Store External References of the section
vector<vector<int>> relLocs(1);
int startAddr;
vector<int> programLength(ZERO);                          //Program Lengths of Individual Sections
bool Error;


void passOne() {                                                                    //Begin first pass
    ifstream fin;
    ofstream fout;
    string currLine;
    codeLine currCode;
    int LOCCTR, jump;
    int controlSect;

    fout.open("Intermediate.txt");
    fin.open("Source Code.txt");
    if(!fin) {
        Error=TRUE;
        cout<<"Error: Source Code not found"<<endl;
        return;
    }
    getline(fin, currLine);                                                         //get first line
    read(currLine, currCode); 

    if(isEquals(currCode.getOpcode(), "START")) {                                                //Initialize everything
        controlSect = ZERO;                                    
        istringstream(currCode.getOperands()) >> hex >> startAddr;
        ostringstream loc;
        
        LOCCTR = startAddr;
        loc << hex << LOCCTR;
        relLocs[controlSect].push_back(LOCCTR); 
        padStr = loc.str();
        pad_len = FOUR;
        fout<<pad(padStr,pad_len)<<" "<<currLine<<endl;

        getline(fin, currLine);
        read(currLine, currCode);
    }
    else LOCCTR = ZERO;                                                                // Address 0 if no other address defined

    do
    {
        jump=ZERO;
        bool write=TRUE;
        bool comment = FALSE;
        string Opcode = currCode.getOpcode();
        if(currCode.getLabel() != ".") {
            if(currCode.getLabel() != "") {
                if(SYMTAB[controlSect].count(currCode.getLabel())) {
                    Error = TRUE;
                    cout<<"Error: duplicate label"<<endl;
                    return;
                } 
                else if(Opcode!="CSECT") 
                    SYMTAB[controlSect][currCode.getLabel()] = LOCCTR;                           //Store in Symbol Table
            }

            if(currCode.preOperand== '=')                                                  //Check for Literals
                if(!LITTAB[controlSect].count(currCode.getOperands()))                           
                    LITTAB[controlSect].insert({currCode.getOperands(), -1});                    //Insert in LITTAB with unassigned address

                                                
            if(OPTAB(Opcode) != "!") {
                int instructionLength = (currCode.getPreOpcode()=='+'?FOUR:3);                      //Check Extended format

                if(twoByte.count(Opcode)) instructionLength = TWO;
                jump = instructionLength;
            }

            else if(isEquals(Opcode, "RESW")) {                                         //RESW and RESB have Operands in Decimal
                int dec_operand;
                istringstream(currCode.getOperands()) >> dec >> dec_operand;
                jump = 3*dec_operand;
            }

            else if(isEquals(Opcode, "LTORG")) {
                fout<<"     "<<currLine<<endl;
                dumpLiterals(fout, controlSect, LOCCTR);                        //Assign addresses to literals and print to Intermediete
                write = FALSE;
            }

            else if(isEquals(Opcode, "WORD")) jump = 3;                                 //Word Operand always has length 3
            
            else if(isEquals(Opcode, "RESB")) {
                int dec_operand;
                istringstream(currCode.operands) >> dec >> dec_operand;
                jump = dec_operand;
            }
            else if(isEquals(Opcode, "BYTE")) {
                jump = computeLength(currCode.operands);                        //Special Subroutine to Calculate length for BYTE
            }
            
            else if(isEquals(Opcode, "EXTREF") || isEquals(Opcode, "EXTDEF")) {
                fout<<"     "<<currLine<<endl;
                write = FALSE;
            }
            
            else if(isEquals(Opcode, "CSECT")) {                                                    //Start of new section
                dumpLiterals(fout, controlSect, LOCCTR);                                    //Dump literals of previous section
                programLength.push_back(LOCCTR - startAddr);

                startAddr = ZERO;                                                              //Reset Start Address
                LOCCTR = ZERO;                                                                 //Reset Location Counter
                jump = ZERO;                                                                   
                controlSect++;                                                              //Increment Control Section Number
                map<string, int> newSYM, newLIT;                                            
                vector<int> newLocs;
                relLocs.push_back(newLocs);                                                 //New Location Array
                SYMTAB.push_back(newSYM);                                                   //New Symbol Table
                LITTAB.push_back(newLIT);                                                   //New Literal Table
                write = TRUE;   
            }
            else if(isEquals(Opcode, "EQU")) {
                jump=ZERO;
                string operand = currCode.operands;
                string val;
                if(isEquals(operand, "*")) write = TRUE;                                // * means current location pointer
                else if(SYMTAB[controlSect].count(operand)) {                   // If Operand is already a Symbol
                    ostringstream loc;
                    loc << hex << SYMTAB[controlSect][operand];
                    int LOCCTRR = SYMTAB[controlSect][operand];
                    SYMTAB[controlSect][currCode.label] = LOCCTR;
                    if(relLocs[controlSect].back()!=LOCCTRR) 
                        relLocs[controlSect].push_back(LOCCTRR);
                    padStr = loc.str();
                    pad_len = FOUR;
                    fout<<pad(padStr, pad_len);
                    fout<<" ";
                    fout<<currLine;
                    fout<<endl;

                    write = FALSE;
                }
                else  {                                                                     //If Operand is an expression
                    string temp = "-1";                                                      
                    int val = evalExpression(currCode.operands, controlSect, temp, ZERO);      //Computes the value of expression
                    SYMTAB[controlSect][currCode.label] = val;
                    ostringstream out;
                    out << hex << val;
                    padStr = out.str();
                    pad_len = FOUR;
                    fout<<pad(padStr, pad_len);
                    fout<<" ";
                    fout<<currLine<<endl;                           
                    write = FALSE;
                }
            }

        }
        else comment = TRUE;

        if(write) {
            ostringstream loc;

            loc << hex << LOCCTR; 
            if(relLocs[controlSect].empty() || relLocs[controlSect].back()!=LOCCTR) 
                relLocs[controlSect].push_back(LOCCTR);
            fout<<( !comment ? pad(loc.str(), FOUR) : "    " )<<" "<<currLine<<endl;         //Write line along with address to Intermediate file
            if(currLine[ZERO]!='.') LOCCTR+=jump;
        }
        getline(fin, currLine);                                                                  //read next line
        read(currLine, currCode);
    }while(currCode.opcode != "END"); 

    ostringstream loc;
    loc << hex << LOCCTR;
    if(relLocs[controlSect].back()!=LOCCTR) 
        relLocs[controlSect].push_back(LOCCTR);
    padStr = loc.str();
    pad_len = FOUR;
    fout<<pad(padStr, pad_len);
    fout<<" ";
    fout<<currLine;
    fout<<" "<<endl;                                              //write last line

    dumpLiterals(fout, controlSect, LOCCTR);                                                        //dump literals of last section

    programLength.push_back(LOCCTR - startAddr);

    fin.close();                                                                                    //close Source Code.txt 
    fout.close();                                                                                   //close Intermediete.txt
}

string currRecord;
string modRecord;
string temp, object;
string LOC;


void passTwo() {                                                                //Beginning of Pass Two
    ifstream fin;
    ofstream fout;
    string currLine;
    codeLine currCode;
    
    bool res=FALSE;   
    int locIdx;       
                                                                                      //bool to specify if previous command was RESB or RESW

    int controlSect;
    fout.open("Object Program.txt");                                            //File to store final object program
    fin.open("Intermediate.txt");                                               //Open Intermediate produced by pass one.
    if(!fin) {
        Error=TRUE;
        cout<<"Error: Intermediate file not found"<<endl;
        return;
    }                                            
    getline(fin, currLine);   
    LOC = currLine.substr(ZERO,FOUR); 
    read(currLine.substr(5,currLine.size()-5), currCode); 

    if(isEquals(currCode.getOpcode(), "START")) {
        controlSect = ZERO;
        istringstream(currCode.getOperands()) >> hex >> startAddr;
        makeHeader(fout, currCode, controlSect);
    } 
    getline(fin, currLine);
    string tempLOC=currLine.substr(ZERO,FOUR); if(!blank(tempLOC)) LOC=tempLOC;
    read(currLine.substr(5,currLine.size()-5), currCode);
    int currLoc, nextLoc, incr;
    istringstream(LOC) >> hex >> currLoc;                                                              //Write Header to Output
    locIdx = ZERO;

    padStr = LOC;
    pad_len = SIX;
    currRecord = "T"+pad(padStr,pad_len)+"00";                                                  //Initiate Record with Start address and Zero length.
    int recLength = ZERO;                                                                          
    while(1) {
        if(currCode.getLabel() != ".") {
            string Opcode = currCode.getOpcode();
            string Operand = currCode.getOperands();
            string objectCode;
            if(isEquals(Opcode, "EXTDEF")) makeDefs(fout, currCode, controlSect);
            else if(isEquals(Opcode, "EXTREF")) makeRefs(fout, currCode, controlSect);
            else if(OPTAB(Opcode) != "!") {                                           
                object=OPTAB(Opcode);
                temp="";
                if(Operand != "") {
                    if(SYMTAB[controlSect].find(Operand) != SYMTAB[controlSect].end()) {                                // Normal Instructions where Operand is a symbol
                        objCode obj(Opcode);                                                // and Opcode is in OPTAB
                        if(currCode.getPreOpcode()=='+') {                                       // check extended format
                            obj.e = TRUE;                                                   // set extension flag
                            obj.p = FALSE;                                                  // b=0 and p=0 always for format 4
                        }
                        if(currCode.getPreOperand()=='@') obj.i=FALSE;                           // check indirect addressing
                        obj.target = SYMTAB[controlSect][Operand] - nextLoc;                // calculate displacement

                        writeRecord(fout, obj, currRecord, LOC, recLength);                 // subroutine to write to record
                        locIdx++;
                    }
                    else if(extrefs[controlSect].count(Operand)) {                          // If operand is an external reference
                        objCode obj(Opcode);
                        if(currCode.getPreOpcode()=='+') {
                            obj.e = TRUE;
                            obj.p = FALSE;
                        }
                        obj.target = ZERO;
                        ostringstream out;
                        out<<hex<<(currLoc+1);
                        padStr = out.str();
                        pad_len = SIX ;
                        modRecord+="M"+pad(out.str(), SIX);

                        if(obj.e)
                        {
                            modRecord+="05";
                        }
                        else
                        {
                            modRecord+="03";
                        }
                        modRecord+="+"+Operand+"\n";
                        writeRecord(fout, obj, currRecord, LOC, recLength);
                        locIdx++;
                    }
                    else if(currCode.getPreOperand()=='#') {                                     // Check Immediete Operands
                        objCode obj(Opcode);
                        int arg;
                        istringstream(Operand) >> hex >> arg;
                        obj.p = FALSE;
                        obj.n = FALSE;
                        obj.target = arg;

                        writeRecord(fout, obj, currRecord, LOC, recLength);
                        locIdx++;
                    }
                    else if(currCode.preOperand=='=') {                                     // check if Operand is a Literal
                        objCode obj(Opcode);
                        obj.target = LITTAB[controlSect][Operand] - nextLoc;

                        writeRecord(fout, obj, currRecord, LOC, recLength);
                        locIdx++;
                    }
                    else if(twoByte.count(Opcode)) {                                        // 2 byte instruction
                        bool comma=ZERO;
                        for(auto x: Operand) 
                            if(x==',') comma=1;

                        objectCode = OPTAB(Opcode);
                        if(!comma) objectCode+= (REGTAB(Operand)+"0");
                        else {
                            string regs[ARRAY_SIZE];
                            int size=-1;
                            split_array(regs, Operand,size);
                            objectCode += (REGTAB(regs[ZERO]) + REGTAB(regs[1]));
                        }

                        incr=2;
                        writeRecordTwo(fout, objectCode, incr, currRecord, LOC, recLength);
                        locIdx++;
                    }
                    else if(isEquals(Opcode,"STCH") || isEquals(Opcode,"LDCH")) {                                      //Here Operand will be of form OPERAND,X
                        objCode obj(Opcode);
                        obj.x = TRUE;
                        if(currCode.getPreOpcode()=='+') {
                            obj.e = TRUE;
                            obj.p = FALSE;
                        }
                        string labels[ARRAY_SIZE];
                        int size = -1;
                        split_array(labels, Operand, size);
                        string label_0 = labels[ZERO];
                        if(SYMTAB[controlSect].find(label_0) != SYMTAB[controlSect].end())                                    // If Label from current section
                            obj.target = SYMTAB[controlSect][label_0] + 0x8000;
                        else if(extrefs[controlSect].find(label_0) != extrefs[controlSect].end()) {                            // If label is externally referenced.
                            obj.target=ZERO;
                            ostringstream out;
                            out<<hex<<(currLoc+1);
                            modRecord+="M"+pad(out.str(), SIX);
                            if(obj.e){
                                modRecord += "05+";
                            }else{
                                modRecord += "03+";
                            }
                            modRecord+=label_0+"\n";
                        }

                        writeRecord(fout, obj, currRecord, LOC, recLength);
                        locIdx++;
                    }
                    else {
                        cout<<"Error: undefined symbol"<<endl;
                        exit(ZERO);
                    }
                }
                else {                                                                              // For cases like "RSUB"
                    objCode obj(Opcode);
                    obj.target= obj.p= ZERO;
                    writeRecord(fout, obj, currRecord, LOC, recLength);
                    locIdx++;
                }
            }
            else if(currCode.getPreOpcode()=='=') {                                                      // Dumped Literal Definition                              
                Opcode += '\'';
                if(LITTAB[controlSect].find(Opcode)!=LITTAB[controlSect].end()) {

                    char first = Opcode[ZERO];
                    char second = Opcode[1];

                    if(first == 'C' || first == 'c' && second == '\'') {
                        temp="";
                        ostringstream out;
                        int i=ZERO;
                        i++;
                        i++;
                        while(Opcode[i]!='\'') {
                            out<<hex<<(int)Opcode[i];
                            i++;
                        }
                        objectCode=out.str();
                    }

                    if(first == 'X' || second == 'x' && second == '\'') 
                        objectCode=Opcode.substr(2,2);


                    first = Opcode[ZERO];
                    second = Opcode[1];

                    int len;
                    if( (first == 'C' || first == 'c') && second == '\'') {
                        for(len = 2; len<Opcode.size(); len++) {
                            if(Opcode[len] == '\'') {
                                len-=2;
                                break;
                            }
                        }
                    }

                    if( (first == 'X' || second == 'x') && second == '\'') 
                        len = 1;
                    
                    incr = len;


                    padStr = LOC;
                    pad_len = SIX;
                    if(isEquals(currRecord, "T")) currRecord+=pad(padStr,pad_len)+"00";
                    if(recLength+incr>30) {                                       
                        ostringstream out;  
                        out << hex << recLength;
                        padStr = out.str();
                        pad_len = TWO;
                        string temp=pad(padStr,pad_len);

                        for(int i=SEVEN;i<NINE;i++)
                        {
                            currRecord[i]=temp[i-SEVEN];
                        }

                        capitalize(currRecord);
                        fout<<currRecord<<endl;
                        padStr = LOC;
                        pad_len = SIX;
                        currRecord = "T"+pad(padStr,pad_len)+"00";
                        recLength = ZERO;

                        currRecord+=objectCode;
                        recLength+=incr;
                    }else{
                        currRecord+=objectCode;
                        recLength+=incr;
                    }
                    



                    locIdx++;
                }    
                else cout<<"Unknown Literal"<<endl;
            }
            else if(isEquals(Opcode, "WORD")) {                                                             // Opcode is WORD
                int val = evalExpression(Operand, controlSect, modRecord, currLoc);                 // Operand can be an expression
                ostringstream out;                                                                  // Can also add to Modification records
                out << hex << val;
                padStr = out.str();
                pad_len = SIX;
                objectCode = pad(padStr, SIX);


                incr=3;
                padStr = LOC;
                pad_len = SIX;
                if(isEquals(currRecord, "T")) currRecord+=pad(padStr,pad_len)+"00";
                if(recLength+incr>30) {                                       
                    ostringstream out;  
                    out << hex << recLength;
                    padStr = out.str();
                    pad_len = TWO;
                    string temp=pad(padStr,pad_len);

                    for(int i=SEVEN;i<NINE;i++)
                    {
                        currRecord[i]=temp[i-SEVEN];
                    }

                    capitalize(currRecord);
                    fout<<currRecord<<endl;
                    padStr = LOC;
                    pad_len = SIX;
                    currRecord = "T"+pad(padStr,pad_len)+"00";
                    recLength = ZERO;

                    currRecord+=objectCode;
                    recLength+=incr;
                }else{
                    currRecord+=objectCode;
                    recLength+=incr;
                }
                
                locIdx++;
            }
            else if(isEquals(Opcode, "BYTE")) {                                                             // BYTE Operands have a different format
                
                char first = Operand[ZERO];
                char second = Operand[1];

                
                if(first == 'C' || first == 'c' && second == '\'') {
                    temp="";
                    ostringstream out;
                    for(int i=TWO; Operand[i]!='\''; i++) out<<hex<<(int)Operand[i];
                    objectCode=out.str();
                }

                if(first == 'X' || second+"" == 'x'+"" && second == '\'') 
                    objectCode=Operand.substr(2,2);

                incr=computeLength(Operand);
                writeRecordTwo(fout, objectCode, incr, currRecord, LOC, recLength);
                locIdx++;
            }
            else if(isEquals(Opcode, "RESW") || isEquals(Opcode, "RESB")) {                                     // These iinstructions end current record line    
                endRec(fout, currRecord, recLength);                                            
                recLength=ZERO;    
                currRecord="T";                                                                 // Start new record
                locIdx++;                                                           
            }
            else if(isEquals(Opcode, "CSECT")) {                                                        // Beginning of new section
                endRec(fout, currRecord, recLength);                                            // End old record
                makeModRecs(fout, modRecord);                                                   // Print Modification Records
                makeEndRec(fout, controlSect);                                                  // Print End record 
                
                set<string> newSet;
                extrefs.push_back(newSet);                                                      // New External references collection
                controlSect++;                                                                  // Go to next section
                startAddr = ZERO;                                                                  // Reset start address
                modRecord.clear();                                                              // Clear Modification Records
                locIdx=ZERO;                                                                       // Reset Location index
                currRecord="T";                                                                 // New Record
                recLength=ZERO;                                                                    // Reset record length
                makeHeader(fout, currCode, controlSect);                                        // Make header for the new section
            }
        }

        getline(fin, currLine);                                                                             // Get next line
        if(currLine.empty()) break;                                                                         // Break out of loop at EOF
        string tempLOC=currLine.substr(ZERO,FOUR); if(!blank(tempLOC)) LOC=tempLOC;                               // read Location
        istringstream(LOC)>>hex>>currLoc;                                                                       
        while(locIdx < relLocs[controlSect].size() && currLoc!=relLocs[controlSect][locIdx]) locIdx++;      // Bring location index to position 
        if(locIdx+1 < relLocs[controlSect].size()) nextLoc = relLocs[controlSect][locIdx+1];                // Get next location
        read(currLine.substr(5, currLine.size()-5), currCode);                                              // Read line, except the Address
    }
    endRec(fout, currRecord, recLength);
    makeModRecs(fout, modRecord);
    makeEndRec(fout, controlSect);


    fin.close();
    fout.close();
}



void read(const string &line, codeLine &code) {                             // Function to read current line
    if(line[ZERO]=='.' || line.size()==1 || line.size()==ZERO) {
        code.label=".";
        code.opcode="";
        code.operands="";
        return;
    }

    string temp;
    temp = line.substr(ZERO, NINE);
    prune(temp);
    code.label = temp;

    code.preOpcode = line[NINE];
    temp = line.substr(10, min(NINE, (int)line.size()-11));
    prune(temp);
    code.opcode = temp;

    if(line.size()>20) {
        code.preOperand = line[19];
        temp = line.substr(20, (int)line.size()-21);
        prune(temp);
        code.operands = temp; 
    }
    else code.operands="";
}

void prune(string &str) {                                                   // Prune entries from the file for blank spaces
    int pause=str.size();
    int i=0;
    do
    {
        if(str[i]==' ') {
            pause = i;
            break;
        }
        i++;
    }while(i<str.size());

    str = str.substr(ZERO,pause);
}

int computeLength(const string &operand) {                                   // Length of Operands like C'EOF'
    int len;
    if( (operand[ZERO] == 'C' || operand[ZERO] == 'c') && operand[1] == '\'') 
    {
        len=2;
        do
        {
            if(operand[len] == '\'') {
                len-=2;
                break;
            }
            len++;
        }while(len<operand.size());

    }

    if( (operand[ZERO] == 'X' || operand[1] == 'x') && operand[1] == '\'') 
        len = 1;
    
    return len;
}

void capitalize(string &str) {                                                 // C++ stores hexadecimals in lower case
    for(auto &x: str){
        if(x>='a' && x<='z')
        {
            if(x == 'a') x = 'A';
            if(x == 'b') x = 'B';
            if(x == 'c') x = 'C';
            if(x == 'd') x = 'D';
            if(x == 'e') x = 'E';
            if(x == 'f') x = 'F';
            if(x == 'g') x = 'G';
            if(x == 'h') x = 'H';
            if(x == 'i') x = 'I';
            if(x == 'j') x = 'J';
            if(x == 'k') x = 'K';
            if(x == 'l') x = 'L';
            if(x == 'm') x = 'M';
            if(x == 'n') x = 'N';
            if(x == 'o') x = 'O';
            if(x == 'p') x = 'P';
            if(x == 'q') x = 'Q';
            if(x == 'r') x = 'R';
            if(x == 's') x = 'S';
            if(x == 't') x = 'T';
            if(x == 'u') x = 'U';
            if(x == 'v') x = 'V';
            if(x == 'w') x = 'W';
            if(x == 'x') x = 'X';
            if(x == 'y') x = 'Y';
            if(x == 'z') x = 'Z';
        }
    }
}



void makeHeader(ofstream &fout, codeLine& currCode, int controlSect) {          // Make Header record
    ostringstream out;
    string currRecord = "H"+currCode.getLabel();                                        
    do{currRecord+=" "; }while(currRecord.size()<SEVEN);                            

    out << hex << startAddr;
    string temp=out.str();
    for(int i=ZERO; i<SIX-temp.size(); i++) currRecord+="0";
    currRecord+=temp;

    out.str(""); out.clear();
    out << hex << programLength[controlSect];
    temp=out.str();

    int i=ZERO;
    do{currRecord+="0";i++;}while(i<SIX-temp.size()); 
    currRecord+=temp;

    capitalize(currRecord);
    fout<<currRecord<<endl;
}

void makeDefs(ofstream &fout, codeLine& currCode, int controlSect) {            // Make Definition Record
    string labels[ARRAY_SIZE];
    int size = -1;
    split_array(labels, currCode.getOperands(),size);
    string currRecord = "D";

    int i=ZERO;
    while(i<=size)
    {
        string x = labels[i];
        ostringstream out;
        out << hex << SYMTAB[controlSect][x];
        string temp(SIX-out.str().size(), '0');
        temp += out.str();        
        currRecord = currRecord + x + temp;
        i++;
    }

    capitalize(currRecord);
    fout<<(currRecord)<<endl;
}

void writeOrModifyRecord (ofstream& fout, objCode& obj, string& currRecord, string& LOC, int& recLength) {                                  // Write Record Function-1
    if(isEquals(currRecord, "T")) currRecord+=pad(LOC,SIX)+"00";
    if(recLength+obj.getLen()>30) {                                       
        ostringstream out;  
        out << hex << recLength;
        string temp=pad(out.str(),2);

        for(int i=SEVEN;i<NINE;i++)
        {
            currRecord[i]=temp[i-SEVEN];
        }

        capitalize(currRecord);
        fout<<currRecord<<endl;

        currRecord = "T"+pad(LOC,SIX)+"00";
        recLength = ZERO;
    }
    currRecord+=obj.getObj();
    recLength+=obj.getLen();

}

void makeRefs(ofstream &fout, codeLine& currCode, int controlSect) {            // Make reference Record
    string labels[ARRAY_SIZE];
    int size = -1;
    split_array(labels, currCode.getOperands(),size);
    string currRecord = "R";
    for(int i=ZERO;i<=size;i++) {
        string x = labels[i];
        extrefs[controlSect].insert(x);
        do{x+=" ";}while(x.size()<SIX);
        currRecord += x;
    }
    fout<<currRecord<<endl;
}

void makeModRecs(ofstream& fout, string& out) {                                 // Print Modification Records
    capitalize(out);
    fout<<out;
}

void makeEndRec(ofstream &fout, int controlSect) {                              // Print End Record
    string currRecord = "E";
    if(controlSect == ZERO) {
        ostringstream out;
        out << hex << startAddr;
        string temp(SIX-out.str().size(), '0');
        temp+=out.str();
        currRecord+=temp;
    }
    fout<<currRecord<<endl<<endl;
}



void dumphandle(ofstream& fout, int controlSect, int& LOCCTR) {                               // Print and assign addresses to literals.
    for(auto &x: LITTAB[controlSect]) {
        if(x.second == -1) {
            ostringstream loc;
            loc << hex << LOCCTR;
            if(relLocs[controlSect].back()!=LOCCTR) 
                relLocs[controlSect].push_back(LOCCTR);
            padStr = loc.str();
            pad_len = FOUR;
            string currLine = pad(padStr, pad_len);
            currLine += " *";
            currLine += "        ";
            currLine += "=";
            currLine += x.first;
            fout<<currLine<<endl;

            x.second = LOCCTR;
            int jump = computeLength(x.first);
            LOCCTR = LOCCTR+ jump;
        }
    }
}

void writeRecord (ofstream& fout, objCode& obj, string& currRecord, string& LOC, int& recLength) {                                  // Write Record Function-1
    if(isEquals(currRecord, "T")) currRecord+=pad(LOC,SIX)+"00";
    if(recLength+obj.getLen()>30) {                                       
        ostringstream out;  
        out << hex << recLength;
        string temp=pad(out.str(),2);
        int i=SEVEN;
        while(i<NINE)
        {
            currRecord[i]=temp[i-SEVEN];
            i++;
        }

        capitalize(currRecord);
        fout<<currRecord<<endl;

        currRecord = "T"+pad(LOC,SIX)+"00";
        recLength = ZERO;
    }
    currRecord+=obj.getObj();
    recLength+=obj.getLen();

}

void writeRecordTwo(ofstream &fout, string& objectCode, int incr, string& currRecord, string& LOC, int&recLength) { 

                // Write Record Function-2

    padStr = LOC;
    pad_len = SIX;
    if(isEquals(currRecord, "T")) currRecord+=pad(LOC,SIX)+"00";
    if(recLength+incr>30) {                                       
        ostringstream out;  
        out << hex << recLength;
        padStr = out.str();
        pad_len = 2;
        string temp=pad(padStr,pad_len);

        for(int i=SEVEN;i<NINE;i++)
        {
            currRecord[i]=temp[i-SEVEN];
        }

        capitalize(currRecord);
        fout<<currRecord<<endl;
        padStr = LOC;
        pad_len = SIX;
        currRecord = "T"+pad(padStr,pad_len)+"00";
        recLength = ZERO;

        currRecord+=objectCode;
        recLength+=incr;
    }else{
        currRecord+=objectCode;
        recLength+=incr;
    }
    
}

void endRec(ofstream& fout, string& currRecord, int recLength) {                                            // End current record
    if(isEquals(currRecord,"T")) return;
    ostringstream out;  
    out << hex << recLength;
    string temp=pad(out.str(),2);

    for(int i=SEVEN;i<NINE;i++)
    {
        currRecord[i]=temp[i-SEVEN];
    }

    capitalize(currRecord);
    fout<<currRecord<<endl;
}

bool blank(const string& str) {                                                                         // Check if string is blank
    for(int i=0; i<str.size(); i++) 
    {
        if(str[i]!=' ') return FALSE;
    }
    return TRUE;
}

int evalExpression(string& Operand, int controlSect, string& modRecord, int currLoc) {                  // Evaluate Expression
    char operations[ARRAY_SIZE];
    int operationsSize = ZERO;
    string labels[ARRAY_SIZE];
    int labelsSize = ZERO;
    operations[ operationsSize ] = '+';
    operationsSize++;
    int start = ZERO;
    int i=ZERO;
    
    do
    {
        if(Operand[i]=='+') {
            labels[ labelsSize ] = Operand.substr(start, i-start);
            labelsSize++;

            operations[ operationsSize ] = Operand[i];
            operationsSize++;
            start = i+1;
        } 
        if(Operand[i]=='-'){
            labels[ labelsSize ] = Operand.substr(start, i-start);
            labelsSize++;

            operations[ operationsSize ] = Operand[i];
            operationsSize++;
            start = i+1;
        } 
        i++;
    }while(i<Operand.size());

    labels[ labelsSize ] = Operand.substr(start, Operand.size() - start);
    labelsSize++;

    int val = ZERO;
    i=ZERO;
    
    do
    {
        if(SYMTAB[controlSect].find(labels[i]) != SYMTAB[controlSect].end()) 
        {
            if(operations[i] == '+') val+=SYMTAB[controlSect][labels[i]];
            if(operations[i] == '-') val-=SYMTAB[controlSect][labels[i]];
        }
        else if(extrefs[controlSect].find(labels[i]) != extrefs[controlSect].end()) 
        {
            ostringstream out;
            out<<hex<<(currLoc);
            padStr = out.str();
            pad_len = SIX;   
            modRecord+="M"+pad(padStr, pad_len)+("06");
            if(operations[i] == '+') {
                modRecord+="+";
            }
            if(operations[i] == '-') {
                modRecord+="-";
            }
            modRecord+=labels[i]+"\n";
        }
        i++;
    }while(i<labelsSize);

    return val;
}
void dumpLiterals(ofstream& fout, int controlSect, int& LOCCTR) {                               // Print and assign addresses to literals.
    for(auto &x: LITTAB[controlSect]) {
        if(x.second == -1) {
            ostringstream loc;
            loc << hex << LOCCTR;
            if(relLocs[controlSect].back()!=LOCCTR) 
                relLocs[controlSect].push_back(LOCCTR);
            padStr = loc.str();
            pad_len = FOUR;
            string currLine = pad(padStr, pad_len);
            currLine += " *";
            currLine += "        ";
            currLine += "=";
            currLine += x.first;
            fout<<currLine<<endl;

            x.second = LOCCTR;
            int jump = computeLength(x.first);
            LOCCTR = LOCCTR+ jump;
        }
    }
}

int main() {
    Error=FALSE;
    init_1();

    if(!Error) passOne();
    if(!Error) passTwo();

    return ZERO;
}

