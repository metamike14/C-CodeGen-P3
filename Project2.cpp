//Authors: Michael Schmidt, Eduardo Roman, Nicholas Ivanoff
//Assignment: Project 3

#include <cstdlib>
#include <iostream>
#include <iomanip>
//#include "SetLimits.h"
#include "SyntacticalAnalyzer.h"

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf ("format: proj1 <filename>\n");
		exit (1);
	}
	//SetLimits ();
	SyntacticalAnalyzer parser (argv[1]);

	return 0;
}
