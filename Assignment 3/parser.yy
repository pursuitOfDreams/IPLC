%skeleton "lalr1.cc"
%require  "3.0.1"

%defines 
%define api.namespace {IPL}
%define api.parser.class {Parser}

%define parse.trace

%code requires{
   namespace IPL {
      class Scanner;
   }

   #include "ast.h"
   #include "symbolTab.h"
   #include<vector>

  // # ifndef YY_NULLPTR
  // #  if defined __cplusplus && 201103L <= __cplusplus
  // #   define YY_NULLPTR nullptr
  // #  else
  // #   define YY_NULLPTR 0
  // #  endif
  // # endif
   

}

%printer { std::cerr << $$; } INT
%printer { std::cerr << $$; } FLOAT
%printer { std::cerr << $$; } VOID
%printer { std::cerr << $$; } STRUCT
%printer { std::cerr << $$; } INT_CONSTANT
%printer { std::cerr << $$; } FLOAT_CONSTANT
%printer { std::cerr << $$; } WHILE
%printer { std::cerr << $$; } FOR
%printer { std::cerr << $$; } IF
%printer { std::cerr << $$; } ELSE
%printer { std::cerr << $$; } RETURN
%printer { std::cerr << $$; } AND_OP
%printer { std::cerr << $$; } OR_OP
%printer { std::cerr << $$; } NE_OP
%printer { std::cerr << $$; } EQ_OP
%printer { std::cerr << $$; } INC_OP
%printer { std::cerr << $$; } PTR_OP
%printer { std::cerr << $$; } STRING_LITERAL
%printer { std::cerr << $$; } LE_OP
%printer { std::cerr << $$; } GE_OP



%parse-param { Scanner  &scanner  }
%locations
%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <string>
   #include <cstring>
   #include <map>
   #include <queue>
   #include <stack>
   #include <algorithm>
   #include "scanner.hh"
   int nodeCount = 0;
   int size = 1;
   int line_num = 1;
   int intconst_val = 0;
   std::stack<string> paramStack;
   std::queue<string> localQueue;
   std::string gFunRt;
   std::string currFun = "";
   SymbTab *lst = new SymbTab(); // local symbol table of the current function
   extern SymbTab gst;
   std::map<std::string,abstract_astnode*> ast;
   std::map<std::string,std::vector<std::string>> code;
   std::vector<std::string> function_order;
   std::vector<std::string> decList;
   bool isPrintf = true;
   int printf_start_idx = -1;
   std::vector<std::vector<std::string>> printf_param_code;
   int intcont = 0;

#undef yylex
#define yylex IPL::Parser::scanner.yylex

}




%define api.value.type variant
%define parse.assert



%token '\n'
%token <std::string> INT
%token <std::string> FLOAT
%token <std::string> VOID
%token <std::string> STRUCT
%token <std::string> INT_CONSTANT
%token <std::string> FLOAT_CONSTANT
%token <std::string> WHILE
%token <std::string> FOR
%token <std::string> IF
%token <std::string> ELSE
%token <std::string> RETURN
%token <std::string> AND_OP
%token <std::string> OR_OP
%token <std::string> NE_OP
%token <std::string> EQ_OP
%token <std::string> INC_OP
%token <std::string> PTR_OP
%token <std::string> STRING_LITERAL
%token <std::string> LE_OP
%token <std::string> GE_OP
%token <std::string> PRINTF
%token <std::string> IDENTIFIER
%token <std::string> OTHERS
%token '+'
%token '-'
%token '/'
%token '*'
%token '('
%token ')'
%token '{'
%token '}'
%token '['
%token ']'
%token ';'
%token ':'
%token '='
%token '&'
%token '.'
%token '>'
%token '<'
%token '!'

// %nterm <int> translation_unit struct_specifier function_definition type_specifier fun_declarator parameter_list parameter_declaration declarator_arr declarator compound_statement statement_list statement assignment_expression assignment_statement procedure_call expression logical_and_expression equality_expression relational_expression additive_expression unary_expression multiplicative_expression postfix_expression primary_expression expression_list unary_operator selection_statement iteration_statement declaration_list declaration declarator_list
%nterm <std::string> type_specifier
%nterm <Symbol*> parameter_declaration
%nterm <Symbol*> declarator
%nterm <Symbol*> declarator_arr

%nterm<abstract_astnode*> function_definition
%nterm<seq_astnode*> statement_list
%nterm<statement_astnode*> statement selection_statement iteration_statement compound_statement
%nterm<assignE_astnode*> assignment_expression
%nterm<assignS_astnode*> assignment_statement
%nterm<proccall_astnode*> procedure_call printf_call
%nterm<exp_astnode*> expression logical_and_expression equality_expression relational_expression additive_expression unary_expression multiplicative_expression primary_expression postfix_expression
//%nterm<ref_astnode*>  postfix_expression
%nterm<std::string> unary_operator
%nterm<std::vector<exp_astnode*>> expression_list
%%

    translation_unit: 
                    struct_specifier
                    | function_definition
                    | translation_unit struct_specifier
                    | translation_unit function_definition

     struct_specifier: STRUCT IDENTIFIER '{' declaration_list '}' ';'
                     {
                        int offset = 0;
                        // for all the local variables
                        while(!localQueue.empty()){
                           std::string symbolName = localQueue.front();
                           lst->Entries[symbolName].offset = offset;
                           offset += lst->Entries[symbolName].size;
                           localQueue.pop();
                        }
                        currFun = "struct "+$2;
                        gst.Entries[currFun] = Symbol();
                        gst.Entries[currFun].symbolName = "struct "+$2;
                        gst.Entries[currFun].varfun = "struct";
                        gst.Entries[currFun].scope = "global";
                        gst.Entries[currFun].base_type = "struct "+$2;
                        gst.Entries[currFun].rem_type = "";
                        gst.Entries[currFun].returnType = "-";
                        gst.Entries[currFun].offset =0;
                        gst.Entries[currFun].Entries = lst->Entries ;
                        gst.Entries[currFun].size=0;
                        for(auto i: lst->Entries){
                           if (i.second.scope=="local"){
                              gst.Entries[currFun].size += i.second.size;
                           }
                        }
                        lst = new SymbTab();
                     }

     function_definition: type_specifier
                           {
                              gFunRt = $1;
                           }
                           fun_declarator compound_statement
                           {  
                              ast[currFun] = $4; 
                              
                              $$ = $4;
                              lst = new SymbTab();
                              function_order.push_back(currFun);
                           }

     type_specifier: VOID { $$ = "void"; size=4; }
                   | INT { $$ = "int"; size=4; }
                   | FLOAT { $$ = "float"; size =4 ;}
                   | STRUCT IDENTIFIER { $$ = "struct " + $2; size = gst.Entries["struct "+$2].size;}

    fun_declarator: IDENTIFIER '(' parameter_list ')'
                    {
                       currFun = $1;
                       int offset = 8; //have made the change here
                       while(!paramStack.empty()){
                          std::string symbolName = paramStack.top();
                          lst->Entries[symbolName].offset = offset;
                          offset = offset + lst->Entries[symbolName].size;
                          paramStack.pop();
                       }
                        
                        gst.Entries[currFun]  = Symbol();
                        gst.Entries[currFun].symbolName = currFun;
                        gst.Entries[currFun].varfun = "fun";
                        gst.Entries[currFun].scope = "global";
                        gst.Entries[currFun].base_type = gFunRt;
                        gst.Entries[currFun].returnType = gFunRt;
                        gst.Entries[currFun].size =0 ;
                        gst.Entries[currFun].offset =0 ;


                    }
                  | IDENTIFIER '(' ')'
                    {
                        currFun = $1;
                        gst.Entries[currFun]  = Symbol();
                        gst.Entries[currFun].symbolName = currFun;
                        gst.Entries[currFun].varfun = "fun";
                        gst.Entries[currFun].scope = "global";
                        gst.Entries[currFun].base_type = gFunRt;
                        gst.Entries[currFun].returnType = gFunRt;
                        gst.Entries[currFun].size =0 ;
                        gst.Entries[currFun].offset =0 ;
                    }

    parameter_list: parameter_declaration
                     {
                        if (lst->Entries.find($1->symbolName)!=lst->Entries.end())
                        {
                           cout<<"Error at line "<<line_num<<": "<<$1->symbolName<<" has a previous declaration"<<endl;
                           exit(1);
                        }
                        lst->Entries[$1->symbolName] = *$1;
                     }
                     | 
                     parameter_list ',' parameter_declaration
                     {
                        if (lst->Entries.find($3->symbolName)!=lst->Entries.end())
                        {
                           cout<<"Error at line "<<line_num<<": "<<$3->symbolName<<" has a previous declaration"<<endl;
                           exit(1);
                        }
                        lst->Entries[$3->symbolName] = *$3;
                     }

    parameter_declaration: type_specifier declarator 
                           {
                              if ($1=="void" && $2->returnType == ""){
                                 cout<<"Error at line "<<line_num<<": Cannot declare the type of a parameter as \"void\""<<endl;
                                 exit(1);
                              }
                              $$ = $2;
                              if ($2->returnType[0]=='*')
                                 $$->size *= 4;
                              else $$->size *= size;
                              $$->base_type = $1;
                              $$->rem_type = $2->returnType;
                              $$->returnType = $1 + $2->returnType;
                              $$->varfun = "var";
                              $$->scope = "param";
                              $$->offset=0;
                              paramStack.push($$->symbolName);
                           }

    declarator_arr: IDENTIFIER 
                    { 
                       $$ = new Symbol();
                       $$->symbolName = $1;
                       $$->size = 1;
                       $$->varfun = "var";
                     }
                  | declarator_arr '[' INT_CONSTANT ']'
                   {
                     $$ = $1;
                     // cout<<$$->size<<endl;
                     $$->size *= std::stoi($3);
                     $$->isarray = true;
                     $$->returnType += "[" + $3 + "]";
                   }

    declarator: declarator_arr 
               { 
                  $$ = $1;
               }
              | '*' declarator 
              { 
                 $$ = $2;
                 $$->returnType = "*" + $2->returnType;
               }

    compound_statement: '{' '}' 
                        {
                           gst.Entries[currFun].Entries = lst->Entries ;
                           $$ = new seq_astnode(std::vector<statement_astnode*>());
                        }
                      | '{' statement_list {gst.Entries[currFun].Entries = lst->Entries ;} '}' 
                      {
                         $$ = $2;
                      }
                      | '{' declaration_list '}' 
                      {
                        int offset = 0;
                        // for all the local variables
                        while(!localQueue.empty()){
                           std::string symbolName = localQueue.front();
                           offset -= lst->Entries[symbolName].size;
                           lst->Entries[symbolName].offset = offset;
                           localQueue.pop();
                        }
                        gst.Entries[currFun].Entries = lst->Entries ;
                        $$ = new seq_astnode(std::vector<statement_astnode*>()); 
                      }
                      | '{' declaration_list 
                      {
                        int offset = 0;
                        // for all the local variables
                        while(!localQueue.empty()){
                           std::string symbolName = localQueue.front();
                           offset -= lst->Entries[symbolName].size;
                           lst->Entries[symbolName].offset = offset;
                           localQueue.pop();
                        }
                         gst.Entries[currFun].Entries = lst->Entries ;
                      }
                      statement_list '}' {$$ = $4;}

    statement_list: statement {$$ = new seq_astnode(std::vector<statement_astnode*>({$1}));}
                  | statement_list statement {$1->vectorStatements.push_back($2);$$ = new seq_astnode($1->vectorStatements);} 

    statement: ';' {$$ = new empty_astnode();}
             | '{' statement_list '}' {$$ = $2;}
             | selection_statement {$$ = $1;}
             | iteration_statement {$$ = $1;}
             | assignment_statement {$$ = $1;}
             | procedure_call {$$ = $1;}
             | printf_call {$$ = $1;}
             | RETURN expression ';' 
             {
                $$ = new return_astnode($2);
             }

    assignment_expression: unary_expression '=' expression
                       {
                           $$ = new assignE_astnode($1,$3);
                        }


    assignment_statement: assignment_expression ';' {$$  = new assignS_astnode($1);}

    procedure_call: IDENTIFIER '(' ')' ';' 
                  {
                     $$ = new proccall_astnode($1,vector<exp_astnode*>());
                  }
                  | IDENTIFIER '(' expression_list ')' ';' 
                  {
                     $$ = new proccall_astnode($1,$3);
                  }

   printf_call: PRINTF '(' STRING_LITERAL ')' ';'
               {
                  $$ = new proccall_astnode($1,vector<exp_astnode*>({new stringconst_astnode($3)}));
               }

               | PRINTF '(' STRING_LITERAL ',' expression_list ')' ';' 
               {
                  vector<exp_astnode*> vec = $5;
                  vec.insert(vec.begin(),new stringconst_astnode($3));
                  $$ = new proccall_astnode($1,vec);
               }

    expression: logical_and_expression 
               {
                  $$ = $1;
               }
              | expression OR_OP logical_and_expression 
               {
                  $$ = new op_binary_astnode(std::string("OR_OP"),$1,$3);
                  $$->base_type = "int";
                  $$->actual_type = "int";
               }

    logical_and_expression: equality_expression 
                           {
                              $$ = $1;
                           }
                          | logical_and_expression AND_OP equality_expression 
                          {
                              $$ = new op_binary_astnode(std::string("AND_OP"),$1,$3);
                              $$->base_type = "int";
                              $$->actual_type = "int";
                              $$->rem_type = "";
                          }
   
    equality_expression: relational_expression {$$ = $1;}
                       | equality_expression EQ_OP relational_expression
                       {
                           $$ = new op_binary_astnode(std::string("EQ_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                        }

                       | equality_expression NE_OP relational_expression 
                       {
                           $$ = new op_binary_astnode(std::string("NE_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                        }

    relational_expression: additive_expression {$$ = $1;}
                         | relational_expression '<' additive_expression 
                         {
                           $$ = new op_binary_astnode(std::string("LT_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                         }
                         | relational_expression '>' additive_expression 
                         {
                           $$ = new op_binary_astnode(std::string("GT_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                         }
                         | relational_expression LE_OP additive_expression 
                         {
                           $$ = new op_binary_astnode(std::string("LE_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                         }
                         | relational_expression GE_OP additive_expression 
                         {
                           $$ = new op_binary_astnode(std::string("GE_OP"),$1,$3);
                           $$->base_type = "int";
                           $$->actual_type = "int";
                           $$->rem_type = "";
                         }

    additive_expression: multiplicative_expression {$$ = $1;}
                       | additive_expression '+' multiplicative_expression
                       {
                           if($1->actual_type == "int" && $3->actual_type == "int"){
                              $$ = new op_binary_astnode(std::string("PLUS"),$1,$3);
                              $$->base_type = "int";
                              $$->actual_type = "int";
                              $$->rem_type = "";
                           }
                           else if($1->actual_type == "int" && $3->rem_type!=""){
                              $$ = new op_binary_astnode(std::string("PLUS"),$1,$3);
                              $$->base_type = $3->base_type;
                              $$->actual_type = $3->actual_type;
                              $$->rem_type = $3->rem_type;
                              $$->is_array = $3->is_array;

                           }
                           
                           else if($3->actual_type == "int" && $1->rem_type!="" ){
                              $$ = new op_binary_astnode(std::string("PLUS"),$1,$3);
                              $$->base_type = $1->base_type;
                              $$->actual_type = $1->actual_type;
                              $$->rem_type = $1->rem_type;
                              $$->is_array = $1->is_array;
                           }
                           
                           else{
                              
                              cout<<"Error at line "<<line_num<<": "<<"Invalid operand types for binary + , \""<<$1->actual_type<<"\" and \""<<$3->actual_type<<"\""<<endl;
                              exit(1);
                           }


                        } 
                       | additive_expression '-' multiplicative_expression 
                       {
                           if($1->actual_type == "int" && $3->actual_type == "int"){
                              $$ = new op_binary_astnode(std::string("MINUS"),$1,$3);
                              $$->base_type = "int";
                              $$->actual_type = "int";
                              $$->rem_type = "";
                           }
                           else if($1->actual_type == "int" && $3->rem_type != ""){
                              $$ = new op_binary_astnode(std::string("MINUS"),$1,$3);
                              $$->base_type = $3->base_type;
                              $$->actual_type = $3->actual_type;
                              $$->rem_type = $3->rem_type;
                           }
                           else if($3->actual_type == "int" && $1->base_type != ""){
                              $$ = new op_binary_astnode(std::string("MINUS"),$1,$3);
                              $$->base_type = $1->base_type;
                              $$->actual_type = $1->actual_type;
                              $$->rem_type = $1->rem_type;
                           }
                           else{
                              $$ = new op_binary_astnode(std::string("MINUS"),$1,$3);
                              $$->base_type = "int";
                              $$->rem_type = "";
                              $$->actual_type = "int";
                  
                           }

                        }

    unary_expression: postfix_expression {$$ = $1; $$->actual_type = $1->actual_type;$$->rem_type = $1->rem_type;$$->offset = $1->offset;}
                    | unary_operator unary_expression 
                     {
                        if($1 == "DEREF"){
                           if($2->rem_type != ""){
                              if($2->rem_type[0] == '('){
                                 $2->rem_type.erase(0,3);
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->rem_type = $2->rem_type;
                                 $$->base_type = $2->base_type;
                                 $$->actual_type = $$->base_type + $$->rem_type;
                                 $$->is_array = $2->is_array;
                                 bool flag = false;
                                 for(int i=0;i<$2->rem_type.size();i++){
                                    if($2->rem_type[i] == '('){
                                       flag = true;
                                       break;
                                    }
                                 }
                                 if(!flag){
                                    $$->astnode_type = typeExp::identifier; //may not be the best idea
                                 }

                              }
                              else if($2->is_array) {
                                 int flag=0;
                                 int startidx=0, endidx=0;
                                 int cnt=0;
                                 for(int i=0;i<$2->rem_type.size();i++){
                                    if($2->rem_type[i] == '['){
                                       cnt++;
                                       if (flag==0){
                                          startidx = i;
                                          flag = 1;
                                       }
                                    }
                                    else if (flag && $2->rem_type[i] == ']'){
                                       if (flag==1){
                                          flag=-1;
                                          endidx = i;
                                       }
                                    }
                                 }
                                 $2->rem_type.erase(startidx, endidx-startidx+1);
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->base_type = $2->base_type;
                                 $$->rem_type = $2->rem_type;
                                 $$->actual_type = $2->base_type + $2->rem_type;
                                 if (cnt>1){
                                    $$->is_array = true;
                                 }
                                 else{
                                    $$->is_array = false;
                                 }
                                 $$->astnode_type = typeExp::member; //may not be the best idea
                              }
                              else{
                                 $2->rem_type.erase(0,1);
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->base_type = $2->base_type;
                                 $$->rem_type = $2->rem_type;
                                 $$->actual_type = $2->base_type + $2->rem_type;
                                 $$->is_array  = $2->is_array;
                                 $$->astnode_type = typeExp::identifier; //may not be the best idea or be needed
                              }
                           }
                           else{
                              cout<<"Error at line "<<line_num<<": "<<"Invalid operand type \""<<$2->actual_type<<"\" of unary *"<<endl;
                              exit(1);
                           }
                        }
                        else if($1 == "ADDRESS"){
                           if($2->astnode_type == typeExp::identifier||$2->astnode_type == typeExp::arrayref||$2->astnode_type == typeExp::arrow||$2->astnode_type == typeExp::member){
                              $$ = new op_unary_astnode($1,$2);
                              $$->base_type = $2->base_type;
                              if($2->is_array) {
                                 int bracket_start_idx = 0;
                                 for(int i=0;i<$2->rem_type.size();i++){
                                    if($2->rem_type[i] == '['){
                                       bracket_start_idx = i;
                                       break;
                                    }
                                 }
                                 std::string prev_string = $2->rem_type.substr(0,bracket_start_idx);
                                 std::string rem_string = $2->rem_type.substr(bracket_start_idx,$2->rem_type.size()-bracket_start_idx+1);             
                                 $$->rem_type = prev_string + "(*)" + rem_string;
                              }
                              else {
                                 $$->rem_type = "*"+$2->rem_type;
                              }
                              $$->actual_type = $$->base_type + $$->rem_type;
                              $$->is_array = $2->is_array;
                           }
                           else{
                              cout<<"Error at line "<<line_num<<": "<<"Operand of & should  have lvalue"<<endl;
                              exit(1); 
                           }
                        }
                        else{
                           // should also not be struct or any pointer
                           if (($1 =="UMINUS") && ($2->actual_type!="int" && $2->actual_type!="float")){
                              cout<<"Error at line "<<line_num<<": "<<"Operand of unary - should be an int or float"<<endl;
                              exit(1);
                           }
                           if ($1 == "NOT"){
                              if($2->actual_type == "void" && $2->actual_type == "struct"){
                                 cout<<"Error at line "<<line_num<<": "<<"Operand of unary ! should be an int or float or pointer"<<endl;
                                 exit(1);
                              }
                              $$ = new op_unary_astnode($1,$2);
                              $$->base_type = "int";
                              $$->rem_type = "";
                              $$->actual_type = "int";
                           }
                           else{
                              if($1 == "TO_INT"){
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->base_type = "int";
                                 $$->rem_type = "";
                                 $$->actual_type = "int";
                              }
                              else if($1 == "TO_FLOAT"){
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->base_type = "float";
                                 $$->rem_type = "";
                                 $$->actual_type = "float";
                              }
                              else{
                                 $$ = new op_unary_astnode($1,$2);
                                 $$->base_type = $2->base_type;
                                 $$->rem_type = $2->rem_type;
                                 $$->actual_type = $$->base_type + $$->rem_type;
                              }
                           }
                        }
                     }

    multiplicative_expression: unary_expression {$$ = $1;}
                             | multiplicative_expression '*' unary_expression 
                             {
                                 $$ = new op_binary_astnode(std::string("MULT"),$1,$3);
                                 $$->actual_type = "int";

                             }
                             | multiplicative_expression '/' unary_expression
                             {
                                 $$ = new op_binary_astnode(std::string("DIV"),$1,$3);
                                 $$->actual_type = "int";

                             } 

    postfix_expression: primary_expression {$$ = $1; $$->actual_type = $1->actual_type;$$->rem_type = $1->rem_type; $$->offset = $1->offset; }
                      | postfix_expression '[' expression ']' 
                      {
                           
                           if($1->is_array){
                              int flag=0;
                              int startidx=0, endidx=0;
                              int cnt=0;
                              std::string rem_type = $1->rem_type;
                              // cout<<"start "+std::to_string(intconst_val)<<endl;
                              for(int i=0;i<$1->rem_type.size();i++){
                                 if($1->rem_type[i] == '['){
                                    cnt++;
                                    if (flag==0){
                                       startidx = i;
                                       flag =1;
                                    }
                                 }
                                 else if (flag && $1->rem_type[i] == ']'){
                                    if (flag==1){
                                       flag=-1;
                                       endidx = i;
                                       break;
                                    }
                                 }
                              }
                              rem_type.erase(startidx, endidx-startidx+1);
                              // cout<<"end "+rem_type<<endl;
                              $$ = new arrayref_astnode($1,$3);
                              $$->base_type = $1->base_type;
                              $$->rem_type = rem_type;
                              $$->actual_type = $1->base_type + rem_type;
                              $$->is_array = true;
                              if (cnt>1){
                                 $$->is_array = true;
                              }
                              
                              
                           }
                           else{

                              // assuming the code would never be wrong
                              $$ = new arrayref_astnode($1,$3);
                              $$->is_param = true;
                              $$->base_type = $1->base_type;
                              $$->rem_type = $1->rem_type;
                              $$->actual_type = $1->base_type + $1->rem_type;
                           }
                      }
                      | IDENTIFIER '(' ')' 
                      {
                        $$ = new funcall_astnode($1,vector<exp_astnode*>({}));
                        $$->base_type = gst.Entries[$1].base_type;
                        $$->rem_type = gst.Entries[$1].rem_type;
                        $$->actual_type  = gst.Entries[$1].returnType;
                         
                      }
                      | IDENTIFIER '(' expression_list ')' 
                        {
                           $$ = new funcall_astnode($1,$3);
                           $$->base_type = gst.Entries[$1].base_type;
                           $$->rem_type = gst.Entries[$1].rem_type;
                           $$->actual_type  = gst.Entries[$1].returnType;
                        } 
                      | postfix_expression '.' IDENTIFIER 
                        {
                           $$ = new member_astnode($1,$3);
                           $$->base_type = gst.Entries[$1->base_type].Entries[$3].base_type;
                           $$->rem_type = gst.Entries[$1->base_type].Entries[$3].rem_type;
                           $$->actual_type = gst.Entries[$1->base_type].Entries[$3].returnType;
                           $$->is_array = gst.Entries[$1->base_type].Entries[$3].isarray;
                           $$->offset = gst.Entries[$1->base_type].Entries[$3].offset;
         
                        }
                      | postfix_expression PTR_OP IDENTIFIER 
                        {
                           $$ = new arrow_astnode($1,$3);
                           $$->base_type = gst.Entries[$1->base_type].Entries[$3].base_type;
                           $$->rem_type = gst.Entries[$1->base_type].Entries[$3].rem_type;
                           $$->actual_type = gst.Entries[$1->base_type].Entries[$3].returnType;
                           $$->is_array = gst.Entries[$1->base_type].Entries[$3].isarray;
                           $$->offset = gst.Entries[$1->base_type].Entries[$3].offset;
                        }
                      | postfix_expression INC_OP  
                        {
                           $$ = new op_unary_astnode(std::string("PP"),$1);
                           $$->base_type = $1->base_type;
                           $$->rem_type = $1->rem_type;
                           $$->actual_type = $1->actual_type;
                        }

    primary_expression: IDENTIFIER 
                        {
                           std::string actualType;
                           std::string baseType;
                           std::string remType;
                           int offset;
                           bool isarray=false;
                           for(auto i: lst->Entries){
                              if (i.first==$1){
                                 actualType = i.second.returnType;
                                 baseType = i.second.base_type;
                                 remType = i.second.rem_type;
                                 isarray=i.second.isarray;
                                 offset = i.second.offset; //added to right now
                                 break;
                              }
                           }

                           $$ = new identifier_astnode($1, offset,remType);
                           $$->actual_type = actualType;
                           $$->base_type = baseType;
                           //$$->rem_type = remType;
                           $$->is_array = isarray;
                           //cout<<$1<<" "<<$$->rem_type<<endl;
                  
                           
                        }
                      | INT_CONSTANT 
                        {
                           $$ = new intconst_astnode(std::stoi($1));
                           $$->actual_type = "int";
                           $$->base_type = "int";
                           $$->rem_type = "";
                           intconst_val = std::stoi($1);

                        }
                      | STRING_LITERAL 
                        {
                           $$ = new stringconst_astnode($1);
                           $$->base_type="string";
                           $$->actual_type="string";

                        }
                      | '(' expression ')' 
                        {
                           $$ = $2;
                        }

    expression_list: expression 
                     {
                        $$ = vector<exp_astnode*>({$1});

                       
                     }
                   | expression_list ',' expression 
                     {
                        $1.push_back($3);
                        $$ = $1;
                     }

    unary_operator: '-' {$$ = std::string("UMINUS");}
                  | '!' {$$ = std::string("NOT");}
                  | '&' {$$ = std::string("ADDRESS");}
                  | '*' {$$ = std::string("DEREF");}

    selection_statement: IF '(' expression ')' statement ELSE statement {$$ = new if_astnode($3,$5,$7);}

    iteration_statement: WHILE '(' expression ')' statement {$$ = new while_astnode($3,$5);}
                       | FOR '(' assignment_expression ';' expression ';' assignment_expression ')' statement {$$ = new for_astnode($3,$5,$7,$9);}

    declaration_list: declaration 
                    | declaration_list declaration

    declaration: type_specifier declarator_list ';'
                  {

                     
                     for(auto i: decList){
                        // cout<<lst->Entries[i].size<<endl;
                        // if the variable is a pointer
                        if (lst->Entries[i].returnType[0]=='*')
                           lst->Entries[i].size *= 4;
                        else
                           lst->Entries[i].size *= size;
                        lst->Entries[i].base_type = $1;
                        lst->Entries[i].rem_type = lst->Entries[i].returnType;
                        lst->Entries[i].returnType = $1 + lst->Entries[i].returnType;
                        if ( (lst->Entries[i].rem_type.size()!=0 && lst->Entries[i].rem_type[0]!='*' && lst->Entries[i].base_type == "void") || lst->Entries[i].returnType=="void"){
                           cout<<"Error at line "<<line_num<<": "<<"Cannot declare variable of type \"void\""<<endl;
                           exit(1);
                        }
                     }
                     decList.clear();
                  }

    declarator_list:  declarator
                     {  
                        if (lst->Entries.find($1->symbolName)!=lst->Entries.end())
                        {
                           cout<<"Error at line "<<line_num<<": "<<$1->symbolName<<" has a previous declaration"<<endl;
                           exit(1);
                        }
                        std::string symbolName = $1->symbolName;
                        lst->Entries[symbolName] = Symbol();
                        lst->Entries[symbolName].symbolName = $1->symbolName;
                        lst->Entries[symbolName].returnType = $1->returnType ;
                        lst->Entries[symbolName].varfun = "var";
                        lst->Entries[symbolName].scope = "local";
                        lst->Entries[symbolName].size = $1->size;
                        lst->Entries[symbolName].isarray = $1->isarray;

                        decList.push_back(symbolName);
                        localQueue.push($1->symbolName);
                     }
                    | declarator_list ','  declarator 
                     {
                        if (lst->Entries.find($3->symbolName)!=lst->Entries.end())
                        {
                           cout<<"Error at line "<<line_num<<": "<<$3->symbolName<<" has a previous declaration"<<endl;
                           exit(1);
                        }
                        std::string symbolName = $3->symbolName;
                        lst->Entries[symbolName] = Symbol();
                        lst->Entries[symbolName].symbolName = $3->symbolName;
                        lst->Entries[symbolName].returnType = $3->returnType;
                        lst->Entries[symbolName].varfun = "var";
                        lst->Entries[symbolName].scope = "local";
                        lst->Entries[symbolName].size = $3->size;
                        lst->Entries[symbolName].isarray = $3->isarray;
                        decList.push_back(symbolName);
                        localQueue.push($3->symbolName);
                     }

%%
void IPL::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cout << "Error at line "<< line_num << ": "<< err_message<<"\n";
   exit(1);
}


