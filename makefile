P2.out : Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o
	g++ -g -o P2.out Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o

Project2.o : Project2.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project2.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

LexicalAnalyzer.o : LexicalAnalyzer.cpp LexicalAnalyzer.h
	g++ -g -c LexicalAnalyzer.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c SyntacticalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h
	g++ -g -c CodeGenerator.cpp

clean : 
	rm *.o P2.out 

submit : Project2.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile README.txt
	rm -rf TeamTP2
	mkdir TeamTP2
	cp Project2.cpp TeamTP2
	cp LexicalAnalyzer.h TeamTP2
	cp LexicalAnalyzer.cpp TeamTP2
	cp SyntacticalAnalyzer.h TeamTP2
	cp SyntacticalAnalyzer.cpp TeamTP2
	cp makefile TeamTP2
	cp README.txt TeamTP2
	tar cfvz TeamTP2.tgz TeamTP2
	cp TeamTP2.tgz ~tiawatts/cs460drop
