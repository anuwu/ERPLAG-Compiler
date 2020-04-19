erplag : erplag.o codegen.o astDriver.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTableDriver.o lexer.h typeChecker.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h codegen.h
	gcc -o erplag.exe erplag.o codegen.o ast.o lexTest.o lexer.o parser.o symbolTable.o typeChecker.o
	./erplag.exe $(FILE).erp
	nasm -felf64 code.asm
	gcc -no-pie code.o

allExec : erplag.o codegen.o astDriver.o ast.o typeChecker.o lexTest.o lexer.o parser.o symbolTableDriver.o lexer.h typeChecker.h lexerDef.h symbolTable.o parser.h parserDef.h symbolTable.h codegen.h
	gcc -o ast.exe astDriver.o ast.o lexTest.o lexer.o parser.o
	gcc -o ST.exe symbolTableDriver.o ast.o lexTest.o lexer.o parser.o symbolTable.o typeChecker.o
	gcc -o erplag.exe erplag.o codegen.o ast.o lexTest.o lexer.o parser.o symbolTable.o typeChecker.o

asm :
	make -f make.asm
	make clean

clean :
	rm *.o
	rm *.exe