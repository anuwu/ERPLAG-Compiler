output : erplag.o codegen.o ast.o typeChecker.o lexer.o parser.o symbolTable.o error.o lexer.h typeChecker.h lexerDef.h parser.h parserDef.h symbolTable.h codegen.h error.h
	gcc -o compiler erplag.o codegen.o ast.o lexer.o parser.o symbolTable.o typeChecker.o error.o
	
allExec : erplag.o codegen.o astDriver.o ast.o typeChecker.o lexer.o parser.o symbolTableDriver.o error.o lexer.h typeChecker.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h codegen.h error.h
	gcc -o ast astDriver.o ast.o lexer.o parser.o error.o
	gcc -o ST symbolTableDriver.o ast.o lexer.o parser.o symbolTable.o typeChecker.o error.o
	gcc -o compiler erplag.o codegen.o ast.o lexer.o parser.o symbolTable.o typeChecker.o error.o

erplag.o : erplag.c
	gcc -c erplag.c

error.o : error.c
	gcc -c error.c

codegen.o : codegen.c
	gcc -c codegen.c

astDriver.o : astDriver.c
	gcc -c astDriver.c

ast.o : ast.c
	gcc -c ast.c

typeChecker.o : typeChecker.c
	gcc -c typeChecker.c

parser.o : parser.c
	./parser.sh

lexer.o : lexer.c
	gcc -c lexer.c

symbolTableDriver.o : symbolTableDriver.c
	gcc -c symbolTableDriver.c

asm :
	make -f make.asm FILE=$(FILE)

erp :
	make
	./compiler $(FILE).erp $(FILE).asm

erpAsm :
	make erp FILE=$(FILE)
	make asm FILE=$(FILE)

erpExec :
	make erpAsm FILE=$(FILE)
	./$(FILE)

clean :
	rm -f *.o
	rm -f compiler
	rm -f ST
	rm -f ast

exeClean :
	rm -f compiler
	rm -f ST
	rm -f ast

objClean :
	rm -f *.o
