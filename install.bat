@echo off
setlocal EnableDelayedExpansion

set die=0
where >nul 2>nul gcc
if %ERRORLEVEL% NEQ 0 (
	echo Please ensure gcc is installed and added to path before attempting to install ERPLAG compiler	
	set die=1
)

where nasm /f > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
	echo Please ensure nasm is installed and added to path before attempting to install ERPLAG compiler
	set die=1
)

IF !die!==1 (
	echo ERPLAG compiler installation failed^!
	EXIT /B 1
)

if exist .compiler (
	echo ERPLAG compiler is already installed in this system 
	echo If you wish to uninstall it, execute "uninstall" in the command prompt

	EXIT /B 1
)

echo Installing ERPLAG compiler^.^.^.
echo.

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

if exist erplag.bat (
	del erplag.bat
)

rename .erplag erplag.bat

echo ERPLAG compiler has been successfully installed^^!
echo Please add this repository to the environment PATH variable
echo.
echo Use erplag -h to read the guidelines for using the compiler