#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "lexer.h"

 
#define bufSize 128
 
char keywords[30][20] = 
{
	"integer",
	"real",
	"boolean",
	"of",
	"array",
	"start",
	"end",
	"declare",
	"module",
	"driver",
	"program",
	"get_value",
	"print",
	"use",
	"with",
	"parameters",
	"true",
	"false",
	"takes",
	"input",
	"returns",
	"AND",
	"OR",
	"for",
	"in",
	"switch",
	"case",
	"break",
	"default",
	"while"
} ;
 
tokenID keywordToTokenID (char *str)
{
	tokenID id ;
	if (strcmp(str,"integer")==0)
		id = TK_INTEGER ;
	else if (strcmp(str, "real")==0)
		id = TK_REAL ;
	else if (strcmp(str, "boolean")==0)
		id = TK_BOOLEAN ;
	else if (strcmp(str, "of")==0)
		id = TK_OF ;
	else if (strcmp(str, "array")==0)
		id = TK_ARRAY ;
	else if (strcmp(str, "start")==0)
		id = TK_START ;
	else if (strcmp(str, "end")==0)
		id = TK_END ;
	else if (strcmp(str, "declare")==0)
		id = TK_DECLARE ;
	else if (strcmp(str, "module")==0)
		id = TK_MODULE ;
	else if (strcmp(str, "driver")==0)
		id = TK_DRIVER ;
	else if (strcmp(str, "program")==0)
		id = TK_PROGRAM ;
	else if (strcmp(str, "get_value")==0)
		id = TK_GET_VALUE ;
	else if (strcmp(str, "print")==0)
		id = TK_PRINT ;
	else if (strcmp(str, "use")==0)
		id = TK_USE ;
	else if (strcmp(str, "with")==0)
		id = TK_WITH ;
	else if (strcmp(str, "parameters")==0)
		id = TK_PARAMETERS ;
	else if (strcmp(str, "true")==0)
		id = TK_TRUE ;
	else if (strcmp(str, "false")==0)
		id = TK_FALSE ;
	else if (strcmp(str, "takes")==0)
		id = TK_TAKES ;
	else if (strcmp(str, "input")==0)
		id = TK_INPUT ;
	else if (strcmp(str, "returns")==0)
		id = TK_RETURNS ;
	else if (strcmp(str, "AND")==0)
		id = TK_AND ;
	else if (strcmp(str, "OR")==0)
		id = TK_OR ;
	else if (strcmp(str, "for")==0)
		id = TK_FOR ;
	else if (strcmp(str, "in")==0)
		id = TK_IN ;
	else if (strcmp(str, "switch")==0)
		id = TK_SWITCH ;
	else if (strcmp(str, "case")==0)
		id = TK_CASE ;
	else if (strcmp(str, "break")==0)
		id = TK_BREAK ;
	else if (strcmp(str, "default")==0)
		id = TK_DEFAULT ;
	else if (strcmp(str, "while")==0)
		id = TK_WHILE ;
	else
		return (tokenID)-1 ;
 
	
	return id ;
}
 
twinBuffer* lexer_init (FILE *fp)
{
	int buf1_read, buf2_read ;
	twinBuffer *twinBuf = (twinBuffer *) malloc (sizeof(twinBuffer)) ;
	twinBuf->fp = fp ;
 
	if (twinBuf->fp == NULL)
		return NULL ;
 
	twinBuf->buf1 = (char *) malloc (sizeof(char) * (bufSize + 1)) ;
	twinBuf->buf2 = (char *) malloc (sizeof(char) * (bufSize + 1)) ;
 
	buf1_read = fread (twinBuf->buf1, sizeof(char), bufSize, twinBuf->fp) ;
	twinBuf->buf1[buf1_read] = EOF ;
	twinBuf->begin = twinBuf->buf1 ;
	twinBuf->forward = twinBuf->buf1 ;
	twinBuf->where_begin = TWIN_ONE ;
	twinBuf->where_forward = TWIN_ONE ;
 
	buf2_read = fread (twinBuf->buf2, sizeof(char) , bufSize, twinBuf->fp) ;
	twinBuf->buf2[buf2_read] = EOF ;
 
	twinBuf->lineNumber = 1 ;
 
	return twinBuf ;
}

void lexer_destroy (twinBuffer *twinBuf)
{
	free (twinBuf->buf1) ;
	free (twinBuf->buf2) ;
	fclose (twinBuf->fp) ;

	free (twinBuf) ;
}
 
int getLexemeLineNumber (twinBuffer *twinBuf)
{
	int line = twinBuf->lineNumber ;
 
	if (twinBuf->forward == twinBuf->buf1)
		return line ;
 
	if ((twinBuf->forward == twinBuf->buf2 && twinBuf->buf1[bufSize - 1] == '\n') || (*(twinBuf->forward - 1) == '\n') && *twinBuf->forward != EOF)
		return line - 1 ;
}
 
void resetBeginPointer (twinBuffer *twinBuf)
{
	if (twinBuf->where_begin == twinBuf->where_forward)
		twinBuf->begin = twinBuf->forward ;
	else
	{
		twinBuf->begin = twinBuf->forward ;
		twinBuf->where_begin = TWIN_TWO ;
	}
}
 
token *handleLexError (DFAError err, twinBuffer *twinBuf)
{
	token *tk = (token *) malloc (sizeof(token)) ;
	tk->id = TK_LEXERROR ;
	tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
	switch (err)
	{
		case UNKNOWN_SYMBOL :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Unknown Symbol") + 1)) ;
			strcpy (tk->lexeme, "Unknown Symbol") ;
			break ;
 
		case EOF_WHILE_LEXING :
			retract (twinBuf) ;
			resetBeginPointer (twinBuf) ;
			tk->lineNumber = twinBuf->lineNumber ;
			nextChar (twinBuf) ;

			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Reached EOF while lexing") + 1)) ;
			strcpy (tk->lexeme, "Reached EOF while lexing") ;
			break ;
 
		case EOF_COMMENT :
			retract (twinBuf) ;
			resetBeginPointer (twinBuf) ;
			tk->lineNumber = twinBuf->lineNumber ;
			nextChar (twinBuf) ;

			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("EOF encountered before end of comment") + 1)) ;
			strcpy (tk->lexeme , "EOF encountered before end of comment") ;
			break ;
 
		case RANGEOP_ERR :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Expected '.'")+1)) ;
			strcpy (tk->lexeme, "Expected '.'") ;
			break ;
 
		case EQ_EXPECTED :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Expected '='") + 1)) ;
			strcpy (tk->lexeme, "Expected '='") ;
			break ;
 
		case TOO_LONG_ID :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Identifier longer than 20 characters") + 1)) ;
			strcpy (tk->lexeme, "Identifier longer than 20 characters") ;
			break ;
 
		case RNUM_DIG_EXPECTED :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Digit expected in real number") + 1)) ;
			strcpy (tk->lexeme, "Digit expected in real number") ;
			break ;
 
		case RNUM_DIGORSIGN_EXPECTED :
			tk->lexeme = (char *) malloc (sizeof(char) * (strlen("Digit or +/- expected in real number") + 1)) ;
			strcpy (tk->lexeme, "Digit or +/- expected in real number") ;
			break ;
	}

	nextChar (twinBuf) ;
	resetBeginPointer (twinBuf) ;
 
	return tk ;
}

token* getNextToken(twinBuffer *twinBuf)
{
	token *  toret = getNextTok(twinBuf);
	while(1){
		if(toret->id !=TK_COMMENT)
			return toret;
 
		toret = getNextTok(twinBuf);
	}
}
 
token* getNextTok (twinBuffer *twinBuf)
{
	char ch ;
	char *lexeme ;
	token *tk = NULL ;
 
	DFAStates dfa ;
	DFAError err ;
	DFAsim sim = NONFINAL ;
	dfa = STATE_START ;
 
 
	while (1)
	{
		switch (dfa)
		{
			case STATE_START :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == EOF)
					dfa = FINAL_EOF ;
				else if (ch == ' ' || ch == '\n' || ch == '\t')
				{
					resetBeginPointer (twinBuf) ;
					dfa = STATE_START ;
				}
				else if (isdigit(ch))
					dfa = STATE_NUM_RNUM1 ;
				else if (isalpha(ch))
					dfa = STATE_ID1 ;
				else if (ch == '+')
					dfa = FINAL_PLUS ;
				else if (ch == '-')
					dfa = FINAL_MINUS ;
				else if (ch == '/')
					dfa = FINAL_DIV ;
				else if (ch == ')') 
					dfa = FINAL_BC ;
				else if (ch == '(')
					dfa = FINAL_BO ;
				else if (ch == ']')
					dfa = FINAL_SQBC ;
				else if (ch == '[')
					dfa = FINAL_SQBO ;
				else if (ch == ';')
					dfa = FINAL_SEMICOL ;
				else if (ch == '.')
					dfa = STATE_RANGEOP1 ;
				else if (ch == ',')
					dfa = FINAL_COMMA ;
				else if (ch == '!')
					dfa = STATE_NE1 ;
				else if (ch == '=')
					dfa = STATE_EQ1 ;
				else if (ch == ':') 
					dfa = STATE_COLON_ASSIGNOP ;
				else if (ch == '<')
					dfa = STATE_LT ;
				else if (ch == '>')
					dfa = STATE_GT ;
				else if (ch == '*')
					dfa = STATE_ASTERISK ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					err = UNKNOWN_SYMBOL ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_PLUS :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_PLUS ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_MINUS :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_MINUS ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_DIV :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_DIV ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_BC :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_BC ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
 
			//----------------------------------------------------------
 
			case FINAL_BO :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_BO ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_SQBC :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_SQBC ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_SQBO :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_SQBO ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_SEMICOL :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_SEMICOL ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_COMMA :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_COMMA ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_RANGEOP1 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '.')
					dfa = FINAL_RANGEOP ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					err = RANGEOP_ERR ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_RANGEOP :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_RANGEOP ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_COLON_ASSIGNOP :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
 
				if (ch == '=')
					dfa = FINAL_ASSIGNOP ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_COLON ;	
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_ASSIGNOP :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_ASSIGNOP ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_COLON :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_COLON ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_NE1 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '=')
					dfa = FINAL_NE ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					err = EQ_EXPECTED ;
					dfa = STATE_TRAP ;
					resetBeginPointer(twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_NE :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_NE ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_EQ1 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
 
				if (ch == '=')
					dfa = FINAL_EQ ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					err = EQ_EXPECTED ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_EQ :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_EQ ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
 
			//======================================================================================================================
 
			case STATE_GT :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '>')
					dfa = STATE_GTGT ;
				else if (ch == '=') 
					dfa = FINAL_GE ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_GT ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_GTGT :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '>')
					dfa = FINAL_DRIVERENDDEF ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ENDDEF ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_DRIVERENDDEF :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_DRIVERENDDEF ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_ENDDEF :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_ENDDEF ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_GE :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_GE ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_GT :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_GT ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_LT :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
 
				if (ch == '<')
					dfa = STATE_LTLT ;
				else if (ch == '=') 
					dfa = FINAL_LE ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_LT ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_LTLT :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '<')
					dfa = FINAL_DRIVERDEF ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_DEF ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_DRIVERDEF :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_DRIVERDEF ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_DEF :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_DEF ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_LE :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_LE ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_LT :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_LT ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_ASTERISK :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '*')
					dfa = STATE_COMMENT_START  ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_MUL ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_COMMENT_START :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '*')
					dfa = STATE_COMMENT_END1 ;
				else if (ch == EOF)
				{
					err = EOF_COMMENT ;
					dfa = STATE_TRAP ;
				}
				else
					resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_COMMENT_END1 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '*')
					dfa = FINAL_COMMENT_END2 ;
				/*
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;

					err = COMMENT_ASTERISK ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
				*/
				else if (ch == EOF)
				{
					err = EOF_COMMENT ;
					dfa = STATE_TRAP ;
				}
				else
				{
					dfa = STATE_COMMENT_START ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------				
 
			case FINAL_COMMENT_END2 :
				sim = FINAL ;
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_COMMENT ;
				tk->lexeme = NULL ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_MUL :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_MUL ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_ID1 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID2 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID2 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID3 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID3 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID4 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID4 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID5 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID5 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID6 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID6 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID7 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID7 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID8 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID8 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID9 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID9 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID10 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID10 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID11 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID11 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID12 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID12 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID13 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID13 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID14 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID14 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID15 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID15 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID16 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID16 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID17 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID17 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID18 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID18 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID19 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_ID19 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_ID20 ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
			
			//----------------------------------------------------------
 
			case STATE_ID20 :
				sim = NONFINAL ;
				ch = nextChar(twinBuf) ;
 
				if (isalpha(ch) || isdigit (ch) || ch == '_')
					dfa = STATE_LONG_ID ;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_ID ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_ID :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				if ((tk->id = keywordToTokenID (tk->lexeme)) == -1)
					tk->id = TK_ID ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_LONG_ID :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (!(isalpha(ch) || isdigit (ch) || ch == '_'))
				{
					err = TOO_LONG_ID ;
					dfa = STATE_TRAP ;
					if (ch != EOF)
						retract (twinBuf) ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//======================================================================================================================
 
			case STATE_NUM_RNUM1 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '.')
					dfa = STATE_NUM_RNUM2 ;
				else if (isdigit(ch))
					;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_NUM ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_NUM_RNUM2 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '.')
				{
					retract (twinBuf) ;
					retract (twinBuf) ;
					dfa = FINAL_NUM ;
				}
				else if (isdigit(ch))
					dfa = STATE_RNUM3 ;
				else
				{
					err = RNUM_DIG_EXPECTED ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_RNUM3 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == 'e' || ch == 'E')
				{
					//printf ("HERE\n") ;
					dfa = STATE_RNUM4 ;
				}
				else if (isdigit(ch))
					;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_RNUM ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_RNUM4 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (ch == '+' || ch == '-')
					dfa = STATE_RNUM5 ;
				else if (isdigit (ch))
				{
					//printf ("HERE\n") ;
					dfa = STATE_RNUM6 ;
				}
				else
				{
					err = RNUM_DIGORSIGN_EXPECTED ;
					dfa = STATE_TRAP ;
					resetBeginPointer (twinBuf) ;
				}
				break ;
 
			//----------------------------------------------------------
 
			case STATE_RNUM5 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (isdigit(ch))
					dfa = STATE_RNUM6 ;
				else
				{
					err = RNUM_DIG_EXPECTED ;
					dfa = STATE_TRAP ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case STATE_RNUM6 :
				sim = NONFINAL ;
				ch = nextChar (twinBuf) ;
 
				if (isdigit(ch))
					;
				else
				{
					if (ch != EOF)
						retract (twinBuf) ;
					dfa = FINAL_RNUM ;
				}
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_NUM :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_NUM ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_RNUM :
				sim = FINAL ;
 
				retract (twinBuf) ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_RNUM ;
				tk->lexeme = extract_lexeme (twinBuf) ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				nextChar (twinBuf) ;
				resetBeginPointer (twinBuf) ;
 
				break ;
 
			//======================================================================================================================
 
			case STATE_TRAP :
				sim = TRAP ;
				tk = handleLexError (err, twinBuf) ;
				break ;
 
			//----------------------------------------------------------
 
			case FINAL_EOF :
				sim = FINAL ;
 
				tk = (token *) malloc (sizeof(token)) ;
				tk->id = TK_EOF ;
				tk->lexeme = (char *) malloc (sizeof(char)) ;
				*tk->lexeme = EOF ;
				tk->lineNumber = getLexemeLineNumber (twinBuf) ;
 
				break ;
 
		}
 
		if (sim == FINAL || sim == TRAP)
			break ;
	}
 
 
	if (ch == EOF && sim == NONFINAL)
	{
		printf ("Will probably never come here!\n") ;
		err = EOF_WHILE_LEXING ;
		tk = handleLexError (err, twinBuf) ;
	}
 
	return tk ;
}

char nextChar (twinBuffer *twinBuf)
{
	char ch ;
	ch = *twinBuf->forward ;
 
	if (ch == EOF)
		return EOF ;
 
	if (ch == '\n')
		twinBuf->lineNumber++ ;
 
	twinBuf->forward++ ;
 
	if (twinBuf->where_forward == TWIN_ONE && *twinBuf->forward == EOF)
	{
		// If the forward pointer is in the first buffer and the fwd pointer is on EOF
 
		if (twinBuf->forward - twinBuf->buf1 == bufSize)
		{
			/* If the expected fwd pointer is EOF and end of buffer, then it's most like true that
			   there is more data. Bgn pointer is expected to be near the end of the first buffer 
			   and fwd pointer is set to start of buf2.
 
			   Even if there actually isn't any data, buf2[0] = ACTUAL EOF and this is what the 
			   fwd poiner is set to. Bgn pointer is again expected to be near the end of the first buffer.
			*/
 
			twinBuf->forward = twinBuf->buf2 ;
			twinBuf->where_forward = TWIN_TWO ;
		}
		/* If the above condition fails, the fwd pointer is on the ACTUAL EOF. 
		   Bgn pointer is expected to be just before the EOF
		   Any subsequent call to nextChar() will actually fill ch with EOF.
		*/
	}
	else if (twinBuf->where_forward == TWIN_TWO && *twinBuf->forward == EOF)
	{
		// If the fwd pointer is on the second buffer and fwd pointer is on EOF
		int buf2_read ;
 
		if (twinBuf->where_begin == TWIN_TWO && twinBuf->forward - twinBuf->buf2 == bufSize)
		{
			/* If the expected fwd pointer is EOF and at end of second buffer, then it's most likely
			   true that there is more data. Bgn pointer is expected to be near the end of the second 
			   buffer. Buf2 is shifted buf1. Bgn pointer is at the same relative location. Fwd pointer
			   is set to start of buf2.
				
			   Even if there isn't actually any data, buf2[0] = ACTUAL EOF and this is what the fwd pointer
			   is set to. Bgn pointer is again expected to be near the end of the first buffer.
			*/
 
			char *xchg_buf ;
			xchg_buf = twinBuf->buf1 ;
			twinBuf->buf1 = twinBuf->buf2 ;
			twinBuf->buf2 = xchg_buf ;
 
			buf2_read = fread (twinBuf->buf2, sizeof(char) , bufSize, twinBuf->fp) ;
			twinBuf->buf2[buf2_read] = EOF ;
			twinBuf->forward = twinBuf->buf2 ;
 
			twinBuf->where_begin = TWIN_ONE ;
		}
		/* If the above condition fails, the fwd pointer is on the ACTUAL EOF.
		   Bgn pointer is expected to be just before the EOF on the second buffer.
		   Any subsequent call to nextChar() will actually fill ch with EOF.
		*/
 
	}
	return ch ;
}
 
void retract (twinBuffer *twinBuf)
{
	if (twinBuf->where_forward == TWIN_ONE)
	{
		if (twinBuf->forward == twinBuf->buf1)
			printf ("Cannot retract!\n") ;
		else
			twinBuf->forward-- ;
	}
	else if (twinBuf->where_forward == TWIN_TWO)
	{
		if (twinBuf->forward == twinBuf->buf2)
		{
			twinBuf->forward = &twinBuf->buf1[bufSize - 1] ;
			twinBuf->where_forward = TWIN_ONE ;
		}
		else
			twinBuf->forward-- ;
	}
 
	if (*twinBuf->forward == '\n')
		twinBuf->lineNumber-- ;
 
}

char* extract_lexeme (twinBuffer *twinBuf)
/*
Given the location of forward and begin pointer, it extracts the characters
into a lexeme 
*/
{
	char *word , *ptr ;
	int i , len ;
 
	if (*twinBuf->forward == EOF)
	{
		if (twinBuf->where_begin == twinBuf->where_forward)
		{
 
			ptr = twinBuf->begin ;
			if (*ptr == EOF)
				return NULL ;
 
			while (*ptr != EOF)
				ptr++ ;
			word = (char *) malloc (sizeof(char) * (ptr - twinBuf->begin + 1)) ;
			ptr = twinBuf->begin ;
			for (i = 0 ; *ptr != EOF ; i++)
			{
				word[i] = *ptr ;
				ptr++ ;
			}
 
			word[i] = '\0' ;
			return word ;
		}
		else
		{
			int j ;
 
			len = (twinBuf->forward - twinBuf->buf2 + 1) + (twinBuf->buf1 + bufSize - twinBuf->begin) ;
			word = (char *) malloc (sizeof(char) * (len + 1)) ;
 
			ptr = twinBuf->begin ;
			if (*ptr == EOF)
				return NULL ;
			for (i = 0 ; *ptr != EOF ; i++)
			{
				word[i] = *ptr ;
				ptr++ ;
			}
 
			ptr = twinBuf->buf2 ;
			for ( ; i < len ; i++)
			{
				word[i] = *ptr ;
				ptr++ ;
			}
 
			word[len] = '\0' ;
		}
	}
	else if ((twinBuf->where_begin == TWIN_ONE && twinBuf->where_forward == TWIN_ONE) || (twinBuf->where_begin == TWIN_TWO && twinBuf->where_forward == TWIN_TWO))
	{
		len = twinBuf->forward - twinBuf->begin + 1 ;
		word = (char *) malloc (sizeof(char) * (len + 1)) ;
 
		ptr = twinBuf->begin ;
		for (i = 0 ; i < len ; i++)
		{
			word[i] = *ptr ;
			ptr++ ;
		}
 
		word[len] = '\0' ;
	}
	else if (twinBuf->where_begin == TWIN_ONE && twinBuf->where_forward == TWIN_TWO)
	{
		int j ;
 
		len = (twinBuf->forward - twinBuf->buf2 + 1) + (twinBuf->buf1 + bufSize - twinBuf->begin) ;
		word = (char *) malloc (sizeof(char) * (len + 1)) ;
 
		ptr = twinBuf->begin ;
		for (i = 0 ; *ptr != EOF ; i++)
		{
			word[i] = *ptr ;
			ptr++ ;
		}
 
		ptr = twinBuf->buf2 ;
		for ( ; i < len ; i++)
		{
			word[i] = *ptr ;
			ptr++ ;
		}
 
		word[len] = '\0' ;
	}
 
	return word ;
}