typedef enum _whereBufPointer
{
	TWIN_ONE, TWIN_TWO
} whereBufPointer ;

typedef struct _twinBuffer 
{
	FILE* fp ;
	char *buf1, *buf2 ;
	char *begin, *forward ;
	whereBufPointer where_begin, where_forward ;

	int lineNumber ;
} twinBuffer ;

typedef enum _DFAStates
{
	STATE_START,

	FINAL_PLUS,
	FINAL_MINUS,
	FINAL_DIV,
	FINAL_BC,
	FINAL_BO,
	FINAL_SQBC,
	FINAL_SQBO,
	FINAL_SEMICOL,
	FINAL_COMMA,

	STATE_RANGEOP1, FINAL_RANGEOP,

	STATE_NE1, FINAL_NE,

	STATE_EQ1, FINAL_EQ,
	
	STATE_COLON_ASSIGNOP, FINAL_ASSIGNOP, FINAL_COLON,

	STATE_ID1, STATE_ID2, STATE_ID3, STATE_ID4, STATE_ID5, STATE_ID6, STATE_ID7, STATE_ID8, STATE_ID9, STATE_ID10,
	STATE_ID11, STATE_ID12, STATE_ID13, STATE_ID14, STATE_ID15, STATE_ID16, STATE_ID17, STATE_ID18, STATE_ID19,
	STATE_ID20, FINAL_ID, STATE_LONG_ID,

	STATE_ASTERISK, STATE_COMMENT_START, STATE_COMMENT_END1, FINAL_COMMENT_END2,
	FINAL_MUL,

	STATE_GT, STATE_GTGT, FINAL_GT, FINAL_ENDDEF, FINAL_DRIVERENDDEF, FINAL_GE,

	STATE_LT, STATE_LTLT, FINAL_LT, FINAL_DEF, FINAL_DRIVERDEF, FINAL_LE,

	STATE_NUM_RNUM1, STATE_NUM_RNUM2, STATE_RNUM3, STATE_RNUM4, STATE_RNUM5, STATE_RNUM6,
	FINAL_NUM, FINAL_RNUM,

	STATE_TRAP,

	FINAL_EOF 
} DFAStates ;

typedef enum _DFAError
{
	UNKNOWN_SYMBOL,
	RANGEOP_ERR,
	EQ_EXPECTED,
	EOF_COMMENT,
	TOO_LONG_ID,
	RNUM_DIG_EXPECTED,
	RNUM_DIGORSIGN_EXPECTED,
	EOF_WHILE_LEXING
} DFAError ;

typedef enum _DFAsim
{
	NONFINAL, FINAL, TRAP
} DFAsim ;

// definitions

tokenID keywordToTokenID (char *str) ;
twinBuffer* lexer_init (FILE *fp) ;
void lexer_destroy (twinBuffer *twinBuf) ;
void resetBeginPointer (twinBuffer *twinBuf) ;
int getLexemeLineNumber (twinBuffer *twinBuf) ;
token *handleLexError (DFAError err, twinBuffer *twinBuf) ;
token* getNextTok (twinBuffer *fs) ;
token* getNextToken (twinBuffer *fs) ;

char nextChar (twinBuffer *fs) ;
void retract (twinBuffer *fs) ;
char* extract_lexeme (twinBuffer *fs) ;