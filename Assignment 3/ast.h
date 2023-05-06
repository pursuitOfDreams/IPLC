#ifndef AST_H
#define AST_H


#include<iostream>
#include<string>
#include<vector>
using namespace std;

enum typeExp {
    op_binary,
    op_unary,
    assignE,
    funcall,
    intconst,
    floatconst,
    stringconst,
    identifier,
    arrayref,
    member,
    arrow,
    empty,
    seq,
    assignS,
    return_ast,
    if_ast,
    while_ast,
    for_ast,
    procall
};

class abstract_astnode{
    public:
        virtual void print(int blanks)=0;
        typeExp astnode_type;
        std::string base_type;
        std::string actual_type;
        std::string compatible_type;
        std::string rem_type;
        bool is_array;
};

/* child classes of abstract astnode**/
class statement_astnode: public abstract_astnode{   
    public:
        std::vector<std::string> next;
};

class exp_astnode : public abstract_astnode{
    public:
        int offset = -1;
        bool is_param = false;
        std::vector<std::string> true_list;
        std::vector<std::string> false_list;
        exp_astnode();
};

class ref_astnode : public exp_astnode {
};

class op_binary_astnode : public exp_astnode {
    public:
        std::string binaryOp;
        exp_astnode* leftNode;
        exp_astnode* rightNode;
        op_binary_astnode(std::string binaryOp,exp_astnode* leftNode,exp_astnode* rightNode);
        void print(int blanks);
};

class op_unary_astnode : public exp_astnode {
    public:
        std::string unaryOp;
        exp_astnode* expression;
        op_unary_astnode(std::string unaryOp,exp_astnode* expression);
        void print(int blanks);
};

class assignE_astnode : public exp_astnode {
    public:
        exp_astnode* leftNode;
        exp_astnode* rightNode;
        assignE_astnode(exp_astnode* leftNode,exp_astnode* rightNode);
        void print(int blanks);
};

class funcall_astnode : public exp_astnode {
    public:
        std::string identifier;
        vector<exp_astnode*> seq;
        funcall_astnode(std::string identifier,vector<exp_astnode*> seq);
        void print(int blanks);
};

class intconst_astnode : public exp_astnode {
    public:
        int term;
        intconst_astnode(int term);
        void print(int blanks);
};

class floatconst_astnode : public exp_astnode {
    public:
        float term;
        floatconst_astnode(float term);
        void print(int blanks);
};

class stringconst_astnode : public exp_astnode {
    public:
        std::string term;
        stringconst_astnode(std::string term);
        void print(int blanks);
};

/* child classes of ref_astnode**/
class identifier_astnode : public ref_astnode {
    public:
        std::string identifier;
        identifier_astnode(std::string indentifier, int offset, std::string rem_type);       
        void print(int blanks);
};

class arrayref_astnode : public ref_astnode {
    public:
        exp_astnode* array;
        exp_astnode* index;
        arrayref_astnode(exp_astnode* array,exp_astnode* index);
        void print(int blanks);
};

class member_astnode : public ref_astnode {
    public:
        exp_astnode* expression;
        std::string identifier;
        member_astnode(exp_astnode* expression,std::string indentifier);
        void print(int blanks);
};

class arrow_astnode : public ref_astnode {
    public:
        exp_astnode* expression;
        std::string identifier;
        arrow_astnode(exp_astnode* expression,std::string identifier); 
        void print(int blanks);

};

/* child classes of statement ast node */
class empty_astnode : public statement_astnode {
    public:
        empty_astnode();
        void print(int blanks);
};

class seq_astnode : public statement_astnode {
    public:
        std::vector<statement_astnode*> vectorStatements;
        seq_astnode(std::vector<statement_astnode*> vectorStatements);
        void print(int blanks);
};

class assignS_astnode : public statement_astnode {
    public:
        assignE_astnode* assignE;
        assignS_astnode(assignE_astnode* assignE);
        void print(int blanks);
};

class return_astnode : public statement_astnode {
    public:
        exp_astnode* returnNode;
        return_astnode(exp_astnode* returnNode);
        void print(int blanks);
};

class if_astnode : public statement_astnode {
    public:
        exp_astnode* conditionNode;
        statement_astnode* ifNode;
        statement_astnode* elseNode;
        if_astnode(exp_astnode* conditionNode,statement_astnode* ifNode,statement_astnode* elseNode);
        void print(int blanks);

};

class while_astnode : public statement_astnode {
    public:
        exp_astnode* conditionNode;
        statement_astnode* whileNode;
        while_astnode(exp_astnode* conditionNode,statement_astnode* whileNode);
        void print(int blanks);

};

class for_astnode : public statement_astnode {
    public:
        assignE_astnode* initNode;
        exp_astnode* conditionNode;
        assignE_astnode* stepNode;
        statement_astnode* forNode;
        for_astnode(assignE_astnode* initNode,exp_astnode* conditionNode,assignE_astnode* stepNode,statement_astnode* forNode);
        void print(int blanks);
};

class proccall_astnode : public statement_astnode {
    public:
        std::string identifier;
        vector<exp_astnode*> seq;
        proccall_astnode(std::string identifier,vector<exp_astnode*> seq);
        void print(int blanks);
};

#endif