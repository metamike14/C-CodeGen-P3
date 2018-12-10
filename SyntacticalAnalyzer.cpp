#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include "SyntacticalAnalyzer.h"

using namespace std;

token_type otherTokens[] = {IDENT_T, NUMLIT_T, STRLIT_T, CONS_T, IF_T, DISPLAY_T, NEWLINE_T, LISTOP_T,
                            AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T,NULLP_T, STRINGP_T, PLUS_T, MINUS_T,
                            DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, COND_T, ELSE_T, LPAREN_T, SQUOTE_T, NONE};

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

SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
	delete lex;
    delete cg;
}


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

int SyntacticalAnalyzer::define() {
    tokenActionsFile << "Entering Define function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == DEFINE_T){
        tokenActionsFile << "Using Rule 4\n";
        token = lex->GetToken(); // get next token after 'define'
        if(token == LPAREN_T){
            token = lex->GetToken(); // get next token after '('
            if(token == IDENT_T){
                cg->WriteCode(0,"Object " + lex->GetLexeme() + "(");
                token = lex->GetToken(); // get next token after 'ident'
                errors +=param_list();
                if(token == RPAREN_T){
                    cg->WriteCode(0, ")\n{\n");
                    token = lex->GetToken(); // get next token after ')'
                    errors += stmt();
                    errors += stmt_list();
                    if(token == RPAREN_T){
                        cg->WriteCode(0, "}\n\n");
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
        errors += stmt_list();
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

int SyntacticalAnalyzer::stmt_list() {
    tokenActionsFile << "Entering Stmt_List function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T ||
            token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 5\n";
        errors += stmt();
        errors += stmt_list();
    }else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 6\n";
    }else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Stmt_List function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

int SyntacticalAnalyzer::stmt() {
    tokenActionsFile << "Entering Stmt function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T){
        tokenActionsFile << "Using Rule 9\n";
        token = lex->GetToken(); // get next token after '('
        errors += action();
        if(token == RPAREN_T){
            token = lex->GetToken();
        } else{
            lex->ReportSyntactErrors("RPAREN_T expected");
            errors++;
        }
    }else if(token == IDENT_T) {
        tokenActionsFile << "Using Rule 8\n";
        cg->WriteCode(1, "return " + lex->GetLexeme());
        token = lex->GetToken(); // get next token after 'ident'
    }else if(token == NUMLIT_T || token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 7\n";
        errors += literal();
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Stmt function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

int SyntacticalAnalyzer::literal() {
    tokenActionsFile << "Entering Literal function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == NUMLIT_T){
        tokenActionsFile << "Using Rule 10\n";
        token = lex->GetToken();

    }else if(token == STRLIT_T){
        tokenActionsFile << "Using Rule 11\n";
        token = lex->GetToken();

    }else if(token == SQUOTE_T){
        tokenActionsFile << "Using Rule 12\n";
        token = lex->GetToken();
        errors+= quoted_list();
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Literal function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

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

int SyntacticalAnalyzer::param_list() {
    tokenActionsFile << "Entering Param_List function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == IDENT_T){
        tokenActionsFile << "Using Rule 16\n";
        cg->WriteCode(0, "Object " + lex->GetLexeme());
        token = lex->GetToken();
        if(token != RPAREN_T)
            cg->WriteCode(0, ", ");
        errors += param_list();
    }else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 17\n";
        //Use rule 17
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Param_List function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

int SyntacticalAnalyzer::else_part() {
    tokenActionsFile << "Entering Else_Part function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T ||
       token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 18\n";
        errors += stmt();
    }else if(token == RPAREN_T){
        tokenActionsFile << "Using Rule 19\n";
        //Use rule 19
    } else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Else_Part function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

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

/*int SyntacticalAnalyzer::stmt_pair_body() {
    tokenActionsFile << "Entering Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n;
    int errors = 0;
    if(token == ELSE_T){
        tokenActionsFile << "Using Rule 23\n";
        token = lex->GetToken();
        errors += stmt();
        if(token == RPAREN_T){
            token = lex->GetToken();
        } else
            errors++;
    }else if(token == LPAREN_T || token == IDENT_T || token == NUMLIT_T ||
       token == STRLIT_T || token == SQUOTE_T){
        tokenActionsFile << "Using Rule 22\n";
        errors += stmt();
        errors += stmt();
        if(token == RPAREN_T){
            token = lex->GetToken();
            errors += stmt_pair();
        } else
            errors++;

    } else
        errors++;
    tokenActionsFile << "Exiting Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) <<'\n;
    return errors;
}*/


int SyntacticalAnalyzer::stmt_pair_body()
{
    tokenActionsFile << "Entering Stmt_Pair_Body function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == ELSE_T)
    {
        tokenActionsFile << "Using Rule 23\n";
        token = lex->GetToken();
        errors += stmt();
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
        errors += stmt();
        errors += stmt();
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


int SyntacticalAnalyzer::any_other_token()
{
    tokenActionsFile << "Entering Any_Other_Token function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if(token == LPAREN_T)
    {
        tokenActionsFile << "Using Rule 50\n";
        token = lex->GetToken();
        errors += more_tokens();
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
        token = lex->GetToken();
        errors += any_other_token();
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
    }

    else
    {
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors ++;
    }
    tokenActionsFile << "Exiting Any_Other_Token function; current token is: " << lex->GetTokenName(token) <<'\n';
    return errors;
}

/*int SyntacticalAnalyzer::action() {
    tokenActionsFile << "Entering Action function; current token is: " << lex->GetTokenName(token) << ", lexeme: " << lex->GetLexeme() <<'\n';
    int errors = 0;
    if (token == IF_T) {
        tokenActionsFile << "Using Rule 24\n";
        token = lex->GetToken();
        errors += stmt();
        errors += stmt();
        errors += else_part();
    } else if (token == COND_T) {
        tokenActionsFile << "Using Rule 25\n";
        token = lex->GetToken();
        if (token == LPAREN_T) {
            errors += stmt_pair_body();
        } else{
            errors++;
        }
    } else if (token == LISTOP_T || token == NOT_T || token == NUMBERP_T || token == LISTP_T || token == ZEROP_T ||
               token == NULLP_T || token == STRINGP_T || token == ROUND_T || token == DISPLAY_T) {
        //all rules with a token(see above) and one stmt after it
        tokenActionsFile << "Using Rule 26, 30, 31, 32, 33, 34, 35, 41, or 48\n";
        token = lex->GetToken();
        errors += stmt();
    } else if (token == CONS_T || token == MODULO_T) {
        //rules with tokens and two stmt after it
        tokenActionsFile << "Using Rule 27 or 40\n";
        token = lex->GetToken();
        errors += stmt();
        errors += stmt();
    } else if (token == AND_T || token == OR_T || token == PLUS_T || token == MULT_T || token == EQUALTO_T ||
               token == GT_T || token == GTE_T || token == LTE_T || token == LT_T || token == IDENT_T) {
        //all rules with a token(see above) and one stmt_list after it
        tokenActionsFile << "Using Rule 28, 29, 36, 39, 42, 43, 44, 45, 46, or 47\n";
        token = lex->GetToken();
        errors += stmt_list();
    } else if (token == MINUS_T || token == DIV_T) {
        tokenActionsFile << "Using Rule 37 or 38\n";
        //rules with tokens and one stmt & one stmt_list after it
        token = lex->GetToken();
        errors += stmt();
        errors += stmt_list();
    } else if (token == NEWLINE_T) {
        tokenActionsFile << "Using Rule 49\n";
        token = lex->GetToken();
    } else
        errors++;
    tokenActionsFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << '\n';
    return errors;
}*/


int SyntacticalAnalyzer::action() {
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
        errors+= stmt();
        errors+= stmt();
    }
    else if (token == MODULO_T ) {
        //rules with tokens and two stmt after it
        tokenActionsFile << "Using Rule 40\n";
        token = lex->GetToken();
        errors+= stmt();
        errors+= stmt();
    }
    else if (token == LISTOP_T || token == NOT_T || token == NUMBERP_T || token == LISTP_T || token == ZEROP_T ||
            token == NULLP_T || token == STRINGP_T || token == ROUND_T || token == DISPLAY_T) {
        //all rules with a token(see above) and one stmt after it
        if(token == LISTOP_T)
            tokenActionsFile << "Using Rule 26\n";
        else if(token == NOT_T )
            tokenActionsFile << "Using Rule 30\n";
        else if(token == NUMBERP_T)
            tokenActionsFile << "Using Rule 31\n";
        else if(token == LISTP_T )
            tokenActionsFile << "Using Rule 32\n";
        else if(token == ZEROP_T)
            tokenActionsFile << "Using Rule 33\n";
        else if(token == NULLP_T )
            tokenActionsFile << "Using Rule 34\n";
        else if(token == STRINGP_T)
            tokenActionsFile << "Using Rule 35\n";
        else if(token == ROUND_T )
            tokenActionsFile << "Using Rule 41\n";
        else if(token == DISPLAY_T)
            tokenActionsFile << "Using Rule 48\n";
        token = lex->GetToken();
        errors += stmt();
    }
    else if (token == AND_T || token == OR_T || token == PLUS_T || token == MULT_T || token == EQUALTO_T ||
            token == GT_T || token == LT_T || token == GTE_T || token == LTE_T || token == IDENT_T ) {
        //all rules with a token(see above) and one stmt_list after it
        if(token == AND_T)
            tokenActionsFile << "Using Rule 28\n";
        else if(token == OR_T )
            tokenActionsFile << "Using Rule 29\n";
        else if(token == PLUS_T)
            tokenActionsFile << "Using Rule 36\n";
        else if(token == MULT_T )
            tokenActionsFile << "Using Rule 39\n";
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
        else if(token == IDENT_T)
            tokenActionsFile << "Using Rule 47\n";
        token = lex->GetToken();
        errors+= stmt_list();
    }
    else if ( token == MINUS_T ) {
        tokenActionsFile << "Using Rule 37\n";
        token = lex->GetToken();
        errors+= stmt();
        errors+= stmt_list();
    }
    else if ( token == DIV_T ) {
        tokenActionsFile << "Using Rule 38\n";
        token = lex->GetToken();
        errors+= stmt();
        errors+= stmt_list();
    }
    else if ( token == IF_T ) {
        tokenActionsFile << "Using Rule 24\n";
        token = lex->GetToken();
        errors+= stmt();
        errors+= stmt();
        errors+= else_part();
    }
    else if (token == NEWLINE_T) {
        tokenActionsFile << "Using Rule 49\n";
        token = lex->GetToken();
    }
    else{
        lex->ReportSyntactErrors(lex->GetTokenName(token) + " unexpected");
        errors++;
    }
    tokenActionsFile << "Exiting Action function; current token is: " << lex->GetTokenName(token) << "\n";
    return errors;
}
