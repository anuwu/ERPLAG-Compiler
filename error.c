#include <stdio.h>
#include "error.h"

#ifdef _WIN64
#endif
#if defined __linux__ || defined __MACH__
#endif

void errHead ()
{
	#ifdef _WIN64
		printf ("ERPLAG : ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_CYAN "ERPLAG : " ANSI_RESET) ;
	#endif
}

void errFatal ()
{
	errHead () ;

	#ifdef _WIN64
		printf ("FATAL ERROR ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_RED "FATAL ERROR " ANSI_RESET) ;
	#endif

	errArrow () ;
}

void errLine (int line)
{
	#ifdef _WIN64
		printf ("%d", line) ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET, line) ;
	#endif
}

void errLex ()
{
	errHead () ;
	#ifdef _WIN64
		printf ("Lexical Error ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_RED "Lexical Error " ANSI_RESET) ;
	#endif
}

void errParse ()
{
	errHead () ;

	#ifdef _WIN64
		printf ("Parsing Error ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_MAGENTA "Parsing Error " ANSI_RESET) ;
	#endif
}

void errSemantic ()
{
	errHead () ;
	#ifdef _WIN64
		printf ("Semantic Error ") ;	
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_YELLOW "Semantic Error " ANSI_RESET) ;	
	#endif
	errArrow () ;
}

void errWarning ()
{
	errHead () ;
	#ifdef _WIN64
		printf ("Warning ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD ANSI_BLUE "Warning " ANSI_RESET) ;
	#endif
	
	errArrow () ;
}

void errArrow ()
{
	#ifdef _WIN64
		printf ("--> ") ;
	#endif
	#if defined __linux__ || defined __MACH__
		printf (ANSI_BOLD "--> " ANSI_RESET) ;
	#endif
}