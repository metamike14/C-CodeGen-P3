#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <fstream>

using namespace std;

enum token_type {NONE = 99, EOF_T, IDENT_T, NUMLIT_T, STRLIT_T, LISTOP_T, CONS_T, IF_T, COND_T, ELSE_T,
    DISPLAY_T, NEWLINE_T, AND_T, OR_T, NOT_T, DEFINE_T, NUMBERP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T,
    PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, LPAREN_T,
    RPAREN_T, SQUOTE_T, ERROR_T, NUM_TOKENS, ERROR_D};

class LexicalAnalyzer 
{
    public:
	LexicalAnalyzer (char * filename);
	~LexicalAnalyzer ();
	token_type GetToken ();
	string GetTokenName (token_type t) const;
	string GetLexeme () const;
    void ReportSyntactErrors(const string & msg);
	void ReportError (const string & msg);
	void FinalErrorReport(int errors);
    int GetCol(const char &symbol);
    token_type CheckKeyworks(const string &lex);

    private:
	ifstream input; //read input
	ofstream listingFile; //file .lst
	ofstream tokenFile; //file .p1
	ofstream debugFile; //file .dbg
	token_type token;
	string line;
	int linenum;
	int pos;
	string lexeme;
	int errors;
    int table[13][21] = {
            {3,	1,	3,	3,	3,	ERROR_T,	6,	4,	5,	7,	9,	DIV_T,	MULT_T,	EQUALTO_T,	10,	11,	LPAREN_T,	RPAREN_T,	SQUOTE_T,	ERROR_T,	ERROR_T},
            {3,	3,	2,	2,	3,	3,	3,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T},
            {3,	3,	3,	2,	12,	3,	3,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T},
            {3,	3,	3,	3,	3,	3,	3,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T,	IDENT_T},
            {PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	6,	PLUS_T,	PLUS_T,	7,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T,	PLUS_T},
            {MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	6,	MINUS_T,	MINUS_T,	7,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T,	MINUS_T},
            {NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	6,	NUMLIT_T,	NUMLIT_T,	8,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T},
            {ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	8,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D,	ERROR_D},
            {NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	8,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T,	NUMLIT_T},
            {9,	9,	9,	9,	9,	9,	9,	9,	9,	9,	STRLIT_T,	9,	9,	9,	9,	9,	9,	9,	9,	9,	9},
            {GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GTE_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T,	GT_T},
            {LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LTE_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T,	LT_T},
            {3,	3,	3,	3,	3,	3,	3,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T,	LISTOP_T}

    };
};
	
#endif
