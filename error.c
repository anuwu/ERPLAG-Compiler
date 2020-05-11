#include <stdio.h>
#include "error.h"

void errHead ()
{
	printf (ANSI_BOLD ANSI_CYAN "ERPLAG : " ANSI_RESET) ;
}

void errFatal ()
{
	errHead () ;
	printf (ANSI_BOLD ANSI_RED "FATAL ERROR " ANSI_RESET) ;
	errArrow () ;
}

void errLine (int line)
{
	printf (ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET, line) ;
}

void errLex ()
{
	errHead () ;
	printf (ANSI_BOLD ANSI_RED "Lexical Error " ANSI_RESET) ;
}

void errParse ()
{
	errHead () ;
	printf (ANSI_BOLD ANSI_MAGENTA "Parsing Error " ANSI_RESET) ;
}

void errSemantic ()
{
	errHead () ;
	printf (ANSI_BOLD ANSI_YELLOW "Semantic Error " ANSI_RESET) ;
	errArrow () ;
}

void errArrow ()
{
	printf (ANSI_BOLD "--> " ANSI_RESET) ;
}