#include<bits/stdc++.h>
using namespace std;

string ip_file,startingAddress,progaddress,csaddress,cslth,execaddr;
ifstream fin,fin2;
ofstream fout;

void increment_by_one(int& x){
	x++;
}

class ESTAB_ENTRY{
	public:
		string name;
		string address;
		string length;
		int isSymbol;

	ESTAB_ENTRY(){

	}
	ESTAB_ENTRY(string name, string address, string length, int isSymbol){
		this->name = name;
		this->address = address;
		this->length = length;
		this->isSymbol = isSymbol;
	}

	void setName(string name){
		this->name = name;
	}

	void setAddress(string address){
		this->address = address;
	}

	void setLength(string length){
		this->length = length;
	}

	void setIsSymbol(int isSymbol){
		this->isSymbol = isSymbol;
	}

	// --

	string getName(){
		return this->name ;
	}

	string getAddress(){
		return this->address ;
	}

	string getLength(){
		return this->length ;
	}

	int getIsSymbol(){
		return this->isSymbol ;
	}

	void printH()
	{
		fout<<this->name<<"\t"<<"*\t"<<this->address<<"\t"<<this->length<<endl;
	}

	void printD()
	{
		fout<<"*"<<"\t"<<this->name<<"\t"<<this->address<<"\t"<<this->length<<endl;
	}
};

struct finalval
{
	string val;
	int isPresent = -1;
	int no_half_bytes;
};

map<string,ESTAB_ENTRY> estab_map;
int isStarted = 0;

string substring(string s, int start, int end)
{
	if(end>=start){
		return s.substr(start,end);
	}
	end--;
	string subs = "";
	for(int i=start;i<=end;increment_by_one(i))
	{
		subs += s[i];
	}
	return subs;
}

int hex_to_dec(char a)
{
	if(a>='0' && a<='9')
	{
		return a-'0';
	}
	else if(a>='A' && a<='F')
	{
		return a-'A'+10;
	}
	return 0;
}              

char dec_to_hex(int a)
{
	if(a>=0 && a<=9)
	{
		return '0'+a;
	}
	else if(a>=10 && a<=15){
		return a-10+'A';
	}

	return '0';
}

int string_length(string a){
	return a.length();
}

void reverse_string(string &a){
	reverse(a.begin(), a.end());
}


string Add_2_hex_strings(string a, string b) 
{ 
    if (string_length(a) < string_length(b))
    {
    	string t = a;
    	a = b;
    	b = t;
    }

    int l1 = string_length(a);
    int l2 = string_length(b);

    string ans = ""; 

    int s;
    int c = 0, i, j; 

    i=l1-1;
    j=l2-1;

    do
    {
    	s = hex_to_dec(a[i]);
        s+= hex_to_dec(b[j]);
        s+=c;  
        ans.push_back(dec_to_hex(s % 16)); 
        c = s / 16;
        i--; j--;
    } while(j>=0);

    do 
    { 
        s = hex_to_dec(a[i]);
        s+=c;  
        i--; 
        c = s / 16; 
        ans.push_back(dec_to_hex(s % 16)); 
    } while(i>=0);

    if(c!=0) 
    { 
        ans.push_back(dec_to_hex(c)); 
    } 

    reverse_string(ans);
    return ans; 
} 


void handle_H(string& templine)
{
	ESTAB_ENTRY ob;
	string csectname;
	int index;
	string addr1;
	string length;
	
	index = 2;
	do{
		csectname+=templine[index++];
	}while(templine[index]!='^');

	do
	{
	}
	while(templine[index++]!='^');
	index = index + 1;
	while(1)
	{
		if(templine[index]=='^')
			break;
		addr1+=templine[index++];
	}
	index = index + 1;
	do
	{
		length+=templine[index++];
	}while(index< string_length(templine));	

	if(!isStarted)
	{
		startingAddress = Add_2_hex_strings(addr1,csaddress);
		csaddress = progaddress;
		isStarted ='1'-'0';
	}

	ob.length = length;
	ob.address = Add_2_hex_strings(addr1,csaddress);
	ob.name = csectname;
	ob.isSymbol = 2;
	estab_map[csectname] = ob;
	cslth = length;
	fout<<ob.name<<"\t"<<"*\t"<<ob.address<<"\t"<<ob.length<<endl;
}


void handle_D(string& templine)
{
	int index = 2;
	do
	{

		ESTAB_ENTRY ob;
		string name;
		string addr1;
		string length;
		length ="0";
		do
		{
			name+=templine[index];
			increment_by_one(index);
		}while(templine[index]!='^');
		increment_by_one(index);
		do
		{
			addr1+=templine[index];
			increment_by_one(index);
		}while(index<string_length(templine) && templine[index]!='^');

		increment_by_one(index);
		
		ob.address = Add_2_hex_strings(addr1,csaddress);
		ob.length = length;
		ob.name = name;
		ob.isSymbol = 0;
		estab_map[name] = ob;
		fout<<"*"<<"\t"<<name<<"\t"<<ob.address<<"\t"<<ob.length<<endl;
	}while(index < string_length(templine));
	
}


void handle_T(string& currline)
{
	while(1)
	{
		if(!getline(fin,currline))
		{
			break;
		}
		if(string_length (currline) > 0)
		{
			if(currline[0]=='E')
			{
				csaddress = Add_2_hex_strings(csaddress,cslth);
				break;
			}
		}
	}	
}


string mypass1()
{
	string address;
	string currline;

	fin.open(ip_file);
	fout.open("loadmap.txt");

	do
	{
		if(!getline(fin,currline))
			break;	
		if(currline[0]=='D')
		{
			handle_D(currline);
		}
		if(currline[0]=='H')
		{
			handle_H(currline);
		}
		if(currline[0]=='T')
		{
			handle_T(currline);
		}
    }while(1);

	fout.close();
	fin.close();
	
	return "Done";
}

void print_gap_lines(string begin , string ending)
{
	do
	{
		if(begin==ending)
			break;
		fout<<begin<<"\t"<<"XXXXXXXX"<<"\t"<<"XXXXXXXX"<<"\t"<<"XXXXXXXX"<<"\t"<<"XXXXXXXX"<<"\n";
		begin = Add_2_hex_strings(begin,"10");
	}while(1);

}


int HexStringToInt(string x)
{
  return stoul(x,nullptr,16);
}

string IntToHexString(int x,int fill = 6)
{
  stringstream s;
  s << setfill('0') << setw(fill) << hex << x;
  string ans = s.str();
  ans = substring(ans,ans.length()-fill,fill);

	string s1 = ans;
	for(int i=0;i<s1.size();i++) { 
      /* Here we are performing a check so that only lowercase 
       * characters gets converted into uppercase. 
       * ASCII value of a to z(lowercase chars) ranges from 97 to 122 
       */ 
      if(s1[i]>=97 && s1[i]<=122)
      {
		s1[i]=s1[i]-32;
      }
   }

  return s1;
}

string subtract_2_hex_strings(string a, string b)
{
	return IntToHexString(HexStringToInt(a)-HexStringToInt(b));
}


string nearest_addr_ending_in_0(string s)
{
	if(s[string_length(s)-1]=='0')
	{

	}
	else
	{
		while(s[string_length(s)-1]!='0')
		{
			s = subtract_2_hex_strings(s,"1"); 
		}
		
	}
	return s;
}

string remove_preceeding_0s(string x){
	string temp="";
	int i=0;
	if(string_length(x) <= 4)
	{
	 	return x;
	}

	do
	{
		increment_by_one(i);
	}while(i < string_length(x) && x[i]=='0');
	do
	{
		temp+=x[i];
		increment_by_one(i);

	}while(i< string_length(x)); 
	return temp;

}

int calculate_start_index(string hexdiff)
{
		string temp="";
		int j=0;
		while(j < string_length(hexdiff))
		{
			if(hexdiff[j]!='0') 
			{
				temp+=hexdiff[j];
			}
			increment_by_one(j);
		}

		hexdiff=temp;
		if(string_length(hexdiff) -1 > 0) 
		{
			return -1;
		}
		return hex_to_dec(hexdiff[0])+hex_to_dec(hexdiff[0]);
}

void reset_buffer(char arr[])
{
	int i=0;
	do
	{
		arr[i]='X';
		increment_by_one(i);
	}while(i<32);
}

void assign11(int& a, int& b)
{
	a=1; b=1;
	return;
}
void assign10(int& a, int& b)
{
	a=1; b=0;
	return;
}
void assign00(int& a, int& b)
{
	a=0; b=0;
	return;
}
void assign01(int& a, int& b)
{
	a=0; b=1;
	return;
}
void mypass2_1()
{
	string s = "XXXXXXXX";
	csaddress = progaddress;
	execaddr = progaddress;
	string currline;
	string beg;
	string end;
	string addr_text_starts;
	string csectname;
	char linestartswith;
	int firstsection = 0;
	string memloc_column;

	fin.open(ip_file);
	fout.open("loaderoutput_primary.txt");

	char buffer[32];

	int i=0;
	do
	{
		buffer[i]='X';
		increment_by_one(i);
	}while(i<32);


	while(true)
	{
		if(!getline(fin,currline)) break;

		linestartswith = currline[0];

		if(currline[0] == 'H')
		{
			int i = 2;
			csectname = "";
			for(; i< string_length(currline) ; increment_by_one(i))
			{
				if(currline[i]=='^') 
					break;
				csectname+=currline[i];
			}

			if(firstsection==0)
			{
				csaddress = progaddress;
				execaddr = progaddress;
				firstsection=1;
				string temp = nearest_addr_ending_in_0(csaddress);
				beg = remove_preceeding_0s(temp);
			}
			else
			{
				csaddress = estab_map[csectname].getAddress();
				string temp = nearest_addr_ending_in_0(csaddress);
				beg = remove_preceeding_0s(temp);
			}
			cslth = "";
			cslth = estab_map[csectname].getLength();

			if(beg!=memloc_column)	
			{
				fout<<beg<<"\t"<<s<<"\t"<<s<<"\t"<<s<<"\t"<<s<<"\n";
			}
			string temp = Add_2_hex_strings(beg,"10");
			beg = remove_preceeding_0s(temp);
		}	
		else if (linestartswith=='T')
		{
			int t=2; addr_text_starts="";
			while(currline[t]!='^') 
			{
				addr_text_starts+=currline[t];
				increment_by_one(t);
			}
			addr_text_starts = Add_2_hex_strings(csaddress,addr_text_starts);
			increment_by_one(t);
			
			string temp = nearest_addr_ending_in_0(addr_text_starts);
			memloc_column = remove_preceeding_0s(temp);

			
			end = memloc_column;
			print_gap_lines(beg,end);

			fout<<memloc_column<<"\t";

			string hexdiff = subtract_2_hex_strings(addr_text_starts,memloc_column);
			int startIndex = calculate_start_index(hexdiff);

			while(currline[t]!='^') 
			{
				increment_by_one(t);
			}
			increment_by_one(t);

			for(int j=0;j<startIndex;increment_by_one(j)) 
			{
				fout<<buffer[j];
				if((j+1)%8==0)
				fout<<"\t";
			}
			
			
			do
			{
				int ft=0,fsi=0;
				while(startIndex<32&& t<currline.length() &&currline[t]!='^')
				{
					buffer[startIndex] = currline[t];
					fout<<buffer[startIndex];
					if((startIndex+1)%8==0){
						fout<<"\t";
					}
					increment_by_one(startIndex);
					increment_by_one(t);


				}
				if(!(t< string_length(currline)))
				{
					break;
				}

				if(startIndex==32 && currline[t]!='^') 
				{
					assign01(ft,fsi);
				}
				if(startIndex==32 && currline[t]=='^') 
				{
					assign11(ft,fsi);
				}
				if(startIndex!=32 && currline[t]=='^')
				{
					assign10(ft,fsi);
				}
				
				if (!ft && fsi)
				{
					startIndex=0;

					string temp = Add_2_hex_strings(memloc_column,"10");
					memloc_column = remove_preceeding_0s(temp);

					fout<<"\n"<<memloc_column<<"\t";
					reset_buffer(buffer);
				}
				if(ft && !fsi)
				{
					increment_by_one(t);
				}
				if(ft && fsi)
				{
					increment_by_one(t);
					startIndex=0;

					string temp = Add_2_hex_strings(memloc_column,"10");
					memloc_column = remove_preceeding_0s(temp);

					fout<<"\n"<<memloc_column<<"\t";
					reset_buffer(buffer);
				}
			}while(t!=currline.length());
			 
			do
			{
				fout<<buffer[startIndex];
				if(startIndex==7||startIndex==15||startIndex==23) fout<<"\t";
				if(startIndex==31) fout<<endl;
				increment_by_one(startIndex);
				
			}while(startIndex < 32);
			reset_buffer(buffer);
			beg = Add_2_hex_strings(memloc_column,"10");			
		}
		else if (linestartswith=='M')
		{
			string operand_addr;
		}
		else if (linestartswith=='E')
		{

		}
	}
	fin.close();
	fout.close();
}

map<string, string> loader_primary;
map<int,string> helpermap;
static int counter_addr=0;



map<string,finalval> final_val;


string	calculateOperand(int offset,int len_data, string addr_label){
	string operand="";
	if(len_data%2!=0) 
	{
		offset+=1;
	}
	int StartIndex = offset,counter=0;

	do
	{
		increment_by_one(counter);
		
		if(counter==len_data+1) break;
		operand+=loader_primary[addr_label][StartIndex];
		increment_by_one(StartIndex);
		
	}while(StartIndex<32);

	if(!(counter==len_data+1))
	{
		addr_label = Add_2_hex_strings(addr_label,"10");
		StartIndex=0;
		do
		{
			increment_by_one(counter);
			if(counter==len_data+1) break;
			operand+=loader_primary[addr_label][StartIndex];
			increment_by_one(StartIndex);
		}while(StartIndex<32);

	}
	return operand+"";
}

void updateloaderPrimary(string label_addr, int len_data, string newval, int offset){
	int StartIndex = offset, j=0;
	string temp="";

	do
	{
		loader_primary[label_addr][StartIndex] = newval[j];
		temp+=loader_primary[label_addr][StartIndex];
		increment_by_one(StartIndex);
		increment_by_one(j);
	}while(StartIndex<32 && j<len_data);


	if(j!=len_data)
	{
		label_addr = Add_2_hex_strings(label_addr,"10");
		StartIndex=0;

		do
		{
			loader_primary[label_addr][StartIndex] = newval[j];
			temp+=loader_primary[label_addr][StartIndex];
			increment_by_one(StartIndex);
			increment_by_one(j);
		}while(StartIndex<32 && j<len_data);
	}

}

void mypass2()
{
		vector<string> list_modified_addrs; 
		string currline,csectname,csaddr;
		mypass2_1();
		fin.open("loaderoutput_primary.txt");
		do
		{
			if(!getline(fin,currline))
				break;
			int i=0;
			string saddr;
			string arr;
			do 
			{
				saddr+=currline[i];
				increment_by_one(i);
			}while(currline[i]!='\t');

			do
			{
				if(currline[i]=='\t')
				{
					increment_by_one(i);
	
				} 				
				else
				{
					arr+=currline[i];
					increment_by_one(i);
				}
			}while(i< string_length(currline));

			loader_primary[saddr] = arr;
			helpermap[counter_addr] = saddr;
			increment_by_one(counter_addr);
		}while(true);

		fin.close();
		fin.open(ip_file);
		fin2.open("loadmap.txt");

		do
		{
			if(!getline(fin,currline))
				break;

			char linestartswith = currline[0];
			if(linestartswith=='H')
			{	
				int i=2; csectname="";
				 
				do
				{
					csectname+=currline[i];
					increment_by_one(i);
				}while(currline[i]!='^');
				csaddr = estab_map[csectname].address;
			}
			else if(linestartswith=='M'){
				int i=2; string addr_to_be_modified="";
				
				do
				{ 
					addr_to_be_modified+=currline[i];
					increment_by_one(i);
				}while(currline[i]!='^') ;
				string temp = addr_to_be_modified;
				addr_to_be_modified=Add_2_hex_strings(addr_to_be_modified,csaddr);
				increment_by_one(i);

				string temp1 = nearest_addr_ending_in_0(addr_to_be_modified);
				string label_addr = remove_preceeding_0s( temp1);
				string offset = subtract_2_hex_strings(addr_to_be_modified,label_addr);
				string num_half_bytes="";

				 
				do
				{
					num_half_bytes+=currline[i];
					increment_by_one(i);
				}while(currline[i]!='^');
				int no_hb = HexStringToInt(num_half_bytes);
				increment_by_one(i);
				char operator_ = currline[i];
				string sym="";
				increment_by_one(i);

				do
				{
					sym+=currline[i];
					increment_by_one(i);
				}while(i<currline.length()); 

				int offset_ = HexStringToInt(offset)*2;

				string operand;
				
				operand = calculateOperand(offset_,no_hb,label_addr);

				final_val[addr_to_be_modified].no_half_bytes = no_hb;
				

				string ans;

				switch (operator_)
				{
					case '+':
						if(final_val[addr_to_be_modified].isPresent!=1) 
						{
							final_val[addr_to_be_modified].val = operand;
							final_val[addr_to_be_modified].isPresent =1;
						}
						ans = Add_2_hex_strings(final_val[addr_to_be_modified].val, estab_map[sym].getAddress());

						if(no_hb%2==1) ans=ans.substr(1,ans.length()-1);

						final_val[addr_to_be_modified].val = ans;
						break;
					
					case '-':
						if(final_val[addr_to_be_modified].isPresent!=1) 
						{
							final_val[addr_to_be_modified].val = operand;
							final_val[addr_to_be_modified].isPresent =1;
						}
						ans = subtract_2_hex_strings(final_val[addr_to_be_modified].val, estab_map[sym].getAddress());
						if(no_hb%2==1) ans=ans.substr(1,ans.length()-1);

						final_val[addr_to_be_modified].val = ans;
						break;

					default:
						cout<<"Invalid operator\n";
						break;
				}

				list_modified_addrs.push_back(addr_to_be_modified);

			}
			else if(linestartswith=='E')
			{
				int i=0;
				do
				{

					string temp = nearest_addr_ending_in_0(list_modified_addrs[i]);
					string label_addr = remove_preceeding_0s( temp);


					string offset = subtract_2_hex_strings(list_modified_addrs[i],label_addr);	
					int offset_ = HexStringToInt(offset)*2;
					string finaldata = final_val[list_modified_addrs[i]].val;
					int no_half_bytes = final_val[list_modified_addrs[i]].no_half_bytes;
					temp="";
					if(finaldata.length()>no_half_bytes) 
					{
						temp=finaldata;
						reverse_string(temp);
						temp = temp.substr(0,no_half_bytes);
						reverse_string(temp);
						finaldata=temp;
					}

					if(finaldata.length()%2==1) offset_+=1;
					updateloaderPrimary(label_addr,finaldata.length(),finaldata,offset_);
					increment_by_one(i);
				}while(i<list_modified_addrs.size());

				list_modified_addrs.clear();
			}

		}while(1);
		fin.close();
		fin2.close();
		fout.close();

		fout.open("LOADEROUTPUT.txt");

		int t=0;
		do
		{
			string templine = loader_primary[helpermap[t]]; 
			fout<<helpermap[t]<<"\t"<< substring(templine,0,8)<<"\t"<<substring(templine,8,8)<< "\t" <<templine.substr(16,8)<<"\t"<<templine.substr(24,8)<<"\n";
			increment_by_one(t);
		}while(t<helpermap.size());

		fout.close();
		remove("loaderoutput_primary.txt");

}

int main(){

	progaddress ="4000"; // default
	ip_file = "lul.txt";
	cout<<"\nType the name of input file: ";
	cin>>ip_file;
	cout<<"\nEnter starting addr : ";
	cin>> progaddress; 
	mypass1();
	cout<<"\nloadmap.txt file has been generated\n";
	mypass2();
	cout<<"\nLOADEROUTPUT.txt file has been generated\n\n";


}
