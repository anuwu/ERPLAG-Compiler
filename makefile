output : driver.o lexTest.o lexer.o parser.o lexer.h lexerDef.h parser.h parserDef.h
	gcc -o stage1.exe driver.o lexTest.o lexer.o parser.o
	make clean

clean :
	rm *.o