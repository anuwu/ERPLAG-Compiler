output : astDriver.o ast.o lexTest.o lexer.o parser.o lexer.h lexerDef.h parser.h parserDef.h
	gcc -o ast.exe astDriver.o ast.o lexTest.o lexer.o parser.o
	make clean

clean :
	rm *.o