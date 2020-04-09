output : astDriver.o ast.o lexTest.o lexer.o parser.o lexer.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h
	gcc -o ST.exe astDriver.o ast.o lexTest.o lexer.o parser.o symbolTable.o
	make clean

symbolTable.o : symbolTable.c symbolTable.h
	gcc -c symbolTable.c

clean :
	rm *.o