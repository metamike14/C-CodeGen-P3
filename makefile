P3.out : Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o
	g++ -g -o P3.out Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o

Project3.o : Project3.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project3.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

LexicalAnalyzer.o : LexicalAnalyzer.cpp LexicalAnalyzer.h
	g++ -g -c LexicalAnalyzer.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c SyntacticalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h
	g++ -g -c CodeGenerator.cpp

clean : 
	rm *.o P3.out 

submit : Project3.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile README.txt
	rm -rf Team9P2
	mkdir Team9P2
	cp Project3.cpp Team9P2
	cp LexicalAnalyzer.h Team9P2
	cp LexicalAnalyzer.cpp Team9P2
	cp SyntacticalAnalyzer.h Team9P2
	cp SyntacticalAnalyzer.cpp Team9P2
	cp CodeGenerator.h Team9P2
	cp CodeGenerator.cpp Team9P2	
	cp makefile Team9P2
	cp README.txt Team9P2
	tar cfvz Team9P2.tgz TeamTP2
	cp Team9P2.tgz ~tiawatts/cs460drop
