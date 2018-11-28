#include <iomanip>
#include <cstdlib>
#include "LexicalAnalyzer.h"

using namespace std;

static string token_names[] = {	"EOF_T", "IDENT_T", "NUMLIT_T", "STRLIT_T", "LISTOP_T", "CONS_T", "IF_T", "COND_T",
								   "ELSE_T", "DISPLAY_T", "NEWLINE_T", "AND_T", "OR_T", "NOT_T","DEFINE_T", "NUMBERP_T",
								   "LISTP_T", "ZEROP_T", "NULLP_T", "STRINGP_T", "PLUS_T", "MINUS_T", "DIV_T", "MULT_T",
								   "MODULO_T", "ROUND_T", "EQUALTO_T", "GT_T", "LT_T", "GTE_T", "LTE_T", "LPAREN_T",
								   "RPAREN_T", "SQUOTE_T", "ERROR_T" };

LexicalAnalyzer::LexicalAnalyzer (char * filename)
{
	// This function will initialize the lexical analyzer class
	input.open(filename);

	string temp = filename;
	temp.replace(temp.end()-2, temp.end(), "lst");
    listingFile.open(temp);

    getline(input,line);
    linenum = 1;
    pos = 0;
    errors = 0;
    lexeme = "";

    listingFile << "Input file: " << filename << endl;
    listingFile << setw(3) << linenum << ": " << line << endl;

}

LexicalAnalyzer::~LexicalAnalyzer ()
{
	// This function will complete the execution of the lexical analyzer class
}

int LexicalAnalyzer::GetCol(const char &symbol) {
	if(isalpha(symbol)) {
		if (symbol == 'c') {
			return 1;
		} else if (symbol == 'a') {
			return 2;
		} else if (symbol == 'd'){
			return 3;
		} else if (symbol == 'r'){
			return 4;
		}else
			return 0;
	}
	else if(symbol == '_')
		return 5;
	else if(isdigit(symbol))
		return 6;
	else if(symbol == '+')
		return 7;
	else if(symbol == '-')
		return 8;
	else if(symbol == '.')
		return 9;
	else if(symbol == '"')
		return 10;
	else if(symbol == '/')
		return 11;
	else if(symbol == '*')
		return 12;
	else if(symbol == '=')
		return 13;
	else if(symbol == '>')
		return 14;
	else if(symbol == '<')
		return 15;
	else if(symbol == '(')
		return 16;
	else if(symbol == ')')
		return 17;
	else if(symbol == '\'')
		return 18;
	else if(symbol == '?')
		return 19;
	else
		return 20;
}

token_type LexicalAnalyzer::CheckKeyworks(const string &lex) {
	if (lex == "cons")
		return CONS_T;
	else if (lex == "if")
		return IF_T;
	else if (lex == "cond")
		return COND_T;
	else if (lex == "else")
		return ELSE_T;
	else if (lex == "display")
		return DISPLAY_T;
	else if (lex == "newline")
		return NEWLINE_T;
	else if (lex == "and")
		return AND_T;
	else if (lex == "or")
		return OR_T;
	else if (lex == "not")
		return NOT_T;
	else if (lex == "define")
		return DEFINE_T;
	else if (lex == "number?")
		return NUMBERP_T;
	else if (lex == "list?")
		return LISTP_T;
	else if (lex == "zero?")
		return ZEROP_T;
	else if (lex == "null?")
		return NULLP_T;
	else if (lex == "string?")
		return STRINGP_T;
	else if (lex == "modulo")
		return MODULO_T;
	else if (lex == "round")
		return ROUND_T;
	else
		return IDENT_T;

}

token_type LexicalAnalyzer::GetToken ()
{
	// This function will find the next lexeme int the input file and return
	// the token_type value associated with that lexeme
    lexeme = "";
	//cout << line.size() << endl;
	while( pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
		pos++;

    while(pos > line.size()-1 || line.empty()|| line[pos] == '\n' || line[pos] == '\r' || line[pos] == '\0'){
        if(getline(input,line)){
            linenum++;
            listingFile << setw(3) << linenum << ": " << line << endl;
            pos = 0;
        }
        else{
            listingFile << setw(3) << linenum+1 << ": " << endl;
            return EOF_T;
        }
    }

    while(line[pos] == ' ' || line[pos] == '\t')
        pos++;

	int currentState = 0;
	int col = 0;
	while(currentState < 100){
        if(currentState == 9 && pos > line.size()){
            token = ERROR_T;
            ReportError(lexeme);
            return token;
        }
		col = GetCol(line[pos]);
		//cout << line[pos] << ' ' << int(line[pos]) << endl;
		lexeme += line[pos];
		currentState = table[currentState][col];
		pos++;
	}
    if(currentState == GT_T || currentState == LT_T || currentState == PLUS_T || currentState == MINUS_T ||
            currentState == NUMLIT_T ||currentState == LISTOP_T) {
        lexeme.pop_back();
        pos--;
    }
	if(currentState == IDENT_T){
        if(col != 19){
            lexeme.pop_back();
            pos--;
        }
		token = CheckKeyworks(lexeme);
		if(token == IDENT_T && col == 19){
			lexeme.pop_back();
			token = CheckKeyworks(lexeme);
			pos--;
		}
	}
    else if(currentState == ERROR_D){
        lexeme.pop_back();
        pos--;
        token = ERROR_T;
    }
	else
		token = token_type (currentState);
    if (token == ERROR_T)
        ReportError(lexeme);
	return token;
}

string LexicalAnalyzer::GetTokenName (token_type t) const
{
	// The GetTokenName function returns a string containing the name of the
	// token passed to it. 
	return token_names[t-100];
}

string LexicalAnalyzer::GetLexeme () const
{
	// This function will return the lexeme found by the most recent call to 
	// the get_token function
	return lexeme;
}

void LexicalAnalyzer::ReportError (const string & msg)
{
	// This function will be called to write an error message to a file
    listingFile << "Error at " << linenum << "," << pos << ": Invalid character(s) found: " << lexeme << endl;
    errors++;
}

void LexicalAnalyzer::ReportSyntactErrors(const string &msg) {
    listingFile << "Error at " << linenum << "," << pos << ": " << msg  << endl;
    errors++;

}

void LexicalAnalyzer::FinalErrorReport(int errors) {
    listingFile << errors << " errors found in input file" << endl;
}
