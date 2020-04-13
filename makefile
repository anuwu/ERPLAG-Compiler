output : astDriver.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTableDriver.o lexer.h typeChecker.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h
	gcc -o ast.exe astDriver.o ast.o lexTest.o lexer.o parser.o
	gcc -o ST.exe symbolTableDriver.o ast.o lexTest.o lexer.o parser.o symbolTable.o typeChecker.o
	make clean

symbolTable.o : symbolTable.c symbolTable.h
	gcc -c symbolTable.c

clean :
	rm *.o