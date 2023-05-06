#include "ast.h"
using namespace std;
exp_astnode::exp_astnode(){
    this->is_array = false;
}
op_binary_astnode::op_binary_astnode(std::string binaryOp,exp_astnode* leftNode,exp_astnode* rightNode){
    this->binaryOp = binaryOp;
    this->leftNode = leftNode;
    this->rightNode = rightNode;
    
    //this->astnode_type = op_binary;
}

void op_binary_astnode::print(int blanks){
    cout<<"\"op_binary\": {"<<endl;
    cout<<"\"op\": \""<<binaryOp<<"\","<<endl;
    cout<<"\"left\": {"<<endl;
    this->leftNode->print(0);
    cout<<"},"<<endl;
    cout<<"\"right\": {"<<endl;
    this->rightNode->print(0);
    cout<<"}"<<endl;
    cout<<"}"<<endl;
}

op_unary_astnode::op_unary_astnode(std::string unaryOp,exp_astnode* expression){
    this->unaryOp = unaryOp;
    this->expression = expression;
}

void op_unary_astnode::print(int blanks){
    cout<<"\"op_unary\": {"<<endl;
    cout<<"\"op\": \""<<this->unaryOp<<"\","<<endl;
    cout<<"\"child\": {"<<endl;
    this->expression->print(0);
    cout<<"}"<<endl;
    cout<<"}"<<endl;
}

assignE_astnode::assignE_astnode(exp_astnode* leftNode,exp_astnode* rightNode){
    this->leftNode = leftNode;
    this->rightNode = rightNode;
}

void assignE_astnode::print(int blanks){
    if(!blanks){
        cout<<"\"assignE\": {"<<endl;
        cout<<"\"left\": {"<<endl;
        this->leftNode->print(0);
        cout<<"},"<<endl;
        cout<<"\"right\": {"<<endl;
        this->rightNode->print(0);
        cout<<"}"<<endl;
        cout<<"}"<<endl;
    }
    else{
        cout<<"\"assignS\": {"<<endl;
        cout<<"\"left\": {"<<endl;
        this->leftNode->print(0);
        cout<<"},"<<endl;
        cout<<"\"right\": {"<<endl;
        this->rightNode->print(0);
        cout<<"}"<<endl;
        cout<<"}"<<endl;
    }
}

funcall_astnode::funcall_astnode(std::string identifier,vector<exp_astnode*> seq){
    this->seq = seq;
    this->identifier = identifier;
}

void funcall_astnode::print(int blanks){
    cout<<"\"funcall\": {"<<endl;
    cout<<"\"fname\": {"<<endl;
    cout<<"\"identifier\": \""<<this->identifier<<"\""<<endl;
    cout<<"},"<<endl;
    cout<<"\"params\": ["<<endl;
    int count =0;
    for (auto elem : seq){
        cout<<"{"<<endl;
        elem->print(0);
        if (count==seq.size()-1){
            cout<<"}"<<endl;
        }
        else
            cout<<"},"<<endl;
        count++;
    }
    cout<<"]"<<endl;
    cout<<"}"<<endl;
}

intconst_astnode::intconst_astnode(int term){
    this->term = term;
    this->astnode_type = typeExp::intconst;
}

void intconst_astnode::print(int blanks){
    cout<<"\"intconst\": "<<this->term<<endl;
}

floatconst_astnode::floatconst_astnode(float term){
    this->term = term;
}

void floatconst_astnode::print(int blanks){
    cout<<"\"floatconst\": "<<this->term<<endl;
}

stringconst_astnode::stringconst_astnode(std::string term){
    this->term = term;
}

void stringconst_astnode::print(int blanks){
    cout<<"\"stringconst\": "<<this->term<<endl;
}

identifier_astnode::identifier_astnode(std::string identifier){
    this->identifier = identifier;
    this->astnode_type = typeExp::identifier;
}

void identifier_astnode::print(int blanks){
    cout<<"\"identifier\": \""<<this->identifier<<"\""<<endl;
}

arrayref_astnode::arrayref_astnode(exp_astnode* array,exp_astnode* index){
    this->array = array;
    this->index = index;
    this->astnode_type = typeExp::arrayref;
}

void arrayref_astnode::print(int blanks){
    cout<<"\"arrayref\": {"<<endl;
    cout<<"\"array\": {"<<endl;
    this->array->print(0);
    cout<<"},"<<endl;
    cout<<"\"index\": {"<<endl;
    this->index->print(0);
    cout<<"}"<<endl;
    cout<<"}"<<endl;
}

member_astnode::member_astnode(exp_astnode* expression,std::string identifier){
    this->expression = expression;
    this->identifier = identifier;
    this->astnode_type = typeExp::member;
}

//need to check below function again
void member_astnode::print(int blanks){ //may be wrong as not part of examples
    cout<<"\"member\": {"<<endl;
    cout<<"\"struct\": {"<<endl; //what to put here
    this->expression->print(0);
    cout<<"},"<<endl;
    cout<<"\"field\": {"<<endl;
    cout<<"\"identifier\": \""<<this->identifier<<"\""<<endl;
    cout<<"}"<<endl;
    cout<<"}"<<endl;
}

arrow_astnode::arrow_astnode(exp_astnode* expression,std::string identifier){
    this->expression = expression;
    this->identifier = identifier;
    this->astnode_type = typeExp::arrow;
}

void arrow_astnode::print(int blanks){
    cout<<"\"arrow\": {"<<endl;
    cout<<"\"pointer\": {"<<endl;
    this->expression->print(0);
    cout<<"},"<<endl;
    cout<<"\"field\": {"<<endl;
    cout<<"\"identifier\": \""<<this->identifier<<"\""<<endl;
    cout<<"}"<<endl;
    cout<<"}"<<endl;
}

empty_astnode::empty_astnode(){this->astnode_type = empty;}

void empty_astnode::print(int blanks){
    cout<<"\"empty\""<<endl;
}

seq_astnode::seq_astnode(std::vector<statement_astnode*> vectorStatements){
    this->vectorStatements = vectorStatements;
}

void seq_astnode::print(int blanks){
    cout<<"\"seq\": ["<<endl;
    int cnt =0;
    for(auto stmt : this->vectorStatements){
        if (stmt->astnode_type==empty){
            stmt->print(0);
            if (cnt==this->vectorStatements.size()-1)
                cout<<""<<endl;
            else
                cout<<","<<endl; //need to handle last element separately here
        }
        else{
            cout<<"{"<<endl;
            stmt->print(0);
            if (cnt==this->vectorStatements.size()-1)
                cout<<"}"<<endl;
            else
                cout<<"},"<<endl; //need to handle last element separately here
        }
        cnt++;
    }
    cout<<"]"<<endl;
}
 
assignS_astnode::assignS_astnode(assignE_astnode* assignE){
    this->assignE = assignE;
}

void assignS_astnode::print(int blanks){
    this->assignE->print(1);
}

return_astnode::return_astnode(exp_astnode* returnNode){
    this->returnNode = returnNode;
}

void return_astnode::print(int blanks){
    cout<<"\"return\": {"<<endl;
    this->returnNode->print(0);
    cout<<"}"<<endl;
}

if_astnode::if_astnode(exp_astnode* conditionNode,statement_astnode* ifNode,statement_astnode* elseNode){
    this->conditionNode = conditionNode;
    this->ifNode = ifNode;
    this->elseNode = elseNode;
}

void if_astnode::print(int blanks){
    cout<<"\"if\": {"<<endl;
    cout<<"\"cond\": {"<<endl;
    this->conditionNode->print(0);
    cout<<"},"<<endl;
    if (this->ifNode->astnode_type == typeExp::empty){
        cout<<"\"then\": "<<endl;
        this->ifNode->print(0);
        cout<<","<<endl;
    }
    else{
        cout<<"\"then\": {"<<endl;
        this->ifNode->print(0);
        cout<<"},"<<endl;
    }
    if (this->elseNode->astnode_type == typeExp::empty){
        cout<<"\"else\": "<<endl;
        this->elseNode->print(0);
    }
    else{
        cout<<"\"else\": {"<<endl;
        this->elseNode->print(0);
        cout<<"}"<<endl;
    }
    cout<<"}"<<endl;
    
}

while_astnode::while_astnode(exp_astnode* conditionNode,statement_astnode* whileNode){
    this->conditionNode = conditionNode;
    this->whileNode = whileNode;
}

void while_astnode::print(int blanks){
    cout<<"\"while\": {"<<endl;
    cout<<"\"cond\": {"<<endl;
    this->conditionNode->print(0);
    cout<<"},"<<endl;
    if (this->whileNode->astnode_type == typeExp::empty){
        cout<<"\"stmt\": "<<endl;
        this->whileNode->print(0);
    }
    else{
        cout<<"\"stmt\": {"<<endl;
        this->whileNode->print(0);
        cout<<"}"<<endl;
    }
    cout<<"}"<<endl;
}

for_astnode::for_astnode(assignE_astnode* initNode,exp_astnode* conditionNode,assignE_astnode* stepNode,statement_astnode* forNode){
    this->initNode = initNode;
    this->conditionNode = conditionNode;
    this->stepNode = stepNode;
    this->forNode = forNode;
}

void for_astnode::print(int blanks){
    cout<<"\"for\": {"<<endl;
    cout<<"\"init\": {"<<endl;
    this->initNode->print(0);
    cout<<"},"<<endl;
    cout<<"\"guard\": {"<<endl;
    this->conditionNode->print(0);
    cout<<"},"<<endl;
    cout<<"\"step\": {"<<endl;
    this->stepNode->print(0);
    cout<<"},"<<endl;
    if (this->forNode->astnode_type == typeExp::empty){
        cout<<"\"body\": "<<endl;
        this->forNode->print(0);
    }
    else{
        cout<<"\"body\": {"<<endl;
        this->forNode->print(0);
        cout<<"}"<<endl;
    }
    cout<<"}"<<endl;
}


proccall_astnode::proccall_astnode(std::string identifier,vector<exp_astnode*> seq){
    this->identifier = identifier;
    this->seq = seq;
}

void proccall_astnode::print(int blanks){
    cout<<"\"proccall\": {"<<endl;
    cout<<"\"fname\": {"<<endl;
    cout<<"\"identifier\": \""<<identifier<<"\""<<endl;
    cout<<"},"<<endl;
    cout<<"\"params\": ["<<endl;
    int count =0;
    for (auto elem : seq){
        cout<<"{"<<endl;
        elem->print(0);
        if (count==seq.size()-1){
            cout<<"}"<<endl;
        }
        else
            cout<<"},"<<endl;
        count++;
    }
    cout<<"]"<<endl;
    cout<<"}"<<endl;
}







