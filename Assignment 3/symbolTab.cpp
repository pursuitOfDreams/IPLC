#include "symbolTab.h"

using namespace std;

Symbol::Symbol(){
    this->size = 1;
    this->offset = 0;
    this->isarray = false;
}

void Symbol::print(){
    cout<< "["<<endl;
    int cnt =0;
    for(auto i: this->Entries){
        cout<<"["<<endl;
        cout<<"\""<<i.first<<"\""<<","<<endl;
        cout<<"\""<<i.second.varfun<<"\""<<","<<endl;
        cout<<"\""<<i.second.scope<<"\""<<","<<endl;
        cout<<"\""<<i.second.rem_type<<"\""<<","<<endl;
        cout<<i.second.size<<","<<endl;
        if (i.second.varfun=="struct"){
            cout<<"\"-\","<<endl;
            cout<<"\"-\""<<endl;
        }
        else{
            cout<<i.second.offset<<","<<endl;
            cout<<"\""<<i.second.returnType<<"\""<<endl;
        }
        if (cnt==this->Entries.size()-1){
            cout<<"]"<<endl;
        }
        else{
            cout<<"],"<<endl;
        }
        cnt++;
    }
    cout<<"]"<<endl;
}
Symbol::Symbol(string symbolName, string varfun, string scope, string rt, int size, int offset){
    this->symbolName = symbolName;
    this->varfun = varfun;
    this->scope = scope;
    this->returnType = rt;
    this->size = size;
    this->isarray = false;
    this->offset = offset;
}

void SymbTab::print(){
    cout<< "["<<endl;
    int cnt =0;
    for(auto i: this->Entries){
        cout<<"["<<endl;
        cout<<"\""<<i.first<<"\""<<","<<endl;
        cout<<"\""<<i.second.varfun<<"\""<<","<<endl;
        cout<<"\""<<i.second.scope<<"\""<<","<<endl;
        cout<<i.second.size<<","<<endl;
        if (i.second.varfun=="struct"){
            cout<<"\"-\","<<endl;
            cout<<"\"-\""<<endl;
        }
        else{
            cout<<i.second.offset<<","<<endl;
            cout<<"\""<<i.second.returnType<<"\""<<endl;
        }
        if (cnt==this->Entries.size()-1){
            cout<<"]"<<endl;
        }
        else{
            cout<<"],"<<endl;
        }
        cnt++;
    }
    cout<<"]"<<endl;
}