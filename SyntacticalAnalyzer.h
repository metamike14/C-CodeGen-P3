#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include "LexicalAnalyzer.h"
#include "CodeGenerator.h"

using namespace std;

class SyntacticalAnalyzer 
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    int program();
    int more_defines();
    int define();
    int stmt_list();
    int stmt();
    int literal();
    int quoted_list();
    int more_tokens();
    int param_list();
    int else_part();
    int stmt_pair();
    int stmt_pair_body();
    int action();
    int any_other_token();

    private:
	LexicalAnalyzer * lex;
    CodeGen * cg;
	token_type token;
    ofstream tokenActionsFile;




};
	
#endif
