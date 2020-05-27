@echo off
set grmPath=%CD%
set grmPath=%grmPath%\parser.c
gcc -w -g -c erplag.c
gcc -w -g -c error.c
gcc -w -g -c codegen.c
gcc -w -g -c symbolTable.c
gcc -w -g -c ast.c
gcc -w -g -c typeChecker.c
gcc -w -g -c lexer.c
gcc -w -g -c "%grmPath%"
gcc -w -g -o .compiler erplag.o codegen.o ast.o lexer.o parser.o symbolTable.o typeChecker.o error.o

del erplag.o
del error.c
del codegen.o
del symbolTable.o
del ast.o
del typeChecker.o
del lexer.o
del parser.c