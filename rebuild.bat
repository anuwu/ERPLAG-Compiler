@echo off
setlocal EnableDelayedExpansion

if exist .compiler (
	set /p yn=Proceed with re-building ERPLAG compiler? ERPLAG compiler? [y/n] : 

	:rst
	set valyn=0

	IF "!yn!"=="y" (
		set valyn=1
		goto rbld
	)
	IF "!yn!"=="Y" (
		set valyn=1
		goto rbld
	)
	IF "!yn!"=="n" (
		EXIT /B 0
	)
	IF "!yn!"=="N" (
		EXIT /B 0
	)

	IF !valyn!==0 (
		set /p yn=Please re-enter correct option [y/n] : 
		goto rst
	)

	:rbld
	echo Rebuilding ERPLAG compiler^.^.^.

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
	del error.o
	del codegen.o
	del symbolTable.o
	del ast.o
	del typeChecker.o
	del lexer.o
	del parser.o

	echo.
	echo ERPLAG compiler re-build OK^^!
	EXIT /B 0
) 

echo ERPLAG compiler is not installed!^^!
echo Please install the compiler by running "install" before using "rebuild"
EXIT /B 1
