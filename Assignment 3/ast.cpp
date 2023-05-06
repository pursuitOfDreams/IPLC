#include "ast.h"
#include "symbolTab.h"
#include<map>
#include<string>
#include<vector>

using namespace std;
extern std::map<std::string,std::vector<std::string>> code;
extern SymbTab gst;
int label_counter=0;
extern std::string curr_fun;
std::string global_literal = "";
int global_offset = 0;
bool is_arrayref = false;

std::vector<int> get_indices(std::string rem_type){
    //  cout<<"rem_type in function "+rem_type<<endl;
    std::string num="";
    vector<int> indices;
    bool is_num = false;
    for(auto i: rem_type){
        if (i=='['){
            is_num=true;
        }
        else if (is_num && isdigit(i)){
            num += i;
        }
        else if (i==']'){
            indices.push_back(std::stoi(num));
            is_num =false;
            num ="";
        }
    }
    // for (auto elem : indices){
    //     cout<<elem<<endl;
    // }
    return indices;
}

int accumulate(vector<int> a){
    int pdt = 1;
    for(auto i: a){
        pdt *= i;
    }
    return pdt;
}
exp_astnode::exp_astnode(){
    //this->is_array = false;
}
op_binary_astnode::op_binary_astnode(std::string binaryOp,exp_astnode* leftNode,exp_astnode* rightNode){
    this->binaryOp = binaryOp;
    this->leftNode = leftNode;
    this->rightNode = rightNode;
}

void op_binary_astnode::print(int flag){
    if (this->binaryOp=="MULT"){

        this->leftNode->print(flag);
        this->rightNode->print(flag);

        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\timul\t%edx, %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");

        if(flag != 2){

            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;

            code[curr_fun].push_back("\there mult "+std::to_string(flag)+"\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tjne\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");

        }
    }
    else if (this->binaryOp=="DIV"){
        this->leftNode->print(flag);
        this->rightNode->print(flag);

        code[curr_fun].push_back("\tpopl\t%ebx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tcltd\n");
        code[curr_fun].push_back("\tidivl\t%ebx\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        
        if(flag != 2){

            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            
            code[curr_fun].push_back("\there "+std::to_string(flag)+"\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tjne\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");

        }
    }
    else if (this->binaryOp=="PLUS"){
        this->leftNode->print(flag);
        this->rightNode->print(flag);

        if (leftNode->rem_type!=""){
            code[curr_fun].push_back("\tpopl\t%edx\n"); // remove b from stack 
            code[curr_fun].push_back("\tpopl\t%eax\n"); // remove b from stack 
            code[curr_fun].push_back("\timul\t$4, %edx\n"); // remove b from stack 
            code[curr_fun].push_back("\taddl\t%edx, %eax\n");  
            code[curr_fun].push_back("\tpushl\t%eax\n"); // push a+b to the stack
        }
        else if (rightNode->rem_type!=""){
            code[curr_fun].push_back("\tpopl\t%eax\n"); // remove b from stack 
            code[curr_fun].push_back("\tpopl\t%edx\n"); // remove b from stack 
            code[curr_fun].push_back("\timul\t$4, %edx\n"); // remove b from stack 
            code[curr_fun].push_back("\taddl\t%edx, %eax\n");  
            code[curr_fun].push_back("\tpushl\t%eax\n"); // push a+b to the stack
        }
        else{
            code[curr_fun].push_back("\tpopl\t%eax\n"); // remove b from stack 
            code[curr_fun].push_back("\tpopl\t%edx\n"); // remove b from stack 
            code[curr_fun].push_back("\taddl\t%edx, %eax\n");  
            code[curr_fun].push_back("\tpushl\t%eax\n"); // push a+b to the stack
        }

        if(flag != 2){
            
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
        
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tjne\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");

        }
        
    }
    else if (this->binaryOp=="MINUS"){
        this->leftNode->print(flag);
        this->rightNode->print(flag);
        if (leftNode->rem_type!=""){
            code[curr_fun].push_back("\tpopl\t%edx\n"); // remove b from stack 
            code[curr_fun].push_back("\tpopl\t%eax\n"); // remove b from stack 
            code[curr_fun].push_back("\timul\t$4, %edx\n"); // remove b from stack 
            code[curr_fun].push_back("\tsubl\t%edx, %eax\n");  
            code[curr_fun].push_back("\tpushl\t%eax\n"); // push a+b to the stack
        }
        else if (rightNode->rem_type!=""){
            code[curr_fun].push_back("\tpopl\t%eax\n"); // remove b from stack 
            code[curr_fun].push_back("\tpopl\t%edx\n"); // remove b from stack 
            code[curr_fun].push_back("\timul\t$4, %edx\n"); // remove b from stack 
            code[curr_fun].push_back("\tsubl\t%edx, %eax\n");  
            code[curr_fun].push_back("\tpushl\t%eax\n"); // push a+b to the stack
        }
        else{
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tsubl\t%ebx, %eax\n");
            code[curr_fun].push_back("\tpushl\t%eax\n");
        }

        if(flag != 2){

            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
        
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tjne\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");

        }


    }
    else if(this->binaryOp == "LT_OP"){

        this->leftNode->print(2);
        this->rightNode->print(2);
        
        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsetl\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tjl\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }
    }
    else if(this->binaryOp == "LE_OP"){

        this->leftNode->print(2);
        this->rightNode->print(2);

        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsetle\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tjle\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }

    }
    else if(this->binaryOp == "GT_OP"){

        this->leftNode->print(2);
        this->rightNode->print(2);
        
        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmpl\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsetg\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tjg\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }
        

    }
    else if(this->binaryOp == "GE_OP"){

        this->leftNode->print(2);
        this->rightNode->print(2);
        
        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsetge\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tjge\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }

    }
    else if(this->binaryOp == "EQ_OP"){

        this->leftNode->print(2);
        this->rightNode->print(2);
        
        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsete\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tje\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }

    }
    else if(this->binaryOp == "NE_OP"){
        this->leftNode->print(2);
        this->rightNode->print(2);
        if (flag==2){
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %edx\n");
            code[curr_fun].push_back("\tsetne\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->true_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            this->false_list.push_back(".L"+std::to_string(label_counter));
            label_counter++;
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%eax\n");
            code[curr_fun].push_back("\tcmp\t%ebx, %eax\n");
            code[curr_fun].push_back("\tjne\t"+this->true_list[0]+"\n");
            code[curr_fun].push_back("\tjmp\t"+this->false_list[0]+"\n");
        }
    }
    else if(this->binaryOp == "OR_OP"){
        if (flag==2){
            this->leftNode->print(flag);
            this->rightNode->print(flag);
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");

            code[curr_fun].push_back("\tcmp\t$0, %ebx\n");
            code[curr_fun].push_back("\tsetne\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %ebx\n");

            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tsetne\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");

            code[curr_fun].push_back("\tor\t%ebx, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->leftNode->print(flag);
            for(auto elem : this->leftNode->false_list){
                code[curr_fun].push_back(elem+":\n");
            }
            this->rightNode->print(flag);
            this->true_list = leftNode->true_list;
            this->true_list.insert(this->true_list.end(), this->rightNode->true_list.begin(), this->rightNode->true_list.end());
            this->false_list = this->rightNode->false_list;
        }
    }
    else if (this->binaryOp=="AND_OP"){
        if (flag==2){
            this->leftNode->print(flag);
            this->rightNode->print(flag);
            code[curr_fun].push_back("\tpopl\t%ebx\n");
            code[curr_fun].push_back("\tpopl\t%edx\n");

            code[curr_fun].push_back("\tcmp\t$0, %ebx\n");
            code[curr_fun].push_back("\tsetne\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %ebx\n");

            code[curr_fun].push_back("\tcmp\t$0, %edx\n");
            code[curr_fun].push_back("\tsetne\t%al\n");
            code[curr_fun].push_back("\tmovzbl\t%al, %edx\n");

            code[curr_fun].push_back("\tand\t%ebx, %edx\n");
            code[curr_fun].push_back("\tpushl\t%edx\n");
        }
        else{
            this->leftNode->print(flag);
            for(auto elem : this->leftNode->true_list){
                code[curr_fun].push_back(elem+":\n");
            }
            this->rightNode->print(flag);
            this->false_list = leftNode->false_list;
            this->false_list.insert(this->false_list.end(), this->rightNode->false_list.begin(), this->rightNode->false_list.end());
            this->true_list = this->rightNode->true_list;
        }
    }
}

op_unary_astnode::op_unary_astnode(std::string unaryOp,exp_astnode* expression){
    this->unaryOp = unaryOp;
    this->expression = expression;
}

void op_unary_astnode::print(int flag){
    if(this->unaryOp == "PP"){
        this->expression->print(0);
        code[curr_fun].push_back("\tpopl\t%eax\n"); //this and below instaructions are not needed
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\taddl\t$1, %eax\n");
        code[curr_fun].push_back("\tmovl\t%eax, "+std::to_string(global_offset)+"(%ebp)\n");
    }
    else if(this->unaryOp =="ADDRESS"){
        this->expression->print(1);
    }
    else if (this->unaryOp =="NOT") {
        this->expression->print(2);
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tcmp\t$0, %eax\n");
        code[curr_fun].push_back("\tsete\t%al\n");
        code[curr_fun].push_back("\tmovzbl\t%al, %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else if (this->unaryOp=="DEREF"){
        this->expression->print(flag);


        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");

    }
    else if (this->unaryOp=="UMINUS"){
        this->expression->print(2);
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tmovl\t$0, %eax\n");
        code[curr_fun].push_back("\tsubl\t%edx, %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
}

assignE_astnode::assignE_astnode(exp_astnode* leftNode,exp_astnode* rightNode){
    this->leftNode = leftNode;
    this->rightNode = rightNode;
    this->astnode_type = typeExp::assignE;
}

void assignE_astnode::print(int flag){
    if (this->leftNode->base_type.size()>=6 && this->leftNode->rem_type==""){
        leftNode->print(1);
        rightNode->print(1);
        int size_struct = gst.Entries[this->leftNode->base_type].size;
        int no_of_iters = size_struct/4;
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        for(int i=0;i<no_of_iters;i++){
            code[curr_fun].push_back("\tmovl\t(%edx), %ebx\n");
            code[curr_fun].push_back("\tmovl\t%ebx, (%eax)\n");
            code[curr_fun].push_back("\taddl\t$4, %edx\n");
            code[curr_fun].push_back("\taddl\t$4, %eax\n");
        }
        
    }
    else{
        leftNode->print(1); // flag==1 for storing address in the stack
        rightNode->print(2); // flag==0 for storung values in the stack
        for(auto elem: this->rightNode->true_list){
            code[curr_fun].push_back(elem+":\n");
        }
        for(auto elem: this->rightNode->false_list){
            code[curr_fun].push_back(elem+":\n");
        }
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t%edx, (%eax)\n");

    }
    
    
}

funcall_astnode::funcall_astnode(std::string identifier,vector<exp_astnode*> seq){
    this->seq = seq;
    this->identifier = identifier;
    this->astnode_type = typeExp::funcall;
}

void funcall_astnode::print(int flag){
    if (gst.Entries[this->identifier].base_type.size()>=6){
        int size_of_return = gst.Entries[gst.Entries[this->identifier].base_type].size;
        // for the return value of only struct
        code[curr_fun].push_back("\tsubl\t$"+std::to_string(size_of_return+4)+", %esp\n");
    }
    for(auto elem: this->seq){
        if (elem->base_type.size()>=6 && elem->base_type.substr(0,6)=="struct" && elem->rem_type==""){
                // struct and not a pointer or array
                elem->print(1);
                int size_struct = gst.Entries[elem->base_type].size;
                int no_of_iters = size_struct/4;
                code[curr_fun].push_back("\tpopl\t%eax\n");
                code[curr_fun].push_back("\taddl\t$"+std::to_string(size_struct-4)+", %eax\n");
                for(int i=0;i<no_of_iters;i++){
                    code[curr_fun].push_back("\tmovl\t(%eax), %edx\n");
                    code[curr_fun].push_back("\tpushl\t%edx\n");
                    code[curr_fun].push_back("\tsubl\t$4, %eax\n");
                }
        }
        else if (elem->rem_type!="" && !elem->is_array){
            elem->print(0); // if the element is pointer then pass the value
        }
        else if (elem->is_array){
            elem->print(1); //there shouldnt be a need to check is_array here because even for pointers we
        }
        else{
            elem->print(2); //not sure if 2 is necessary
        }
    }

    code[curr_fun].push_back("\tcall\t"+this->identifier+"\n");
    if (this->seq.size()!=0)
    {
        int size_params = 4*this->seq.size();
        code[curr_fun].push_back("\taddl\t$"+std::to_string(size_params)+", %esp\n");
    }
    
    if (gst.Entries[this->identifier].base_type.size()>=6){
        int size_of_return = gst.Entries[gst.Entries[this->identifier].base_type].size;
        // for the return value of only struct
        code[curr_fun].push_back("\taddl\t$"+std::to_string(size_of_return+4)+", %esp\n");
    }
    code[curr_fun].push_back("\tpushl\t%eax\n");
}

intconst_astnode::intconst_astnode(int term){
    this->term = term;
    this->astnode_type = typeExp::intconst;
}

void intconst_astnode::print(int flag){
    code[curr_fun].push_back("\tmovl\t$"+std::to_string(term)+", %eax\n");
    code[curr_fun].push_back("\tpushl\t%eax\n");
}

floatconst_astnode::floatconst_astnode(float term){
    this->term = term;
    this->astnode_type = typeExp::floatconst;
}

void floatconst_astnode::print(int flag){
    // cout<<"\"floatconst\": "<<this->term<<endl;
}

stringconst_astnode::stringconst_astnode(std::string term){
    this->term = term;
    this->astnode_type = typeExp::stringconst;
}

void stringconst_astnode::print(int flag){
    global_literal = this->term;
}

identifier_astnode::identifier_astnode(std::string identifier, int offset,std::string rem_type){
    this->offset = offset;
    this->identifier = identifier;
    this->rem_type = rem_type;
    this->astnode_type = typeExp::identifier;
}

void identifier_astnode::print(int flag){
    //this->rem_type = gst.Entries[curr_fun].Entries[this->identifier].rem_type;
    //code[curr_fun].push_back("this is it "+this->rem_type);

    global_offset = this->offset;
    // code[curr_fun].push_back("rem_type is "+this->rem_type+"\n");
    // code[curr_fun].push_back("scope is "+gst.Entries[curr_fun].Entries[this->identifier].scope+"\n");
    //code[curr_fun].push_back(std::to_string(this->is_array));
    //if (this->rem_type!="")
    // this line will handle only those cases where function needs array as parameter but won't handle cases of type casting
    if (is_arrayref && gst.Entries[curr_fun].Entries[this->identifier].scope=="param"){
        // array as input
        //code[curr_fun].push_back("here2\n");
        code[curr_fun].push_back("\tmovl\t"+std::to_string(this->offset)+"(%ebp), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else if (flag==1){
        // pointer waala
        //code[curr_fun].push_back("here1\n");
        code[curr_fun].push_back("\tleal\t"+std::to_string(this->offset)+"(%ebp), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else{
        //code[curr_fun].push_back("here\n");
        code[curr_fun].push_back("\tmovl\t"+std::to_string(this->offset)+"(%ebp), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
}

arrayref_astnode::arrayref_astnode(exp_astnode* array,exp_astnode* index){
    this->array = array;
    this->index = index;
    this->astnode_type = typeExp::arrayref;
}

void arrayref_astnode::print(int flag){
    is_arrayref = true;
    this->array->print(1);
    is_arrayref = false;
    this->index->print(2);
    vector<int> indices = get_indices(this->rem_type);
    int size = 4;
    if (this->array->base_type!="int")
        size = gst.Entries[this->array->base_type].size;
    int off = accumulate(indices);
    code[curr_fun].push_back("\tpopl\t%edx\n");
    code[curr_fun].push_back("\timul\t$"+std::to_string(off)+", %edx\n");
    code[curr_fun].push_back("\timul\t$"+std::to_string(size)+", %edx\n");
    code[curr_fun].push_back("\tpopl\t%eax\n");
    code[curr_fun].push_back("\taddl\t%edx, %eax\n");
    if ((indices.size()==0 && flag==1)||(indices.size()!=0)){
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else {
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    
}

member_astnode::member_astnode(exp_astnode* expression,std::string identifier){
    this->expression = expression;
    this->identifier = identifier;
    this->astnode_type = typeExp::member;
}

//need to check below function again
void member_astnode::print(int flag){ //may be wrong as not part of examples
    if(flag == 1){
        this->expression->print(1);
        code[curr_fun].push_back("\tmovl\t$"+std::to_string(this->offset)+", %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\taddl\t%edx, %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else{
        this->expression->print(1);
        code[curr_fun].push_back("\tmovl\t$"+std::to_string(this->offset)+", %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\taddl\t%edx, %eax\n");
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
}

arrow_astnode::arrow_astnode(exp_astnode* expression,std::string identifier){
    this->expression = expression;
    this->identifier = identifier;
    this->astnode_type = typeExp::arrow;
}

void arrow_astnode::print(int flag){
    if(flag == 1){
        this->expression->print(1);
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t$"+std::to_string(this->offset)+", %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\taddl\t%edx, %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }
    else{
        this->expression->print(1);
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tmovl\t$"+std::to_string(this->offset)+", %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\taddl\t%edx, %eax\n");
        code[curr_fun].push_back("\tmovl\t(%eax), %eax\n");
        code[curr_fun].push_back("\tpushl\t%eax\n");
    }

}

empty_astnode::empty_astnode(){this->astnode_type = typeExp::empty;}

void empty_astnode::print(int flag){
    // cout<<"\"empty\""<<endl;
}

seq_astnode::seq_astnode(std::vector<statement_astnode*> vectorStatements){
    this->vectorStatements = vectorStatements;
    this->astnode_type =typeExp::seq;
}

void seq_astnode::print(int flag){
    for(auto stmt : this->vectorStatements){
        stmt->print(flag);
        for(auto elem: stmt->next){
            code[curr_fun].push_back(elem+":\n");
        }
    }
}
 
assignS_astnode::assignS_astnode(assignE_astnode* assignE){
    this->assignE = assignE;
    this->astnode_type = typeExp::assignS;
}

void assignS_astnode::print(int flag){
    this->assignE->print(0);
}

return_astnode::return_astnode(exp_astnode* returnNode){
    this->returnNode = returnNode;
    this->astnode_type = typeExp::return_ast;
}

void return_astnode::print(int flag){

    if (this->returnNode->base_type.size()>=6 && this->returnNode->rem_type==""){
        this->returnNode->print(1);
        code[curr_fun].push_back("\tpopl\t%edx\n");
        // get the top of return value
        int size_of_params = 8;
        for(auto elem: gst.Entries[curr_fun].Entries){
            if (elem.second.scope=="param"){
                size_of_params += elem.second.size;
            }
        }
        code[curr_fun].push_back("\tleal\t"+std::to_string(size_of_params)+"(%ebp), %eax\n");
        
        int size_struct = gst.Entries[this->returnNode->base_type].size;
        int no_of_iters = size_struct/4;
        for(int i=0;i<no_of_iters;i++){
            code[curr_fun].push_back("\tmovl\t(%edx), %ebx\n");
            code[curr_fun].push_back("\tmovl\t%ebx, (%eax)\n");
            code[curr_fun].push_back("\taddl\t$4, %edx\n");
            code[curr_fun].push_back("\taddl\t$4, %eax\n");
        }
        code[curr_fun].push_back("\tsubl\t$"+std::to_string(size_struct)+", %eax\n");

        code[curr_fun].push_back("\tleave\n");
        code[curr_fun].push_back("\tret\n");
    }

    else{
    
        this->returnNode->print(2);
        code[curr_fun].push_back("\tpopl\t%eax\n");
        code[curr_fun].push_back("\tleave\n");
        code[curr_fun].push_back("\tret\n");
    }
}

if_astnode::if_astnode(exp_astnode* conditionNode,statement_astnode* ifNode,statement_astnode* elseNode){
    this->conditionNode = conditionNode;
    this->ifNode = ifNode;
    this->elseNode = elseNode;
    this->astnode_type = typeExp::if_ast;
}

void if_astnode::print(int flag){
    this->next.push_back(".L"+std::to_string(label_counter));
    label_counter++;
    this->conditionNode->print(flag);
    if (this->conditionNode->astnode_type==typeExp::identifier){
        this->conditionNode->true_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
        this->conditionNode->false_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
    
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tcmp\t$0, %edx\n");
        code[curr_fun].push_back("\tjne\t"+this->conditionNode->true_list[0]+"\n");
        code[curr_fun].push_back("\tjmp\t"+this->conditionNode->false_list[0]+"\n");
    }
    for(auto elem: this->conditionNode->true_list){
        code[curr_fun].push_back(elem+":\n");
    }
    this->ifNode->print(flag);
    for(auto elem: this->ifNode->next){
        code[curr_fun].push_back(elem+":\n");
    }
    
    code[curr_fun].push_back("\tjmp\t"+this->next[0]+"\n");
    for(auto elem: this->conditionNode->false_list){
        code[curr_fun].push_back(elem+":\n");
    }
    this->elseNode->print(flag);
    for(auto elem: this->elseNode->next){
        code[curr_fun].push_back(elem+":\n");
    }

}

while_astnode::while_astnode(exp_astnode* conditionNode,statement_astnode* whileNode){
    this->conditionNode = conditionNode;
    this->whileNode = whileNode;
    this->astnode_type = typeExp::while_ast;
}

void while_astnode::print(int flag){
    std::string condition_label = ".L"+std::to_string(label_counter);
    label_counter++;
    code[curr_fun].push_back(condition_label+":\n");
    this->conditionNode->print(flag);
    if (this->conditionNode->astnode_type==typeExp::identifier){
        this->conditionNode->true_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
        this->conditionNode->false_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
    
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tcmp\t$0, %edx\n");
        code[curr_fun].push_back("\tjne\t"+this->conditionNode->true_list[0]+"\n");
        code[curr_fun].push_back("\tjmp\t"+this->conditionNode->false_list[0]+"\n");
    }
    for(auto elem: this->conditionNode->true_list){
        code[curr_fun].push_back(elem+":\n");
    }

    this->whileNode->print(flag);
    for(auto elem: this->whileNode->next){
        code[curr_fun].push_back(elem+":\n");
    }
    code[curr_fun].push_back("\tjmp\t"+condition_label+"\n");
    this->next = this->conditionNode->false_list;    
}

for_astnode::for_astnode(assignE_astnode* initNode,exp_astnode* conditionNode,assignE_astnode* stepNode,statement_astnode* forNode){
    this->initNode = initNode;
    this->conditionNode = conditionNode;
    this->stepNode = stepNode;
    this->forNode = forNode;
    this->astnode_type = typeExp::for_ast;
}

void for_astnode::print(int flag){
    this->initNode->print(flag);
    
    std::string condition_label = ".L"+std::to_string(label_counter);
    label_counter++;
    code[curr_fun].push_back(condition_label+":\n");
    this->conditionNode->print(flag);
    if (this->conditionNode->astnode_type==typeExp::identifier){
        this->conditionNode->true_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
        this->conditionNode->false_list.push_back(".L"+std::to_string(label_counter));
        label_counter++;
    
        code[curr_fun].push_back("\tpopl\t%edx\n");
        code[curr_fun].push_back("\tcmp\t$0, %edx\n");
        code[curr_fun].push_back("\tjne\t"+this->conditionNode->true_list[0]+"\n");
        code[curr_fun].push_back("\tjmp\t"+this->conditionNode->false_list[0]+"\n");
    }
    for(auto elem: this->conditionNode->true_list){
        code[curr_fun].push_back(elem+":\n");
    }

    this->forNode->print(flag);
    for(auto elem: this->forNode->next){
        code[curr_fun].push_back(elem+":\n");
    }
    this->stepNode->print(flag);
    code[curr_fun].push_back("\tjmp\t"+condition_label+"\n");
    this->next = this->conditionNode->false_list;
}


proccall_astnode::proccall_astnode(std::string identifier,vector<exp_astnode*> seq){
    this->identifier = identifier;
    this->seq = seq;
    this->astnode_type = typeExp::procall;
}

void proccall_astnode::print(int flag){
    if (this->identifier=="printf"){
        for(int i=this->seq.size()-1;i>=0;i--){
            this->seq[i]->print(2);
        }
        code[curr_fun].push_back("\tpushl\t$.L"+std::to_string(label_counter)+"\n");
        code[curr_fun].insert(code[curr_fun].begin(),"\t.string\t"+global_literal+"\n");
        code[curr_fun].insert(code[curr_fun].begin(),".L"+std::to_string(label_counter)+":\n");
        label_counter++;
    }
    else{
        if (gst.Entries[this->identifier].base_type.size()>=6){
            int size_of_return = gst.Entries[this->identifier].size;
            // for the return value of only struct
            code[curr_fun].push_back("\tsubl\t$"+std::to_string(size_of_return+4)+", %esp\n");
        }
        for(auto elem: this->seq){
            
            if (elem->base_type.size()>=6 && elem->base_type.substr(0,6)=="struct" && elem->rem_type==""){
                // struct and not a pointer or array
                elem->print(1);
                int size_struct = gst.Entries[elem->base_type].size;
                int no_of_iters = size_struct/4;
                code[curr_fun].push_back("\tpopl\t%eax\n");
                code[curr_fun].push_back("\taddl\t$"+std::to_string(size_struct-4)+", %eax\n");
            }
            else if (elem->rem_type!="" && !elem->is_array){
                elem->print(0); // if the element is pointer then pass the value
            }
            else if (elem->is_array){
                elem->print(1); //there shouldnt be a need to check is_array here because even for pointers we
            }
            else{
                elem->print(2);
            }
        }
    }
    

    code[curr_fun].push_back("\tcall\t"+this->identifier+"\n");
    if (this->seq.size()!=0)
    {
        int size_params = 4*this->seq.size();
        code[curr_fun].push_back("\taddl\t$"+std::to_string(size_params)+", %esp\n");
    }
    if (gst.Entries[this->identifier].base_type.size()>=6){
        int size_of_return = gst.Entries[this->identifier].size;
        // for the return value of only struct
        code[curr_fun].push_back("\taddl\t$"+std::to_string(size_of_return+4)+", %esp\n");
    }
}   






