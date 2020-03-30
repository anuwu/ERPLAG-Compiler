#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"

#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule


int lineBeforeAsterisk ;
char charBeforeAsterisk ;


char* tokenIDToString (tokenID id)
{
	switch (id)
	{
		case TK_REAL :
			return "TK_REAL" ;
		case TK_INTEGER :
			return "TK_INTEGER" ;
		case TK_BOOLEAN :
			return "TK_BOOLEAN" ;
		case TK_ID :
			return "TK_ID" ;
		case TK_NUM :
			return "TK_NUM" ;
		case TK_RNUM :
			return "TK_RNUM" ;
		case TK_OF :
			return "TK_OF" ;
		case TK_ARRAY :
			return "TK_ARRAY" ;
		case TK_START :
			return "TK_START" ;
		case TK_END :
			return "TK_END" ;
		case TK_DECLARE :
			return "TK_DECLARE" ;
		case TK_MODULE :
			return "TK_MODULE" ;
		case TK_DRIVER :
			return "TK_DRIVER" ;
		case TK_PROGRAM :
			return "TK_PROGRAM" ;
		case TK_GET_VALUE :
			return "TK_GET_VALUE" ;
		case TK_PRINT :
			return "TK_PRINT" ;
		case TK_USE :
			return "TK_USE" ;
		case TK_WITH :
			return "TK_WITH" ;
		case TK_PARAMETERS :
			return "TK_PARAMETERS" ;
		case TK_TRUE :
			return "TK_TRUE" ;
		case TK_FALSE :
			return "TK_FALSE" ;
		case TK_TAKES :
			return "TK_TAKES" ;
		case TK_INPUT :
			return "TK_INPUT" ;
		case TK_RETURNS :
			return "TK_RETURNS" ;
		case TK_AND :
			return "TK_AND" ;
		case TK_OR :
			return "TK_OR" ;
		case TK_FOR :
			return "TK_FOR" ;
		case TK_IN :
			return "TK_IN" ;
		case TK_SWITCH:
			return "TK_SWITCH" ;
		case TK_CASE :
			return "TK_CASE" ;
		case TK_BREAK :
			return "TK_BREAK" ;
		case TK_DEFAULT :
			return "TK_DEFAULT" ;
		case TK_WHILE :
			return "TK_WHILE" ;
		case TK_PLUS :
			return "TK_PLUS" ;
		case TK_MINUS :
			return "TK_MINUS" ;
		case TK_MUL :
			return "TK_MUL" ;
		case TK_DIV :
			return "TK_DIV" ;
		case TK_LT :
			return "TK_LT" ;
		case TK_LE :
			return "TK_LE" ;
		case TK_GE :
			return "TK_GE" ;
		case TK_GT :
			return "TK_GT" ;
		case TK_EQ :
			return "TK_EQ" ;
		case TK_NE :
			return "TK_NE" ;
		case TK_DEF :
			return "TK_DEF" ;
		case TK_ENDDEF :
			return "TK_ENDDEF" ;
		case TK_DRIVERDEF :
			return "TK_DRIVERDEF" ;
		case TK_DRIVERENDDEF :
			return "TK_DRIVERENDDEF" ;
		case TK_COLON :
			return "TK_COLON" ;
		case TK_RANGEOP :
			return "TK_RANGEOP" ;
		case TK_SEMICOL :
			return "TK_SEMICOL" ;
		case TK_COMMA :
			return "TK_COMMA" ;
		case TK_ASSIGNOP :
			return "TK_ASSIGNOP" ;
		case TK_SQBO:
			return "TK_SQBO" ;
		case TK_SQBC :
			return "TK_SQBC" ;
		case TK_BO :
			return "TK_BO" ;
		case TK_BC :
			return "TK_BC" ;
	}
}

extern int printNextToken (twinBuffer* twinBuf);
extern int printNextTokenCOMMENT (twinBuffer *twinBuf);

twinBuffer *twinBuf ;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}
	FILE* outfile ;
	outfile = fopen (argv[2] , "w") ;

	astTest () ;
	treeNode *root = parseTree (argv[1]) ;
	inorderTraversal(root, outfile) ;

	printf ("ID of root = %d\n", root->tnt.nonTerm) ;

	return 0;
}