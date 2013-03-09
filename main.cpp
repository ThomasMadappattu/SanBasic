/*
description          :A simple intrepreter for a basic like language . 
written  by          :Sandeep Mathew     
License              :Released to Public Domain
notice               :You should note that this programming style is not worth
                      emulating . This is a dirty hack i wrote in  a few hours.
                      However the program as a whole should be instructional
*/ 
#include <cstdlib>
#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <cstring>
#include <cctype>
#include <map>
#include <conio.h>

/* the token id's*/
#define TOKEN_END_OF_INPUT 100
#define TOKEN_NUMBER       101 
#define TOKEN_VARIABLE     102
#define TOKEN_EQUAL        103
#define TOKEN_LESS         104
#define TOKEN_GREATER      105
#define TOKEN_OR           106
#define TOKEN_AND          107  
#define TOKEN_PLUS         108
#define TOKEN_MINUS        109
#define TOKEN_COMMA        110
#define TOKEN_SEMI         111          
#define TOKEN_MUL          112
#define TOKEN_DIV          113
#define TOKEN_LP           114
#define TOKEN_RP           115
#define TOKEN_MOD          116
#define TOKEN_CR           129 

#define TOKEN_LET          117
#define TOKEN_PRINT        118
#define TOKEN_IF           119
#define TOKEN_THEN         120
#define TOKEN_ELSE         121
#define TOKEN_FOR          122
#define TOKEN_NEXT         123   
#define TOKEN_GOTO         124
#define TOKEN_GOSUB        125 
#define TOKEN_RETURN       126  
#define TOKEN_CALL         127
#define TOKEN_STRING       128
#define TOKEN_INPUT        130 
#define TOKEN_TO           131
#define TOKEN_REM          132
#define TOKEN_END          133

/* program restrictions*/
#define MAX_NUMBER_LEN     10
#define MAX_VAR_LEN        20   
#define MAX_TOKEN_LEN      200 


using namespace std;
/* main data structures used by the intrepreter*/
struct Token
{
       char token[MAX_TOKEN_LEN];
       int type;
};
struct for_state
{
   string line_start;
   string for_var;
   int to;
};    
list <char> source_file;
list <char> :: iterator source_ptr;
stack<for_state> for_stack; 
map<string,int> varTab; 
stack<string> gosub_stack;


/* functions related to lexical analysis*/
void open_source_file(char *filename);
void init_tokenizer();
void display_source_file();
void print_source_file();
Token get_token();
void quit_error(char *msg);
Token current_token; 
void match(int type,bool &correct);

/*function related to parsing*/
int factor();
int term();
int expression();
void print_statement();
void let_statement();
void input_statement();
void rem_statement();
void if_statement();
void next_statement();
void goto_statement();
void gosub_statement();
void return_statement();
void jump_to_line(char *lineno);
void for_statement();
void end_statement();
void line();

int main(int argc, char *argv[])
{
    
     
    if(argc != 2 ) 
    {
        cout<<"\n SanBasic Intrepreter : Usage "<<argv[0]<<"   <filename>";   
        exit(EXIT_FAILURE);
    }    
    open_source_file(argv[1]);
    // (yea for debugging !)display_source_file();
    init_tokenizer();    
    /*
       for the production
         program -> (line)*
    
    */
    while(current_token.type != TOKEN_END_OF_INPUT )
    {
      
       
        line();
        
    }
     
    return EXIT_SUCCESS;
}
/* initilizes the tokenizer*/
void init_tokenizer()
{
  source_ptr = source_file.begin();
  current_token= get_token();  
  
}

/*opens the input file and saves it into a linked list*/  
void open_source_file(char *filename)
{
      FILE *input_file = fopen(filename,"r");
      assert(input_file);
      char ch;
      /* first condition is needed so that we do not inset the end of
         file marker
      */
      while(((ch = fgetc(input_file))!= NULL)&& !feof(input_file)) 
      {
         source_file.push_back(tolower(ch));
         
      }
      /* insert final delimiter*/
      source_file.push_back('\0');

}
/* for debugging display the inputted source file*/
void display_source_file()
{
   list<char> ::iterator p = source_file.begin();
   while(p != source_file.end())
   {
     cout<<*p;
     p++;
  
   }
   cout<<"\n";
}
/* skip white space*/
void skip_blanks()
{
     while((*source_ptr != '\0') && ((*source_ptr == ' ')|| (*source_ptr == '\r'))) source_ptr++;
}

/* ah hoc lexer , gets a token from the stream and identifies it*/
Token get_token()
{
    Token tok;
    char *tok_ptr = tok.token;
    int num_len = 0;
    int var_len = 0;
    int const_str_len =0;
    skip_blanks();
    if( *source_ptr == '\0')
    {
       *tok_ptr ='\0';
       tok.type=TOKEN_END_OF_INPUT;
    }  
    else if (*source_ptr == '+')
    {
       *tok_ptr++ = '+';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_PLUS;
    }
    else if (*source_ptr == '-')
    {
       *tok_ptr++ = '-';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_MINUS;
    }
    else if (*source_ptr == '<')
    {
       *tok_ptr++ = '<';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_LESS;
    }
    else if (*source_ptr == '>')
    {
       *tok_ptr++ = '>';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_GREATER;
    }
    else if (*source_ptr == '=')
    {
       *tok_ptr++ = '=';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_EQUAL;
    }
    else if (*source_ptr == '*')
    {
       *tok_ptr++ = '*';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_MUL;
    }
    else if (*source_ptr == '/')
    {
       *tok_ptr++ = '/';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_DIV;
    }
    else if (*source_ptr == '(')
    {
       *tok_ptr++ = '(';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_LP;
    }
    else if (*source_ptr == ')')
    {
       *tok_ptr++ = ')';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_RP;
    }
    else if (*source_ptr == ',')
    {
       *tok_ptr++ = ',';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_COMMA;
    }
    else if (*source_ptr == ';')
    {
       *tok_ptr++ = ';';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_SEMI;
    }
    else if (*source_ptr == '%')
    {
       *tok_ptr++ = '%';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_MOD;
    }
    else if (*source_ptr == '&')
    {
       *tok_ptr++ = '&';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_AND;
    }
    else if (*source_ptr == '|')
    {
       *tok_ptr++ = '|';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_OR;
    }
    else if (*source_ptr == '\n')
    {
       *tok_ptr++ = '\n';
       *tok_ptr = '\0';
       *source_ptr++;
       tok.type = TOKEN_CR;
    }
    else if(isdigit(*source_ptr))
    {
        while(isdigit(*source_ptr) && (num_len < MAX_NUMBER_LEN+1 ))
        {
          ++num_len;
          *tok_ptr++ = *source_ptr++;
        }
        if(num_len == MAX_NUMBER_LEN+1 )
        {
          quit_error("Number length exceeded");
        }
        else
        {
           tok.type = TOKEN_NUMBER;
           *tok_ptr = '\0';
        }
    }
    else if((*source_ptr == '"') )
    {
        *tok_ptr++ = *source_ptr++;
        while((*source_ptr != '"')&& (const_str_len < MAX_TOKEN_LEN-2))
        {
          *tok_ptr++ = *source_ptr++;
          ++const_str_len;
        }
        if(const_str_len == MAX_TOKEN_LEN-1)
        {
          quit_error("constant string too long");
        }
        else
        {
           *tok_ptr++= *source_ptr++;
           *tok_ptr = '\0';
          
           tok.type = TOKEN_STRING;
        }
    }
    else if(isalpha(*source_ptr))
    {
       while(isalpha(*source_ptr) && (var_len < MAX_VAR_LEN))
       {
         ++var_len;
         *tok_ptr++ = *source_ptr++; 
       }    
       if( var_len == MAX_VAR_LEN)
       {
           quit_error("Token name length exceeded!");
           
       }
       else
       {
          
          *tok_ptr = '\0';
          if(strcmp(tok.token,"print") == 0 )tok.type = TOKEN_PRINT;
          else if(strcmp(tok.token,"let")==0)tok.type = TOKEN_LET;
          else if(strcmp(tok.token,"if")==0)tok.type = TOKEN_IF;
          else if(strcmp(tok.token,"goto")==0)tok.type = TOKEN_GOTO; 
          else if(strcmp(tok.token,"gosub")==0)tok.type = TOKEN_GOSUB; 
          else if(strcmp(tok.token,"then")==0)tok.type = TOKEN_THEN;
          else if(strcmp(tok.token,"for")==0)tok.type = TOKEN_FOR;
          else if(strcmp(tok.token,"next")==0)tok.type = TOKEN_NEXT;
          else if(strcmp(tok.token,"return")==0)tok.type = TOKEN_RETURN;
          else if(strcmp(tok.token,"else")==0)tok.type = TOKEN_ELSE;
          else if(strcmp(tok.token,"input")==0)tok.type = TOKEN_INPUT;
          else if(strcmp(tok.token,"to")==0)tok.type = TOKEN_TO;
          else if(strcmp(tok.token,"rem")==0)tok.type = TOKEN_REM;
          else if(strcmp(tok.token,"end")==0)tok.type = TOKEN_END;
          else tok.type= TOKEN_VARIABLE;
          skip_blanks();
       } 
    }
    else
    {
        quit_error("Unidentified token!");
    }
    return tok; 
}
/* display message and quit*/
void quit_error(char *msg)
{
      cout<<"\n >-- Error!--"<<msg<<"--<"<<"\n";
      exit(EXIT_FAILURE); 
}

/* match the input stream for the given token  and advance to next token*/
void  match(int type,bool &correct)
{
     
     if(current_token.type == type ) correct = true;
     else   correct = false;
     if( correct) current_token = get_token();
     
       
}
/* look up the variable from the variable table*/
int lookup_var(string varname)
{
     map <string,int> :: iterator p = varTab.begin();
     if( (p = varTab.find(varname))!= varTab.end())
        return varTab[varname];
     else
        quit_error("Variable not found!");
}
/*
   for the prodcution
     factor -> number | "(" expression")"|variable
*/      
int factor(void)
{
    int t;
    bool correct;
    switch(current_token.type)
    {
        case TOKEN_NUMBER:
                   t = atoi(current_token.token);
                   match(TOKEN_NUMBER,correct);
                   if(!correct)quit_error("Expected Number!");
                   break;      
        case TOKEN_LP:
                   match(TOKEN_LP,correct);
                   if(!correct)quit_error("Expected (");
                   t = expression();
                   match(TOKEN_RP,correct);
                   if(!correct)quit_error("Expected )");
                   break;
          
        case TOKEN_VARIABLE:
                   t = lookup_var(string(current_token.token));
                   match(TOKEN_VARIABLE,correct);
                   if(!correct)quit_error("Expected Variable");
                   break;
        default:  
                  quit_error("Factor Error!");
                  break;
    }
    return t;
} 

/*
  for the production
     term ->(factor ("*"  | " / " | "%" ) )* factor
*/ 
int term()
{
     int t1,t2,oper;
     t1 = factor();
     
     oper = current_token.type;
     
     while((oper == TOKEN_MUL) || (oper == TOKEN_DIV) || (oper == TOKEN_MOD))
     {
          current_token = get_token();
         
          t2 = factor();
              
          switch(oper)
          {
              case TOKEN_MUL:
                        t1 *= t2;
                        break;
              case TOKEN_DIV:
                        t1 /= t2;
                        break;
               case TOKEN_MOD:
                         t1 %= t2;
                         break;
          }   
          oper = current_token.type;     
     }
     return t1;    
}     
/*
  for the production 
    expression -> (term ( "+" | "-"" | "&" | "|" | ))* term
*/
int expression()
{
    int t1,t2,oper;
    t1 = term();
    oper = current_token.type;
    while ((oper == TOKEN_PLUS) || (oper == TOKEN_MINUS) || (oper == TOKEN_AND) || (oper == TOKEN_OR))
    {
       current_token = get_token();
       t2 = term();
       switch(oper)
       {
           case TOKEN_PLUS:
                     t1+=t2;
                     break;
           case TOKEN_MINUS:
                     t1 -=t2;
                     break;
           case TOKEN_AND:
                     t1 &= t2;
                     break;
           case TOKEN_OR:
                     t1 |= t2;
                     break;      
       }
        oper = current_token.type;
    }  
    return t1; 
   
}
/*
  for the production
     relational -> expression ( "<" | " > " | "=" ) expression
*/
int relational()
{
    int t1,t2,oper;
    t1 = expression();
    oper = current_token.type;
    while( (oper == TOKEN_LESS) || (oper == TOKEN_GREATER) || (oper == TOKEN_EQUAL))
    {
        current_token = get_token();
        t2 = expression();
        switch(oper)
        {
            case TOKEN_LESS:
                      t1 = t1 < t2;
                      break;
            case TOKEN_GREATER:
                      t1 = t1 > t2;
                      break;
            case TOKEN_EQUAL:
                       t1 = t1 == t2;
                       break;
       } 
       oper = current_token.type;  
    }
    return t1;                              
}
/*
for the production
  statement - > ifstatement | rem_statement | input_statement | let_statement | for_statemet 
  statement - > goto_statement | gosub_statement | return_statement | if_statement |end_satement
  statemet  - > next_statement | print_statement
*/
void statement()
{
   switch(current_token.type)
   {
       case TOKEN_PRINT:
            print_statement();
            break;
     
       case TOKEN_REM:
            rem_statement();
            break; 
       case TOKEN_INPUT:
            input_statement();
            break;
       case TOKEN_LET:
            let_statement();
            break;
       case TOKEN_FOR:
            for_statement();
            break;
       case TOKEN_NEXT:
            next_statement();
            break;
       case TOKEN_GOTO:
            goto_statement();
            break;
       case TOKEN_GOSUB:
            gosub_statement();
            break;
       case TOKEN_RETURN:
            return_statement();
            break;
       case TOKEN_IF:
            if_statement();
            break;
       case TOKEN_END:
            end_statement();
            break;
       default:
             quit_error(current_token.token);      
   }
}
/* 
this is a bad practice ... this should have been done by the lexer.
 for the production
   rem_statemet - > "rem" any_number_of_crap "\n"
*/
void rem_statement()
{
     bool correct;
     match(TOKEN_REM,correct);
     if(!correct)quit_error("expected rem");
     while((current_token.type != TOKEN_CR) && (current_token.type != TOKEN_END_OF_INPUT))
     {
         current_token = get_token();
     }
     if(current_token.type != TOKEN_END_OF_INPUT ) current_token = get_token();
}   
/*
for the prodution 
  
  print_statement -> "print " ( expression ( ","  | nothing ))* expression "\n"
*/

void print_statement()
{
    bool correct;
    match(TOKEN_PRINT,correct);
    if(!correct)quit_error("Expected print");
    do
    {
        if(current_token.type == TOKEN_STRING)
        {
             char *tok_ptr = current_token.token;
             tok_ptr++;
             while((*tok_ptr != '"') && (*tok_ptr != '\0'))
                cout<<*tok_ptr++;
             current_token = get_token();   
        }   
        else if(current_token.type == TOKEN_COMMA)
        {
             cout<<" ";
             current_token = get_token();
        }
        else if(current_token.type == TOKEN_SEMI )
        {
            current_token  = get_token();
        }
        else if((current_token.type == TOKEN_VARIABLE) ||(current_token.type == TOKEN_NUMBER) || (current_token.type == TOKEN_LP ))
        {
             cout<<expression();
        } 
        else 
        {
            break;
        }    
    }while((current_token.type != TOKEN_CR) && (current_token.type != TOKEN_END_OF_INPUT));
    cout<<"\n";
    current_token = get_token(); 
}

/*
for the production
let_statement -> "let" variable "=" expression "\n"
*/
void let_statement()
{
     bool correct;
     string var_name;
   
     match(TOKEN_LET,correct);
     if(!correct)quit_error("Expected let");
     
     if(current_token.type == TOKEN_VARIABLE)
     {   
        var_name = string(current_token.token);
        match(TOKEN_VARIABLE,correct);
        if(!correct)quit_error("Expected  variable");
     }
    
     match(TOKEN_EQUAL,correct);
     if(!correct)quit_error("Expected =");
     varTab[var_name] = expression();
     if( (current_token.type == TOKEN_CR))
     {
         current_token = get_token();
     }
     else if ( (current_token.type != TOKEN_END_OF_INPUT))
     {
         quit_error("Stray inputs after let");
     } 
     
 
}
/*
for the production
 input_statement -> "input" variable "\n"
*/

void input_statement()
{
     bool correct;
     int value;
     match(TOKEN_INPUT,correct);
     if(!correct)quit_error("Expected input");
     if(current_token.type == TOKEN_VARIABLE)
     {
         fflush(stdin);
         string var_name = string(current_token.token);
         cin>>value;
         varTab[var_name] = value;
         current_token = get_token();
     }   
     else 
     {
          quit_error("Expected variable!");
     }
     if(current_token.type == TOKEN_CR ) current_token = get_token();
     else if(current_token.type != TOKEN_END_OF_INPUT) quit_error("Expected CR");
 }
 
 /*
     jump to the given line number
 */
 
 void jump_to_line(char *line_no)
 {
      init_tokenizer();
      while(strcmp(current_token.token,line_no) != 0)
       {
          do
          { 
            do
            {
                 current_token = get_token();
            }while((current_token.type != TOKEN_CR) && (current_token.type) != TOKEN_END_OF_INPUT);                                      
            if(current_token.type == TOKEN_CR) current_token = get_token();
          
          }while(current_token.type != TOKEN_NUMBER);
       }
 }
 /*
 goto_statement -> "goto" linenumber
 */
 void goto_statement()
 {
      bool correct;
      char temp_token[80];
      match(TOKEN_GOTO,correct);
      if(current_token.type != TOKEN_NUMBER)
      {
         quit_error("Expected number!");  
      }
      else
      {
        strcpy(temp_token,current_token.token); 
        jump_to_line(temp_token);
      } 
 }
 /*
 forstatemet -> "for" variable "=" expression "to" expression "\n"
 */ 
 void for_statement()
 {
    int to;
    bool correct;
    string var_name;
    for_state this_for_state;
    match(TOKEN_FOR,correct);
    if(!correct)quit_error("Expected  for in for!");
    if(current_token.type == TOKEN_VARIABLE)
    {
       var_name = string(current_token.token);
       current_token = get_token();
    }  
    else
    {
        quit_error("Expected variable in for");
    }
    match(TOKEN_EQUAL ,correct);
    if(!correct)quit_error("Expected =");
    varTab[var_name] = expression();
    match(TOKEN_TO,correct);
    if(!correct)quit_error("Expected to in for");
    to = expression();
    match(TOKEN_CR,correct);
    if(!correct)quit_error("Expected CR");
    
    if(current_token.type != TOKEN_NUMBER)                    
          quit_error("Expected number after for ");
    this_for_state.line_start = string(current_token.token);
    this_for_state.for_var =   var_name;
    this_for_state.to = to;
    for_stack.push(this_for_state); 
 }
 
 /*
 if_statement -> "if" relational then statement B
 B -> "\n" | "else" statement "\n" 
*/
 void if_statement()
 {
    int condition;
    bool correct;
    match(TOKEN_IF,correct);
    if(!correct)quit_error("Expected if");
    condition = relational();
    match(TOKEN_THEN,correct);
    if(!correct)quit_error("Expected then in if");
    if(condition)
    {
       statement();
    }
    else
    {
        do
        { 
           current_token = get_token();
        }while((current_token.type != TOKEN_CR) && (current_token.type != TOKEN_END_OF_INPUT) && (current_token.type  != TOKEN_ELSE));
        if(current_token.type == TOKEN_ELSE)
        {
           current_token = get_token();
           statement();
        } 
        else if(current_token.type == TOKEN_CR)
        {
            current_token = get_token();
        }
     }             
                   
  }
 /*
 next_statment -> "next" variable "\n"
 */
 void next_statement()
 {
      int var;
      bool correct;
      for_state current_state;
      match(TOKEN_NEXT,correct);
      if(!correct)quit_error("Expected next");
      if(current_token.type == TOKEN_VARIABLE)
      {
        current_state = for_stack.top();
        if((current_state.for_var.compare(current_token.token) == 0 ) && (!for_stack.empty()))
        {
           var = lookup_var(string(current_token.token));
           var++;
           varTab[string(current_token.token)] = var;
           current_token = get_token();
           if( var <= current_state.to)
           {
               jump_to_line((char *)current_state.line_start.c_str());
           }
           else
           {
              if(!for_stack.empty())
              for_stack.pop();
              if(current_token.type == TOKEN_CR)
              {
                 current_token = get_token();
              }
              else
              {
                 if(current_token.type != TOKEN_END_OF_INPUT)
                 {
                   quit_error("Expected CR");
                 }
              }
           }    
         }
         else
         {
             quit_error("unmatched variable for the preceeding for");
         }
      }                   
      else
      {
         quit_error("variable expected here!"); 
      }  
}     
/*
gosub -> "gosub" line no
*/

void gosub_statement()
{
     bool correct;
     string var_name; 
     match(TOKEN_GOSUB,correct);
     if(!correct)quit_error("expected gosub");
     if(current_token.type == TOKEN_NUMBER)
     {
         var_name = string(current_token.token);
         gosub_stack.push(var_name);
         jump_to_line((char*)var_name.c_str());
         
     } 
     else
     {
         quit_error("Expected number here");
     }
}    
/*
   return -> "return" 
*/
void return_statement()
{
     bool correct;
     match(TOKEN_RETURN,correct);
     if(!correct)quit_error("expected return");
     if(!gosub_stack.empty())
     {
          string to_jump = gosub_stack.top();
          gosub_stack.pop();
          jump_to_line((char *)to_jump.c_str());                    
     }             
}
/*
end_statement -> "end" 
*/
void end_statement()
{
     bool correct;
     match(TOKEN_END,correct);
     if(!correct)quit_error("expected END");
     else
     exit(EXIT_SUCCESS);
} 
/*
   line -> number statement
*/
void line()
{
    
     bool  correct;
     match(TOKEN_NUMBER,correct);
     if(!correct)quit_error("expected line number!");
     statement();
}     
