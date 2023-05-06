
#include "scanner.hh"
#include "symbolTab.h"
#include "parser.tab.hh"
#include <fstream>
#include <vector>
using namespace std;

SymbTab gst, gstfun, gststruct; 
string filename;
string curr_fun;
extern std::map<std::string, vector<string>> code;
extern vector<std::string> function_order;
extern std::map<string,abstract_astnode*> ast;
//extern std::vector<string> gencode;
std::map<std::string, std::string> predefined {
            {"printf", "VOID_TYPE"},
            {"scanf", "VOID_TYPE"},
            {"mod", "INT_TYPE"}
        };
int main(int argc, char **argv)
{
	using namespace std;
	fstream in_file, out_file;
	

	in_file.open(argv[1], ios::in);

	IPL::Scanner scanner(in_file);

	IPL::Parser parser(scanner);

#ifdef YYDEBUG
	parser.set_debug_level(1);
#endif
parser.parse();
// create gstfun with function entries only
for (const auto &entry : gst.Entries)
{
	if (entry.second.varfun == "fun")
	gstfun.Entries.insert({entry.first, entry.second});
}

// create gststruct with struct entries only

for (const auto &entry : gst.Entries)
{
	if (entry.second.varfun == "struct")
	gststruct.Entries.insert({entry.first, entry.second});
}
// start the JSON printing

std::string s =argv[1];
cout<<"\t.file\t\""+s+"\"\n";
cout<<"\t.text\n";
cout<<"\t.section\t.rodata\n";

//NOTE : need to change the offsets of the parameters in the symbol table
//need to decrease them by 4


for(int i =0; i<function_order.size();i++){
	curr_fun = function_order[i];
	code[curr_fun] = vector<std::string>();
	code[curr_fun].push_back("\t.text\n");
	code[curr_fun].push_back("\t.globl\t"+curr_fun+"\n");
	code[curr_fun].push_back("\t.type\t"+curr_fun+", "+"@function\n");
	code[curr_fun].push_back(curr_fun+":\n");
	code[curr_fun].push_back("\tpushl\t%ebp\n"); 
	code[curr_fun].push_back("\tmovl\t%esp, %ebp\n");
	int size_for_locals = 0;
	for(auto local:gst.Entries[curr_fun].Entries){
		if (local.second.scope=="local")
			size_for_locals += local.second.size;
	}
	code[curr_fun].push_back("\tsubl\t$"+std::to_string(size_for_locals)+", %esp\n");
	ast[curr_fun]->print(0);
	if(code[curr_fun][code[curr_fun].size()-1]!="\tret\n"){
		code[curr_fun].push_back("\tleave\n");
		code[curr_fun].push_back("\tret\n");
	}
	for(auto line: code[function_order[i]]){
		cout<<line;
	}
}

fclose(stdout);
}