//Title: Project 3
//Authors: Michael Schmidt, Eduardo Roman, Nicholas ivanoff
//cs460
//Description: This file contains the implementation of our syntactical analyzer class. It will parse through the list of grammar and receive the tokens and lexemes for our code generator.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include "SyntacticalAnalyzer.h"

using namespace std;

token_type otherTokens[] = {IDENT_T, NUMLIT_T, STRLIT_T, CONS_T, IF_T, DISPLAY_T, NEWLINE_T, LISTOP_T,
                            AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T,NULLP_T, STRINGP_T, PLUS_T, MINUS_T,
                            DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, COND_T, ELSE_T, LPAREN_T, SQUOTE_T, NONE};

/*Function: Constructor;
this function will inialize the Syntactical class functions, as well create essential items needed to complete the process.
 */
SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
    cg = new CodeGen (filename);
    lex = new LexicalAnalyzer (filename);

    string temp = filename;
    temp.replace(temp.end()-2, temp.end(), "p2");
    tokenActionsFile.open(temp);

    token = lex->GetToken();  // gets the first token to start
    int errors = program();
    while(token != EOF_T)
        token = lex->GetToken();
    lex->FinalErrorReport(errors);
}
/* function: Destructor;
 this function will be destroy the lexical and Code generaotr objects.
 */
SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
	delete lex;
    delete cg;
}

/*
//Fuction: Program: This function will initialize the grammar rules of our project.
*/
int SyntacticalAnalyzer::program() {
    tokenActionsFile << "Entering Program function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T){
        tokenActionsFile << "Using Rule 1\n";
        token = lex->GetToken(); // get next token after '('
        errors+= define();
        if(token == LPAREN_T){
            token = lex->GetToken(); // get next token after '('
            errors+= more_defines();
        } else{
            lex->ReportSyntactErrors("LPAREN_T expected");
            errors+= more_defines(); //Pretend there was a LPAREN_T
            errors++;
        }
    } else{
        lex->ReportSyntactErrors("LPAREN_T expected");
        errors++;
    }
    tokenActionsFile << "Exiting Program function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Fuction: define(): This function will be initialized from function program and
will determine the body of a function.
*/
int SyntacticalAnalyzer::define() {
    tokenActionsFile << "Entering Define function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    string main;
    int errors = 0;
    if(token == DEFINE_T){
        tokenActionsFile << "Using Rule 4\n";
        token = lex->GetToken(); // get next token after 'define'
        if(token == LPAREN_T){
            token = lex->GetToken(); // get next token after '('
            if(token == IDENT_T){
                if(lex->GetLexeme() == "main"){
		  //writing to code gen
                    cg->WriteCode(0,"int " + lex->GetLexeme() + "(");
                    main = "main";
                }
                else{
		  //writing to code gen
		  cg->WriteCode(0,"Object " + lex->GetLexeme() + "(");
		}
		token = lex->GetToken(); // get next token after 'ident'
                errors +=param_list();
                if(token == RPAREN_T){
		  //writing to code gen
                    cg->WriteCode(0, ")\n{\n");
                    token = lex->GetToken(); // get next token after ')'
                    errors += stmt("");
                    errors += stmt_list("");
                    if(token == RPAREN_T){
                        if(main == "main")
			  cg->WriteCode(1, "return 0;\n"); //code gen
                        cg->WriteCode(0, "}\n\n"); //writing to code gen
                        token = lex->GetToken(); // get next token after ')'
                    } else{
                        lex->ReportSyntactErrors("RPAREN_T expected");
                        errors++;
                    }
                } else{
                    lex->ReportSyntactErrors("RPAREN_T expected");
                    errors++;
                }
            } else{
                lex->ReportSyntactErrors("IDENT_T expected");
                errors++;
            }
        } else{
            lex->ReportSyntactErrors("LPAREN_T expected");
            errors++;
        }
    } else{
        lex->ReportSyntactErrors("DEFINE_T expected");
        errors++;
    }
    tokenActionsFile << "Exiting Define function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: more_defines(); This function will call <define> or itself. It builds up a nested define of functions. It will check for nested defines from scheme code
 */
int SyntacticalAnalyzer::more_defines() {
    tokenActionsFile << "Entering More_Defines function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == DEFINE_T){
        tokenActionsFile << "Using Rule 2\n";
        errors += define();
        if(token == LPAREN_T){
            token = lex->GetToken();
            errors += more_defines();
        } else{
            lex->ReportSyntactErrors("LPAREN_T expected");
            errors += more_defines(); //Pretend LPAREN_T is there
            errors++;
        }
    }else if(token == IDENT_T){
        tokenActionsFile << "Using Rule 3\n";
        token = lex->GetToken(); // get next token after 'ident'
        errors += stmt_list("");
        if(token == RPAREN_T){
            token = lex->GetToken(); // get next token after ')'
        } else{
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting More_Defines function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function:stmt_list(); this function will check if the correct information is presented inside a define fucntion. 
 */
int SyntacticalAnalyzer::stmt_list(string op) {
    tokenActionsFile << "Entering Stmt_List function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T ||
            token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 5\n";
        errors += stmt("");
	//writin to Code Gen
        cg->WriteCode(0, op);
        errors += stmt_list("");
    }
    else if(token == RPAREN_T)
      {
        tokenActionsFile << "Using Rule 6\n";
      }
    else
      {
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
      }
    
    tokenActionsFile << "Exiting Stmt_List function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: stmt(); this function will check for statements that express an evaluation of some sort, withing a define funciton. 
 */
int SyntacticalAnalyzer::stmt(string checkIF) {
    tokenActionsFile << "Entering Stmt function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T){
        tokenActionsFile << "Using Rule 9\n";
        token = lex->GetToken(); // get next token after '('
        errors += action(checkIF);
        if(token == RPAREN_T){
            token = lex->GetToken();
        } else{
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
    }else if(token == IDENT_T) {
        tokenActionsFile << "Using Rule 8\n";
        if(checkIF == "return")
	  cg->WriteCode(1, "return"); //writing to code gen
        cg->WriteCode(0, "( " + lex->GetLexeme() + " )"); //send to code gen
        token = lex->GetToken(); // get next token after 'ident'
    }else if(token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 7\n";
        if(checkIF == "return")
	  cg->WriteCode(1, "return"); //writint to code gen
        errors += literal();
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Stmt function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: literal(); this function will check for num, string, quoted literals as well any other tokens in scheme function
 */
int SyntacticalAnalyzer::literal() {
    tokenActionsFile << "Entering Literal function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == NUMLIT_T){
      //writing to code gen
        cg->WriteCode(0, " Object (" + lex->GetLexeme() + ")");
        tokenActionsFile << "Using Rule 10\n";
        token = lex->GetToken();

    }
    else if(token == STRLIT_T){
      //writing to code gen
        cg->WriteCode(0, " Object (" + lex->GetLexeme() + ")");
        tokenActionsFile << "Using Rule 11\n";
        token = lex->GetToken();

    }
    else if(token == SQUOTE_T){
      //writing to code gen
      cg->WriteCode(0, " Object (\"" + lex->GetLexeme() + "\")");
      tokenActionsFile << "Using Rule 12\n";
      token = lex->GetToken();
      errors+= quoted_list();
    }
    else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Literal function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: quoted_list(); This function will check for correct any other tokens from our grammar,
 */
int SyntacticalAnalyzer::quoted_list() {
    tokenActionsFile << "Entering Quoted_Lit function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(&otherTokens[32] != find(&otherTokens[0], &otherTokens[32], token)){
        tokenActionsFile << "Using Rule 13\n";
        errors += any_other_token();
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Quoted_Lit function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: more_tokens(); This function will check for correctnes in any other tokens from our grammar, and anything else a user might input in there scheme code,
*/

int SyntacticalAnalyzer::more_tokens() {
    tokenActionsFile << "Entering More_Tokens function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(&otherTokens[32] != find(&otherTokens[0], &otherTokens[32], token)) {
        tokenActionsFile << "Using Rule 14\n";
        errors += any_other_token();
        errors += more_tokens();
    }
    else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 15\n";
        //Use rule 15
    }else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting More_Tokens function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
function: param_list(); this function will check for correct parameters pas into a define function from scheme code.
 */
int SyntacticalAnalyzer::param_list() {
    tokenActionsFile << "Entering Param_List function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == IDENT_T){
        tokenActionsFile << "Using Rule 16\n";
	//writing to code gen
        cg->WriteCode(0, "Object " + lex->GetLexeme());
        token = lex->GetToken();
        if(token != RPAREN_T)
            cg->WriteCode(0, ", ");
        errors += param_list();
    }
    else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 17\n";
        //Use rule 17
    }
    else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Param_List function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

/*
function: else_part(); this function will check for correct staments in the else part of scheme function;
 */
int SyntacticalAnalyzer::else_part() {
    tokenActionsFile << "Entering Else_Part function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T ||
       token == STRLIT_T || token == SQUOTE_T){
      //Writing to code Gen
        cg->WriteCode(1, "else\n\t{\n");
        tokenActionsFile << "Using Rule 18\n";
	//Writing to code Gen
	cg->WriteCode(2, "");
        errors += stmt("return");
	//Writing to code Gen
	cg->WriteCode(0, ";\n");
    }
    else if(token == RPAREN_T){
      cg->WriteCode(2, "\t\t}\n");
      tokenActionsFile << "Using Rule 19\n";
      //Use rule 19
    }
    else
      {
	lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
	errors++;
      }
    
    tokenActionsFile << "Exiting Else_Part function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
function:stmt_pair(); this function will check for both staments and statement pair bodies.
 */
int SyntacticalAnalyzer::stmt_pair() {
    tokenActionsFile << "Entering Stmt_Pair function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T){
        tokenActionsFile << "Using Rule 20\n";
        token = lex->GetToken();
        errors += stmt_pair_body();
    }else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 21\n";
        //Use rule 21
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Stmt_Pair function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

/*
Function: stmt_pair_body(); this function will check for the correct format of staments from scheme code tokens.
 */
int SyntacticalAnalyzer::stmt_pair_body()
{
    tokenActionsFile << "Entering Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == ELSE_T)
    {
        tokenActionsFile << "Using Rule 23\n";
        token = lex->GetToken();
        errors += stmt("");
        if(token == RPAREN_T)
        {
            token = lex->GetToken();
        }
        else
        {
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
    }
    else if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T)
    {
        tokenActionsFile << "Using Rule 22\n";
        errors += stmt("");
        errors += stmt("");
        if(token == RPAREN_T)
        {
            token = lex->GetToken();
        }
        else{
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
        errors += stmt_pair();
    }
    else
    {
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Fucntion:any_other_token(); this function will check to see if the correct token is read from schem code, and if it is not correct it will write to error function
 */

int SyntacticalAnalyzer::any_other_token()
{
    tokenActionsFile << "Entering Any_Other_Token function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T)
    {
        tokenActionsFile << "Using Rule 50\n";
	//Write To code gen
	cg->WriteCode(0,"Object(\"(");
	token = lex->GetToken();
	//Write To code gen
	cg->WriteCode(0,lex->GetLexeme());
	errors += more_tokens();
	//Write To code gen
	cg->WriteCode(0,"\"));\n");
        if(token == RPAREN_T)
        {
            token = lex->GetToken();
        }
        else
        {
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
    }
    else if(token == SQUOTE_T)
    {
        tokenActionsFile << "Using Rule 79\n";
	//Write To code gen. SQUOTE should be a cout ??
        cg->WriteCode(1,"cout << " + lex->GetLexeme());
        token = lex->GetToken();
        errors += any_other_token();
	//Write To code gen
	cg->WriteCode(0," << endl; ");
    }
    else if(&otherTokens[32] != find(&otherTokens[0], &otherTokens[32], token))
    {
        //Both LPAREN_T and SQUOTE_T should have all ready been found so there is no risk of using find
        //tokenActionsFile << "Using Rule in 51-81, not 79\n";
        for(int i = 0; i < 31; i++){
            if(token == otherTokens[i]){
                if(token == COND_T || token == ELSE_T)
                    tokenActionsFile << "Using Rule " << i + 52 << "\n";
                else
                    tokenActionsFile << "Using Rule " << i + 51 << "\n";
            }
        }
        token = lex->GetToken();
	//Write To code gen
	cg->WriteCode(0,lex->GetLexeme());

    }

    else
    {
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors ++;
    }
    tokenActionsFile << "Exiting Any_Other_Token function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}
/*
Function: action(); this function will check scheme code for the correct tokens as to the action it is stating.
*/
int SyntacticalAnalyzer::action(string check) {
    tokenActionsFile << "Entering Action function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if ( token == COND_T ){
        tokenActionsFile << "Using Rule 25\n";
        token = lex->GetToken();
        if (token == LPAREN_T) {
            token = lex->GetToken();
            errors += stmt_pair_body();
        } else{
            lex->ReportSyntactErrors("LPAREN_T expected");
            errors += stmt_pair_body(); // Pretend LPAREN_T is there
            errors++;
        }
    }
    else if ( token == CONS_T) {
        //rules with tokens and two stmt after it
        tokenActionsFile << "Using Rule 27\n";
        token = lex->GetToken();
        errors+= stmt("");
        errors+= stmt("");
    }
    else if (token == MODULO_T ) {
        //rules with tokens and two stmt after it
        tokenActionsFile << "Using Rule 40\n";
        token = lex->GetToken();
        errors+= stmt("");
        errors+= stmt("");
    }
    else if (token == LISTOP_T || token == NOT_T || token == NUMBERP_T || token == LISTP_T || token == ZEROP_T ||
            token == NULLP_T || token == STRINGP_T || token == ROUND_T || token == DISPLAY_T) {
        //all rules with a token(see above) and one stmt after it
        if(token == LISTOP_T)
            tokenActionsFile << "Using Rule 26\n";
        else if(token == NOT_T )
            tokenActionsFile << "Using Rule 30\n";
        else if(token == NUMBERP_T){
	  //Writing to code Gen
	  cg->WriteCode(0,"numberp");
            tokenActionsFile << "Using Rule 31\n";
        }
        else if(token == LISTP_T )
            tokenActionsFile << "Using Rule 32\n";
        else if(token == ZEROP_T){
	  //Writing to code Gen
            cg->WriteCode(0, "zerop");
            tokenActionsFile << "Using Rule 33\n";
        }
        else if(token == NULLP_T )
            tokenActionsFile << "Using Rule 34\n";
        else if(token == STRINGP_T)
            tokenActionsFile << "Using Rule 35\n";
        else if(token == ROUND_T )
            tokenActionsFile << "Using Rule 41\n";
        else if(token == DISPLAY_T){
	  //Writing to code Gen
	  cg->WriteCode(1, "cout << ");
	}
	tokenActionsFile << "Using Rule 48\n";
        token = lex->GetToken();
        errors += stmt("");
    }
    else if (token == AND_T || token == OR_T || token == PLUS_T || token == MULT_T || token == EQUALTO_T ||
            token == GT_T || token == LT_T || token == GTE_T || token == LTE_T || token == IDENT_T ) {
        //all rules with a token(see above) and one stmt_list after it
        string _operator;
        if(token == AND_T)
            tokenActionsFile << "Using Rule 28\n";
        else if(token == OR_T )
            tokenActionsFile << "Using Rule 29\n";
        else if(token == PLUS_T){
	  //writing to code gen
            cg->WriteCode(0, check);
            tokenActionsFile << "Using Rule 36\n";
        }
        else if(token == MULT_T ){
	  //writint to code gen
            cg->WriteCode(0, check);
            tokenActionsFile << "Using Rule 39\n";
        }
        else if(token == EQUALTO_T)
            tokenActionsFile << "Using Rule 42\n";
        else if(token == GT_T )
            tokenActionsFile << "Using Rule 43\n";
        else if(token == LT_T)
            tokenActionsFile << "Using Rule 44\n";
        else if(token == GTE_T )
            tokenActionsFile << "Using Rule 45\n";
        else if(token == LTE_T)
            tokenActionsFile << "Using Rule 46\n";
        else if(token == IDENT_T){
	  //writing to code gen
            cg->WriteCode(0, lex->GetLexeme() + " (");
            _operator = " );\n";
            tokenActionsFile << "Using Rule 47\n";
        }
        if(token != IDENT_T)
            _operator = lex->GetLexeme();
        token = lex->GetToken();
        errors+= stmt_list(_operator);
    }
    else if ( token == MINUS_T ) {
        tokenActionsFile << "Using Rule 37\n";
        token = lex->GetToken();
	//writing to code gen
        cg->WriteCode(0, check);
        errors+= stmt("");
	//writing to code gen
        cg->WriteCode(0, " -");
        errors+= stmt_list("");
    }
    else if ( token == DIV_T ) {
        tokenActionsFile << "Using Rule 38\n";
        token = lex->GetToken();
	//writing to code gen
        cg->WriteCode(0, check);
        errors+= stmt("");
	//writing to code gen
        cg->WriteCode(0, " /");
        errors+= stmt_list("");
    }
    else if ( token == IF_T ) {
        tokenActionsFile << "Using Rule 24\n";
	//writing to code gen
        cg->WriteCode(1, lex->GetLexeme()+"(");
        token = lex->GetToken();
        errors+= stmt("");
	//writing to code gen
        cg->WriteCode(0, ")\n");
        cg->WriteCode(1, "{\n");
        errors+= stmt("return");
        cg->WriteCode(0, ";\n}\n");
        errors+= else_part();
        cg->WriteCode(0, ";\n}\n");
    }
    else if (token == NEWLINE_T) {
        tokenActionsFile << "Using Rule 49\n";
	//writing to code gen
        cg->WriteCode(1, "cout << endl;\n");
        token = lex->GetToken();
    }
    else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << "\n";
    return errors;
}
