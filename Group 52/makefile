output : finalDriver.o codegen.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTable.o parser.h parserDef.h symbolTable.h codegen.h lexer.h typeChecker.h lexerDef.h 
	gcc -o compiler finalDriver.o codegen.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTable.o

erplag.o : erplag.c
	gcc -c erplag.c

codegen.o : codegen.c
	gcc -c codegen.c

astDriver.o : astDriver.c
	gcc -c astDriver.c

ast.o : ast.c
	gcc -c ast.c

typeChecker.o : typeChecker.c
	gcc -c typeChecker.c

parser.o : parser.c
	gcc -c parser.c

lexTest.o : lexTest.c
	gcc -c lexTest.c

lexer.o : lexer.c
	gcc -c lexer.c

symbolTableDriver.o : symbolTableDriver.c
	gcc -c symbolTableDriver.c

clean :
	rm *.o
	rm *.exe