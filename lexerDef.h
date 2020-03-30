#ifndef _lexerDef
#define _lexerDef



typedef enum _tokenID
{
	program, moduleDeclarations, otherModules, moduleDeclaration, IPL, input_plist, ret, moduleDef, module, 
	driverModule, output_plist, OPL, statements, statement, ioStmt, print_var, simpleStmt, whichStmt, assignmentStmt, 
	lvalueIDStmt, lvalueARRStmt, moduleReuseStmt, optional, declareStmt, iterativeStmt, conditionalStmt, caseStmt, 
	default_new, expression_new, U, expression, bT, boolTerm, aE, arithmeticExpr, aT, term, aF, factor, factor_new, logicalOp, 
	relationalOp, pmop, mdop, dataType, type, var, whichID, index_new, value, range, range_new, idList, idL,

	TK_EPS,
	TK_INTEGER, TK_REAL, TK_BOOLEAN, TK_OF, TK_ARRAY, TK_START, TK_END, TK_DECLARE, TK_MODULE,TK_ID, TK_DRIVER,
	TK_PROGRAM, TK_GET_VALUE, TK_PRINT, TK_USE, TK_WITH, TK_PARAMETERS, TK_TRUE, TK_FALSE, TK_TAKES, TK_COMMENT,
	TK_INPUT, TK_RETURNS, TK_AND, TK_OR, TK_FOR, TK_IN, TK_SWITCH, TK_CASE, TK_BREAK, TK_DEFAULT, TK_WHILE, TK_PLUS,
	TK_MINUS, TK_MUL, TK_DIV, TK_LT, TK_LE, TK_GE, TK_GT, TK_EQ, TK_NE, TK_DEF, TK_ENDDEF, TK_DRIVERDEF, TK_DRIVERENDDEF, TK_COLON, TK_RANGEOP, TK_SEMICOL,
	TK_COMMA, TK_ASSIGNOP, TK_SQBO, TK_SQBC, TK_BO, TK_BC, TK_LEXERROR, TK_CHAR_BADCOMMENT, TK_EOF, TK_NUM,TK_RNUM
} tokenID ;


typedef struct _token
{
	tokenID id ;
	char *lexeme ;
	int lineNumber ;
} token ;

#endif