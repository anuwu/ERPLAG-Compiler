output : codegen.o astDriver.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTableDriver.o lexer.h typeChecker.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h
	gcc -o codegen.exe codegen.o ast.o lexTest.o lexer.o parser.o symbolTable.o typeChecker.o
	make clean

clean :
	rm *.o