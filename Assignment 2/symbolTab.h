#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include<iostream>
#include<string>
#include<map>

using namespace std;


class Symbol {
public:
    string symbolName;
    string varfun; // fun/var type
    string scope; // global, local, param
    string returnType; //  ex INT, FLOAT, VOID
    string base_type;
    string rem_type;
    int size;
    int offset; // by default the offset will be zero for global variables
    bool isarray;
    map<string, Symbol> Entries;
    Symbol();
    Symbol(string symbolName, string varfun, string scope, string rt, int size, int offset);
    void print();
};

class SymbTab {
public:
    map<string, Symbol> Entries; // variable name and symbol information 
    SymbTab(){};
    void print();
};

#endif
