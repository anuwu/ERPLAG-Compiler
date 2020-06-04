#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "parser.h"
#include "lexer.h"
#include "error.h"

#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule


// structures 
//globals

Rule rule[SIZE];
node* firsts[SIZE];
node* follows[SIZE];
location heads[SIZE];
int parsetable[150][150];
int rule_count;
char utility_array[150][40] ; 
extern twinBuffer* lexer_init (FILE *fp) ;
extern token* getNextTok (twinBuffer *fs) ;
extern token* getNextToken (twinBuffer *fs) ;

//functions
location* init()
{
	location* tmp= (location*)malloc(sizeof(location));
	tmp->next=NULL;
	return tmp;
}

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


		////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////


		case program :
			return "program" ;
		case moduleDeclarations :
			return "moduleDeclarations" ;
		case otherModules :
			return "otherModules" ;
		case moduleDeclaration :
			return "moduleDeclaration" ;
		case IPL :
			return "IPL" ;
		case input_plist :
			return "input_plist" ;
		case ret :
			return "ret" ;
		case moduleDef :
			return "moduleDef" ;
		case module :
			return "module" ;
		case driverModule :
			return "driverModule" ;
		case output_plist :
			return "output_plist" ;
		case OPL :
			return "OPL" ;
		case statements :
			return "statements" ;
		case statement :
			return "statement" ;
		case ioStmt :
			return "ioStmt" ;
		case print_var :
			return "print_var" ;
		case simpleStmt :
			return "simpleStmt" ;
		case whichStmt :
			return "whichStmt" ;
		case assignmentStmt :
			return "assignmentStmt" ;
		case lvalueIDStmt :
			return "lvalueIDStmt" ;
		case lvalueARRStmt :
			return "lvalueARRStmt" ;
		case moduleReuseStmt :
			return "moduleReuseStmt" ;
		case optional :
			return "optional" ;
		case declareStmt :
			return "declareStmt" ;
		case iterativeStmt :
			return "iterativeStmt" ;
		case conditionalStmt :
			return "conditionalStmt" ;
		case caseStmt :
			return "caseStmt" ;
		case default_new :
			return "default_new"  ;
		case expression_new :
			return "expression_new" ;
		case U :
			return "U" ;
		case expression :
			return "expression" ;
		case bT :
			return "bT" ;
		case boolTerm :
			return "boolTerm" ;
		case aE :
			return "aE" ;
		case arithmeticExpr :
			return "arithmeticExpr" ;
		case aT :
			return "aT" ;
		case term :
			return "term" ;
		case aF :
			return "aF" ;
		case factor :
			return "factor" ;
		case factor_new :
			return "factor_new" ;
		case logicalOp :
			return "logicalOp" ;
		case relationalOp :
			return "relationalOp" ;
		case pmop :
			return "pmop" ;
		case mdop :
			return "mdop" ;
		case dataType :
			return "dataType" ;
		case type :
			return "type" ;
		case var :
			return "var" ;
		case whichID :
			return "whichId" ;
		case index_new :
			return "index_new" ;
		case value :
			return "value" ;
		case range :
			return "range" ;
		case range_new :
			return "range_new" ;
		case idList :
			return "idList" ;
		case idL :
			return "idL" ;
 	}
}


void populate_hash(int index,int enum_value)
{
	if(heads[enum_value].ind == -1)
	{
		heads[enum_value].ind=index;	
	}

	else
	{
		location* tmp=&heads[enum_value];
		while(tmp->next!=NULL)
		{
			tmp=tmp->next;
		}
		tmp->next=init();
		tmp=tmp->next;
		tmp->ind=index;
		tmp->next=NULL;
	}
}


tokenID grammarStringToTokenID (char *str){
	tokenID id ;

	if (strcmp(str,"INTEGER")==0)
		id = TK_INTEGER ;
	else if (strcmp(str, "REAL")==0)
		id = TK_REAL ;
	else if (strcmp(str, "BOOLEAN") == 0)
		id = TK_BOOLEAN ;
	else if (strcmp(str, "OF")==0)
		id = TK_OF ;
	else if (strcmp(str, "ARRAY")==0)
		id = TK_ARRAY ;
	else if (strcmp(str, "START")==0)
		id = TK_START ;
	else if (strcmp(str, "END")==0)
		id = TK_END ;
	else if (strcmp(str, "DECLARE")==0)
		id = TK_DECLARE ;
	else if (strcmp(str, "MODULE")==0)
		id = TK_MODULE ;
	else if (strcmp(str, "DRIVER")==0)
		id = TK_DRIVER ;
	else if (strcmp(str, "PROGRAM")==0)
		id = TK_PROGRAM ;
	else if (strcmp(str, "GET_VALUE")==0)
		id = TK_GET_VALUE ;
	else if (strcmp(str, "PRINT")==0)
		id = TK_PRINT ;
	else if (strcmp(str, "USE")==0)
		id = TK_USE ;
	else if (strcmp(str, "WITH")==0)
		id = TK_WITH ;
	else if (strcmp(str, "PARAMETERS")==0)
		id = TK_PARAMETERS ;
	else if (strcmp(str, "TRUE")==0)
		id = TK_TRUE ;
	else if (strcmp(str, "FALSE")==0)
		id = TK_FALSE ;
	else if (strcmp(str, "TAKES")==0)
		id = TK_TAKES ;
	else if (strcmp(str, "INPUT")==0)
		id = TK_INPUT ;
	else if (strcmp(str, "RETURNS")==0)
		id = TK_RETURNS ;
	else if (strcmp(str, "AND")==0)
		id = TK_AND ;
	else if (strcmp(str, "OR")==0)
		id = TK_OR ;
	else if (strcmp(str,"FOR")==0)
		id = TK_FOR ;
	else if (strcmp(str, "IN")==0)
		id = TK_IN ;
	else if (strcmp(str, "SWITCH")==0)
		id = TK_SWITCH ;
	else if (strcmp(str, "CASE")==0)
		id = TK_CASE ;
	else if (strcmp(str, "BREAK")==0)
		id = TK_BREAK ;
	else if (strcmp(str, "DEFAULT")==0)
		id = TK_DEFAULT ;
	else if (strcmp(str, "WHILE")==0)
		id = TK_WHILE ;
	else if (strcmp(str, "PLUS")==0)
		id = TK_PLUS ;
	else if (strcmp(str, "MINUS")==0)
		id = TK_MINUS ;
	else if (strcmp(str, "MUL")==0)
		id = TK_MUL ;
	else if (strcmp(str, "DIV")==0)
		id = TK_DIV ;
	else if (strcmp(str, "LT")==0)
		id = TK_LT ;
	else if (strcmp(str, "LE")==0)
		id = TK_LE ;
	else if (strcmp(str, "GE")==0)
		id = TK_GE ;
	else if (strcmp(str, "GT")==0)
		id = TK_GT ;
	else if (strcmp(str, "EQ")==0)
		id = TK_EQ ;
	else if (strcmp(str, "NE")==0)
		id = TK_NE ;
	else if (strcmp(str, "DEF")==0)
		id = TK_DEF ;
	else if (strcmp(str, "ENDDEF")==0)
		id = TK_ENDDEF ;
	else if (strcmp(str, "DRIVERDEF")==0)
		id = TK_DRIVERDEF ;
	else if (strcmp(str, "DRIVERENDDEF")==0)
		id = TK_DRIVERENDDEF ;
	else if (strcmp(str, "COLON")==0)
		id = TK_COLON ;
	else if (strcmp(str, "RANGEOP")==0)
		id = TK_RANGEOP ;
	else if (strcmp(str, "SEMICOL")==0)
		id = TK_SEMICOL ;
	else if (strcmp(str, "COMMA")==0)
		id = TK_COMMA ;
	else if (strcmp(str, "ASSIGNOP")==0)
		id = TK_ASSIGNOP ;
	else if (strcmp(str, "SQBO")==0)
		id = TK_SQBO ;
	else if (strcmp(str, "SQBC")==0)
		id = TK_SQBC ;
	else if (strcmp(str, "BO")==0)
		id = TK_BO ;
	else if (strcmp(str, "BC")==0)
		id = TK_BC ;
	/////////////////////// lexerror and EOF are handled differently
	else if (strcmp(str, "TK_LEXERROR")==0)
		id = TK_LEXERROR ;
	else if (strcmp(str, "EOF")==0)
		id = TK_EOF ;
	///////////////////////////////////////////////////////////////
	else if (strcmp(str, "EPS")==0)
		id = TK_EPS ;
	else if (strcmp(str, "NUM")==0)
		id = TK_NUM ;
	else if (strcmp(str, "RNUM")==0)
		id = TK_RNUM ;
	else if (strcmp(str, "program")==0)
		id = program ;
	else if (strcmp(str, "otherModules")==0)
		id = otherModules ;																	
	else if (strcmp(str, "moduleDeclarations")==0)
		id = moduleDeclarations ;
	else if (strcmp(str, "moduleDeclaration")==0)
		id = moduleDeclaration ;
	else if (strcmp(str, "IPL")==0)
		id =  IPL;
	else if (strcmp(str, "input_plist")==0)
		id = input_plist ;
	else if (strcmp(str, "ret")==0)
		id = ret ;
	else if (strcmp(str, "moduleDef")==0)
		id = moduleDef ;
	else if (strcmp(str, "module")==0)
		id = module ;
	else if (strcmp(str,"ID")==0)
		id = TK_ID;
	else if (strcmp(str, "driverModule")==0)
		id =  driverModule;
	else if (strcmp(str, "output_plist")==0)
		id = output_plist ;
	else if (strcmp(str, "OPL")==0)
		id =  OPL;
	else if (strcmp(str, "statements")==0)
		id = statements ;
	else if (strcmp(str, "statement")==0)
		id = statement ;
	else if (strcmp(str, "ioStmt")==0)
		id = ioStmt ;
	else if (strcmp(str, "print_var")==0)
		id = print_var ;
	else if (strcmp(str, "simpleStmt")==0)
		id = simpleStmt ;
	else if (strcmp(str, "assignmentStmt")==0)
		id = assignmentStmt ;
	else if (strcmp(str, "whichStmt")==0)
		id = whichStmt ;
	else if (strcmp(str, "lvalueIDStmt")==0)
		id = lvalueIDStmt ;
	else if (strcmp(str, "lvalueARRStmt")==0)
		id=lvalueARRStmt ;
	else if (strcmp(str, "moduleReuseStmt")==0)
		id = moduleReuseStmt;
	else if (strcmp(str, "optional")==0)
		id =  optional;
	else if (strcmp(str, "declareStmt")==0)
		id = declareStmt ;
	else if (strcmp(str, "iterativeStmt")==0)
		id = iterativeStmt ;			 
	else if (strcmp(str, "conditionalStmt")==0)
		id =  conditionalStmt;	
	else if (strcmp(str, "caseStmt")==0)
		id =  caseStmt;
	else if (strcmp(str, "default_new")==0)
		id =  default_new;
	else if (strcmp(str, "expression_new")==0)
		id =  expression_new;
	else if (strcmp(str, "U")==0)
		id =  U;
	else if (strcmp(str, "expression")==0)
		id =expression;
	else if (strcmp(str, "bT")==0)
		id =bT;
	else if (strcmp(str, "boolTerm")==0)
		id =boolTerm;
	else if (strcmp(str, "aE")==0)
		id =aE;
	else if (strcmp(str, "arithmeticExpr")==0)
		id =arithmeticExpr;
	else if (strcmp(str, "aT")==0)
		id =aT;
	else if (strcmp(str, "term")==0)
		id =term;
	else if (strcmp(str, "aF")==0)
		id =aF;
	else if (strcmp(str, "factor")==0)
		id =factor;
	else if (strcmp(str , "factor_new")==0)
		id = factor_new ;
	else if (strcmp(str, "logicalOp")==0)
		id =logicalOp;
	else if (strcmp(str, "relationalOp")==0)
		id =relationalOp;
	else if (strcmp(str, "pmop")==0)
		id =pmop;
	else if (strcmp(str, "mdop")==0)
		id =mdop;
	else if (strcmp(str, "dataType")==0)
		id =dataType;
	else if (strcmp(str, "type")==0)
		id =type;
	else if (strcmp(str, "var")==0)
		id =var;
	else if (strcmp(str, "whichID")==0)
		id =whichID;
	else if (strcmp(str, "index_new")==0)
		id =index_new;
	else if (strcmp(str, "value")==0)
		id =value;
	else if (strcmp(str, "range")==0)
		id =range;
	else if (strcmp(str, "range_new")==0)
		id =range_new;
	else if (strcmp(str, "idList")==0)
		id =idList;
	else if (strcmp(str, "idL")==0)
		id = idL;
	else 
		return (tokenID)-1 ;
	
	return id ;
}

// for storing rules
node * initnode(int l){
	node * tmp = (node*)malloc(sizeof(node));
	tmp->key = (tokenID) l;
	tmp->next = NULL;

	return tmp;
}


void fillUtilityArray ()
{
	strcpy(utility_array[program],"program");
	strcpy(utility_array[moduleDeclarations],"moduleDeclarations");
	strcpy(utility_array[otherModules],"otherModules");
	strcpy(utility_array[moduleDeclaration],"moduleDeclaration");
	strcpy(utility_array[IPL],"IPL");
	strcpy(utility_array[input_plist],"input_plist");
	strcpy(utility_array[ret],"ret");
	strcpy(utility_array[moduleDef],"moduleDef");
	strcpy(utility_array[module],"module");
	strcpy(utility_array[driverModule],"driverModule");
	strcpy(utility_array[output_plist],"output_plist");
	strcpy(utility_array[OPL],"OPL");
	strcpy(utility_array[statements],"statements");
	strcpy(utility_array[statement],"statement");
	strcpy(utility_array[ioStmt],"ioStmt");
	strcpy(utility_array[print_var],"print_var");
	strcpy(utility_array[simpleStmt],"simpleStmt");
	strcpy(utility_array[whichStmt],"whichStmt");
	strcpy(utility_array[assignmentStmt],"assignmentStmt");
	strcpy(utility_array[lvalueIDStmt],"lvalueIDStmt");
	strcpy(utility_array[lvalueARRStmt],"lvalueARRStmt");
	strcpy(utility_array[moduleReuseStmt],"moduleReuseStmt");
	strcpy(utility_array[optional],"optional");
	strcpy(utility_array[declareStmt],"declareStmt");
	strcpy(utility_array[iterativeStmt],"iterativeStmt");
	strcpy(utility_array[conditionalStmt],"conditionalStmt");
	strcpy(utility_array[caseStmt],"caseStmt");
	strcpy(utility_array[default_new],"default_new");
	strcpy(utility_array[expression_new],"expression_new");
	strcpy(utility_array[U],"U");
	strcpy(utility_array[expression],"expression");
	strcpy(utility_array[bT],"bT");
	strcpy(utility_array[boolTerm],"boolTerm");
	strcpy(utility_array[aE],"aE");
	strcpy(utility_array[arithmeticExpr],"arithmeticExpr");
	strcpy(utility_array[aT],"aT");
	strcpy(utility_array[term],"term");
	strcpy(utility_array[aF],"aF");
	strcpy(utility_array[factor],"factor");
	strcpy(utility_array[factor_new] , "factor_new") ;
	strcpy(utility_array[logicalOp],"logicalOp");
	strcpy(utility_array[relationalOp],"relationalOp");
	strcpy(utility_array[pmop],"pmop");
	strcpy(utility_array[mdop],"mdop");
	strcpy(utility_array[dataType],"dataType");
	strcpy(utility_array[type],"type");
	strcpy(utility_array[var],"var");
	strcpy(utility_array[whichID],"whichID");
	strcpy(utility_array[index_new],"index_new");
	strcpy(utility_array[value],"value");
	strcpy(utility_array[range],"range");
	strcpy(utility_array[range_new],"range_new");
	strcpy(utility_array[idList],"idList");
	strcpy(utility_array[idL],"idL");
	strcpy(utility_array[TK_EPS],"TK_EPS");
	strcpy(utility_array[TK_INTEGER],"TK_INTEGER");
	strcpy(utility_array[TK_REAL],"TK_REAL");
	strcpy(utility_array[TK_BOOLEAN],"TK_BOOLEAN");
	strcpy(utility_array[TK_OF],"TK_OF");
	strcpy(utility_array[TK_ARRAY],"TK_ARRAY");
	strcpy(utility_array[TK_START],"TK_START");
	strcpy(utility_array[TK_END],"TK_END");
	strcpy(utility_array[TK_DECLARE],"TK_DECLARE");
	strcpy(utility_array[TK_MODULE],"TK_MODULE");
	strcpy(utility_array[TK_ID],"TK_ID");
	strcpy(utility_array[TK_DRIVER],"TK_DRIVER");
	strcpy(utility_array[TK_PROGRAM],"TK_PROGRAM");
	strcpy(utility_array[TK_GET_VALUE],"TK_GET_VALUE");
	strcpy(utility_array[TK_PRINT],"TK_PRINT");
	strcpy(utility_array[TK_USE],"TK_USE");
	strcpy(utility_array[TK_WITH],"TK_WITH");
	strcpy(utility_array[TK_PARAMETERS],"TK_PARAMETERS");
	strcpy(utility_array[TK_TRUE],"TK_TRUE");
	strcpy(utility_array[TK_FALSE],"TK_FALSE");
	strcpy(utility_array[TK_TAKES],"TK_TAKES");
	strcpy(utility_array[TK_COMMENT],"TK_COMMENT");
	strcpy(utility_array[TK_INPUT],"TK_INPUT");
	strcpy(utility_array[TK_RETURNS],"TK_RETURNS");
	strcpy(utility_array[TK_AND],"TK_AND");
	strcpy(utility_array[TK_OR],"TK_OR");
	strcpy(utility_array[TK_FOR],"TK_FOR");
	strcpy(utility_array[TK_IN],"TK_IN");
	strcpy(utility_array[TK_SWITCH],"TK_SWITCH");
	strcpy(utility_array[TK_CASE],"TK_CASE");
	strcpy(utility_array[TK_BREAK],"TK_BREAK");
	strcpy(utility_array[TK_DEFAULT],"TK_DEFAULT");
	strcpy(utility_array[TK_WHILE],"TK_WHILE");
	strcpy(utility_array[TK_PLUS],"TK_PLUS");
	strcpy(utility_array[TK_MINUS],"TK_MINUS");
	strcpy(utility_array[TK_MUL],"TK_MUL");
	strcpy(utility_array[TK_DIV],"TK_DIV");
	strcpy(utility_array[TK_LT],"TK_LT");
	strcpy(utility_array[TK_LE],"TK_LE");
	strcpy(utility_array[TK_GE],"TK_GE");
	strcpy(utility_array[TK_GT],"TK_GT");
	strcpy(utility_array[TK_EQ],"TK_EQ");
	strcpy(utility_array[TK_NE],"TK_NE");
	strcpy(utility_array[TK_DEF],"TK_DEF");
	strcpy(utility_array[TK_ENDDEF],"TK_ENDDEF");
	strcpy(utility_array[TK_DRIVERDEF],"TK_DRIVERDEF");
	strcpy(utility_array[TK_DRIVERENDDEF],"TK_DRIVERENDDEF");
	strcpy(utility_array[TK_COLON],"TK_COLON");
	strcpy(utility_array[TK_RANGEOP],"TK_RANGEOP");
	strcpy(utility_array[TK_SEMICOL],"TK_SEMICOL");
	strcpy(utility_array[TK_COMMA],"TK_COMMA");
	strcpy(utility_array[TK_ASSIGNOP],"TK_ASSIGNOP");
	strcpy(utility_array[TK_SQBO],"TK_SQBO");
	strcpy(utility_array[TK_SQBC],"TK_SQBC");
	strcpy(utility_array[TK_BO],"TK_BO");
	strcpy(utility_array[TK_BC],"TK_BC");
	strcpy(utility_array[TK_LEXERROR],"TK_LEXERROR");
	strcpy(utility_array[TK_EOF],"TK_EOF");
	strcpy(utility_array[TK_NUM],"TK_NUM");
	strcpy(utility_array[TK_RNUM],"TK_RNUM");
}

void traverse_grammar()
{
	for(int i=0;i<150;i++)
	{
		for(int j=0;j<150;j++)
		{
			parsetable[i][j]=-1;
		}
	}	

	for(int i=0;i<rule_count;i++)
	{
		int lhs_enum_value=rule[i].LHS;
		node* temp=rule[i].RHS;


		// NT -> eps 
		if(temp->key==TK_EPS)
		{
			node* temp3=follows[lhs_enum_value];
			while(temp3!=NULL)
			{
				parsetable[lhs_enum_value][temp3->key]=i;
				temp3=temp3->next;
			}
			continue;
		}	

		// traversing on rhs T/NT sequentially	
		while(temp!=NULL)
		{	
			// temp2 is Terminals in first of T/NT
			node* temp2=firsts[temp->key];

			int flag=0;
			while(temp2!=NULL)
			{
				if(temp2->key!=TK_EPS)
					parsetable[lhs_enum_value][temp2->key]=i;
				else
					flag=1;

				temp2=temp2->next;
			}

			if(flag==0)
				break;
			else // haven't encountered eps yet so keep filling by next T/NT's first
				temp=temp->next;

		}

		if(temp==NULL)
		{
			// all firsts are consumed and all are nullable
			node* temp3=follows[lhs_enum_value];
			while(temp3!=NULL)
			{
				parsetable[lhs_enum_value][temp3->key]=i;
				temp3=temp3->next;
			}
		}
		
	}	
}


void addUniquely(tokenID target, tokenID insert_it)
{
	//Target linked list should never be empty
	node * tmp_target = firsts[target];

	if(tmp_target->key != insert_it){
		while( 1){

			if(tmp_target->next ==  NULL){
				tmp_target->next = initnode(insert_it);
				return;
			}
			else if(tmp_target->next->key == insert_it){
				return;
			}
			tmp_target = tmp_target->next;
		}
		// else we need not to do anything because its already there
	}
	
}


void addUniquelyFollow(tokenID target, tokenID insert_it){
	//Target linked list should never be empty
	node * tmp_target = follows[target];

	if(tmp_target->key != insert_it){
		while( 1){

			if(tmp_target->next ==  NULL){
				tmp_target->next = initnode(insert_it);
				return;
			}
			else if(tmp_target->next->key == insert_it){
				return;
			}
			tmp_target = tmp_target->next;
		}
		// else we need not to do anything because its already there
	}
	
}


void unionFirst(tokenID target, tokenID source){
	node * sourceLL = firsts[source];
	node * tmp_target = firsts[target];

	if(sourceLL == NULL)
		return;

	// when target was initially empty
	if(tmp_target == NULL){
		firsts[target] = initnode(sourceLL->key);

		node * tmp = firsts[target] ;
		sourceLL = sourceLL->next ;
		while(sourceLL){

			if(sourceLL->key != TK_EPS){
				tmp->next = initnode(sourceLL->key);
				tmp = tmp->next;
				
			}
			
			sourceLL = sourceLL->next;
		}
	}
	else {
		// when target is initially not empty

		while(sourceLL!=NULL){
			tokenID need_to_insert = sourceLL->key;	

			if(need_to_insert != TK_EPS)	
				addUniquely(target,need_to_insert);

			sourceLL = sourceLL->next;
		}
	}

}

int containsEpsilon(tokenID id)
{
	node* tmp = firsts[id];
	while(tmp){
		if(tmp->key == TK_EPS)
			return 1;
		tmp = tmp->next ;
	}
	return 0;
}


void populateFirst (tokenID id)
{
	if(isTerminal(id))
		return;

	location* ruleLoc ;
	node* rRHS ;
	Rule r ;
	int rule_index ;
	ruleLoc = &heads[id] ;

	while (ruleLoc != NULL)
	{		
		rule_index = ruleLoc->ind ;
		r = rule[rule_index] ;
		rRHS = r.RHS ;

		// recursive call to fill all empty RHSs
		while(rRHS != NULL)
		{
			if (firsts[rRHS->key] == NULL)
				populateFirst (rRHS->key) ;

			unionFirst (id, rRHS->key) ;

			if (containsEpsilon(rRHS->key))
				rRHS = rRHS->next ;
			else
				break ;
		}

		if (rRHS == NULL)
			addUniquely (id, TK_EPS) ;

		ruleLoc = ruleLoc->next ;
	}
}

void printFollowSet (tokenID id)
{
	node* tmp = follows[id] ;
	if (tmp == NULL)
	 	printf ("Empty follow set\n") ;
	else
	{
	 while (tmp != NULL)
	 {
		 printf ("%d, ", tmp->key) ;
		 tmp = tmp->next ;
	 }
	 endl ;
	}
}

void populateFollow (tokenID id)
{
	int rule_index ;

	for (rule_index = 0 ; rule_index < rule_count ; rule_index++)
	{

		tokenID current_rule_lhs = (tokenID) rule[rule_index].LHS;
		node * tmp_of_rhs_of_current_rule = rule[rule_index].RHS;

		while(tmp_of_rhs_of_current_rule != NULL)
		{		
			if(tmp_of_rhs_of_current_rule->key == id)
			{ // if my key is id

				if(tmp_of_rhs_of_current_rule->next == NULL)
				{ // i am last element of this rule

					if(follows[current_rule_lhs] == NULL)
						populateFollow(current_rule_lhs);

					if (id != current_rule_lhs)
						unionFollow (id, current_rule_lhs) ;
				}
				else
				{

					if(containsEpsilon(tmp_of_rhs_of_current_rule->next->key))
					{ // next can go to null firsts[next] contains eps
		
						node * tmp = tmp_of_rhs_of_current_rule->next;
						while(tmp!=NULL && containsEpsilon(tmp->key))
						{
							UnionFollowFirst(id,tmp->key);
							tmp = tmp->next;
						}
						// while can exit either tmp == NULL
						if(tmp == NULL)
						{
							if(follows[current_rule_lhs] == NULL)
								populateFollow(current_rule_lhs);

							unionFollow(id,current_rule_lhs);
						}
						else
							UnionFollowFirst(id,tmp->key);

					}
					else
						UnionFollowFirst(id,tmp_of_rhs_of_current_rule->next->key);
					
				}
			}
			tmp_of_rhs_of_current_rule = tmp_of_rhs_of_current_rule->next;
		}
	}
}

void UnionFollowFirst (tokenID target, tokenID source)
 {
	node * sourceLL = firsts[source];
	node * tmp_target = follows[target];

	if(sourceLL == NULL)
		return;

	// when target was initially empty
	if(tmp_target == NULL){
		follows[target] = initnode(sourceLL->key);

		node * tmp = follows[target] ;
		sourceLL = sourceLL->next ;
		while(sourceLL){

			if(sourceLL->key != TK_EPS){
				tmp->next = initnode(sourceLL->key);
				tmp = tmp->next;	
			}
			
			sourceLL = sourceLL->next;
		}
	}
	else {
		// when target is initially not empty
		while(sourceLL!=NULL){
			tokenID need_to_insert = sourceLL->key;	

			if(need_to_insert != TK_EPS)	
				addUniquelyFollow(target,need_to_insert);

			sourceLL = sourceLL->next;
		}
	}
}

void unionFollow(tokenID target, tokenID source){
	node * sourceLL = follows[source];
	node * tmp_target = follows[target];

	if(sourceLL == NULL)
		return;

	// when target was initially empty
	if(tmp_target == NULL){
		follows[target] = initnode(sourceLL->key);

		node * tmp = follows[target] ;
		sourceLL = sourceLL->next ;
		while(sourceLL){
			tmp->next = initnode(sourceLL->key);
			tmp = tmp->next ;
			sourceLL = sourceLL->next;
		}
	}
	else {
		// when target is initially not empty

		while(sourceLL!=NULL){
			tokenID need_to_insert = sourceLL->key;	
			addUniquelyFollow(target,need_to_insert);
			sourceLL = sourceLL->next;
		}
	}
}


/******************************************************************************************
*/
  
char* nodeSymbol (treeNode* node)
{
	if (node == NULL)
		return "ROOT" ;

	if (node->tag == NON_TERMINAL)
		return utility_array[node->tnt.nonTerm] ;
	else
		return utility_array[node->tnt.term->id] ;
}
 
char* isLeafNode (treeNode* node)
{
	if (node->child == NULL)
		return "YES" ;
	else
		return "NO" ;
}
 

void init_parser()
{
	rule_count =0;

	for(int i=0;i<SIZE;i++)
	{
		heads[i].ind=-1;
		heads[i].next=NULL;
	}

 
	// file handling

	int slash = 0 ;
	char grammarPath[500] ;
	strcpy (grammarPath, __FILE__) ;

	slash = strlen (grammarPath) - 1 ;

	#ifdef _WIN64
	while (grammarPath[slash] != '\\')
	#endif
	#if defined __linux__ || defined __MACH__	
	while (grammarPath[slash] != '/')
	#endif
		slash-- ;

	grammarPath[slash+1] = '\0' ;
	strcat (grammarPath, "grammar.txt") ;
	FILE* file_pointer=fopen(grammarPath, "r");
	
	int st=0; // 3 states 0-LHS 1-RHS_head 2-rests
	char tempstr[TNTLENGTH];
	node* tmp;
	
	while( fscanf(file_pointer,"%[^\n\r ])",tempstr) != EOF)
	{
		if(st==0){
			rule[rule_count].LHS = grammarStringToTokenID(tempstr);
			populate_hash(rule_count,grammarStringToTokenID(tempstr));
			st++;
		}
		else if(st==1){
			rule[rule_count].RHS = initnode(grammarStringToTokenID(tempstr));
			tmp = rule[rule_count].RHS;
			st++;
		}
		else if(st!=-1){
			tmp->next = initnode(grammarStringToTokenID(tempstr));
			tmp = tmp->next;
		}
 
		char ch;
		ch=fgetc(file_pointer);
		if(ch=='\r') st=-1;
		else if(ch=='\n'){
			rule_count++;
			st=0;
		}
	}
	rule_count++;
	fclose(file_pointer);

 
 	int id ;
	for (id = program ; id <= TK_RNUM ; id++)
	{
		if(isTerminal(id))
			firsts[id] = initnode (id) ;	
		else 
			firsts[id] = NULL;
	}



	for (id = program ; id <= TK_RNUM ; id++)
	{
		if(isNonTerminal(id) && firsts[id] == NULL)
			populateFirst((tokenID)id);	
	}
	
	
	follows[program] = initnode (TK_EOF) ;
	for (id = program + 1 ; id <= idL ; id++)
		follows[id] = NULL ;


	for (id = program + 1 ; id <= idL ; id++)
	{
		if (isNonTerminal(id))
			populateFollow ((tokenID)id) ;
	}

	traverse_grammar() ;
}
 
/*
*******************************************************************************************************************************************************
*******************************************************************************************************************************************************
*******************************************************************************************************************************************************
*******************************************************************************************************************************************************
*/
 
 
treeNode* create_root ()
{
	treeNode *node = (treeNode *) malloc (sizeof(treeNode)) ;
 
	node->tag = NON_TERMINAL ;
	node->tnt.nonTerm = program ;
 
 	node->syntax_error = 0 ;
	node->child = NULL ;
	node->next = NULL ;
	node->parent = NULL ;
	
	node->gcode = -1 ;
	node->syn = NULL ;
	node->inh = NULL ;

	return node ;
}
 
treeNode* create_node (nodeTag tag, TNT tnt)
{
	treeNode *node = (treeNode *) malloc (sizeof(treeNode)) ;

	node->tag = tag ;
 
	node->tnt = tnt ;
	node->child = NULL ;
	node->next = NULL ;
	node->parent = NULL ;


	node->gcode = -5 ; // default value ( should be replaced )
	node->syn = NULL ;
	node->inh = NULL ;

	return node ;
}
 
 
void add_child (treeNode *parent, treeNode *node)
{
	if (parent->child == NULL)
		parent->child = node ;
	else
	{
		treeNode *ptr ;
		ptr = parent->child ;
		while (ptr->next != NULL)
			ptr = ptr->next ;
 
		ptr->next = node ;
	}
 
	node->parent = parent ;
}
 
void print_node (treeNode *node)
{
	if (node->tag == TERMINAL)
		printf ("%d %d %s %d", node->tag, node->tnt.term->id, node->tnt.term->lexeme, node->tnt.term->lineNumber) ;
	else if (node->tag == NON_TERMINAL)
		printf ("%d %d", node->tag, node->tnt.nonTerm) ;
	else
		printf ("INVALID ENUM\n") ;
}
 
void print_siblings (treeNode *node)
{
	treeNode *ptr ;
	printf ("---------------\n") ;
 
	ptr = node ;
	while (ptr != NULL)
	{
		print_node (ptr) ;
		printf("\n") ;
		ptr = ptr->next ;
	}
}
 
stacknode * initStack(){
	stacknode * head = (stacknode* ) malloc(sizeof(stacknode));
	head->key = TK_EOF;
	head->next = NULL;
 
	return head;
}
 

stacknode * push(stacknode * stc , tokenID token){
	if(token == TK_EPS)
		return stc;
 
	stacknode * tmp = (stacknode *) malloc(sizeof(stacknode));
	tmp->key = token;
	tmp->next = stc;
	stc = tmp;
 
	return tmp;
}
 

stacknode * pop(stacknode * stc){
	if(stc==NULL){
		printf("Stack is already empty\n");
		return stc;
	}
		
	
	stacknode * tmp = stc;
	stc = stc->next;
	free(tmp);
	return stc;
}
 


stacknode * pushRule(stacknode * stc,Rule *allRules, int rule_index){
	stc = pop(stc);	// E-> abc - popping E
 
	stacknode * tmp_stack = initStack();
	// tmp_stack->key = -2;
	
	node * tmp = allRules[rule_index].RHS; // pushing a
	tmp_stack->key = tmp->key;
	tmp = tmp->next;
 
	while(tmp!=NULL){ //pushing b then c
		tmp_stack = push(tmp_stack,tmp->key);
		tmp = tmp->next;
	} 
 
	while(tmp_stack){
		stc = push(stc,(tokenID)tmp_stack->key);
		tmp_stack = pop(tmp_stack);
	}
 
	return stc;
}
 

void printSL(stacknode * stc){
	stacknode * tmp = stc;
	while(tmp){
		printf(" %d",tmp->key);
		tmp = tmp->next;
	}
	endl;
}
 
void addRulesParseTree (treeNode *parent, Rule *allRules, int rule_index)
{
	node* ptr = allRules[rule_index].RHS ;
	treeNode* child ; 
	nodeTag tag ;
	TNT tnt ;
 
	while (ptr!=NULL)
	{
		//treeNode* create_node (nodeTag tag, TNT tnt)
		if (isNonTerminal (ptr->key))
		{
			tag = NON_TERMINAL ;
			tnt.nonTerm = ptr->key ;
		}
		else if (isTerminal (ptr->key))
		{
			tag = TERMINAL ;
			tnt.term = (token *) malloc (sizeof(token)) ;
			tnt.term->id = ptr->key ;
			tnt.term->lexeme = NULL ;
			tnt.term->lineNumber = -1 ;
		}
 
		child = create_node (tag, tnt) ;
		add_child (parent, child) ;
		
		ptr = ptr->next ;
	}
}
 
 
 
treeNode * nextTreeNode(treeNode* current_node){
 
 
	if(current_node->tag == TERMINAL)
	{
		if( current_node->next )
		{
			return current_node->next;
		}
		else 
		{
			treeNode* target = current_node->parent;
 
			while( target != NULL && target->next == NULL)
			{	
				target = target->parent;
			}
			if(target == NULL)
				return target;
 
			return target->next;	
		}
	}
	
}

treeNode* parse (FILE *fp)
{
	fillUtilityArray();
	init_parser () ;

 	int rule_index , while_count = 1 ; 	
	twinBuffer *twinBuf = lexer_init (fp) ;
	token* tk ;
 
	stacknode* stack = initStack () ;
	stack = push (stack, program) ;
	treeNode *root = create_root () ;
 	treeNode* current_node ;
	current_node = root ;

	tk = getNextToken (twinBuf) ;
	int flag=0;
	int current=-1;

	while (1)
	{		
		if(tk->id == TK_LEXERROR)
		{
			root->syntax_error=1;
			errLex () ;
			printf ("at line ") ;
			errLine (tk->lineNumber) ;
			printf (" ") ;
			errArrow () ;
			#ifdef _WIN64
				printf (" %s\n", tk->lexeme) ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf (ANSI_BOLD ANSI_RED " %s\n" ANSI_RESET, tk->lexeme) ;
			#endif
		}
		
		if(stack!=NULL && stack->key<0)
		{
			flag=1;
			root->syntax_error=1;
			errParse () ;
			errArrow () ;
			#ifdef _WIN64
				printf ("Invalid symbol on stack\n") ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf(ANSI_BOLD ANSI_RED "Invalid symbol on stack\n" ANSI_RESET);
			#endif
			break;
		}

		if (tk->id == TK_EOF)
		{
			if(stack!= NULL && stack->key==TK_EOF)
				break ;
			
			if(stack==NULL)
			{
				errParse () ;
				errArrow () ;
				#ifdef _WIN64
					printf ("Stack is empty\n") ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf(ANSI_BOLD ANSI_RED "Stack is empty\n" ANSI_RESET);
				#endif
				break;
			}
		}
 
		if (isTerminal(stack->key)) // We are getting a terminal on top of the stack
		{
			if (stack->key == tk->id) // If top of the stack matches with the input symbol
			{
				stack = pop (stack); // Pop the element from top of the stack and move ahead
				if(flag==0) // No error has been encountered yet
				{
					//Add the terminal to the parse tree
					current_node->tnt.term->lexeme =  tk->lexeme ; 
					current_node->tnt.term->lineNumber = tk->lineNumber ;
					current_node=nextTreeNode(current_node);
				}

				// Get the new look-ahead symbol
				tk = getNextToken (twinBuf) ;
			}
			else   // The top of stack is a terminal but doesn't match with the input symbol
			{
				root->syntax_error=1;
				if(current != tk->lineNumber)
				{
					if(stack->key!= TK_SEMICOL && stack->key!= TK_ENDDEF && stack->key!= TK_DRIVERENDDEF)
					{
						errParse () ;
						printf ("at line ") ;
						errLine (tk->lineNumber) ;
						printf (" ") ;
						errArrow () ;
						#ifdef _WIN64
							printf ("%s symbol was expected\n", utility_array[stack->key]) ;
						#endif
						#if defined __linux__ || defined __MACH__
							printf (ANSI_BOLD ANSI_RED "%s" ANSI_RESET " symbol was expected\n", utility_array[stack->key]);
						#endif
						current=tk->lineNumber;
					}
					else
					{
						errParse () ;
						printf ("at line ") ;
						errLine (tk->lineNumber - 1) ;
						printf (" ") ;
						errArrow () ;
						#ifdef _WIN64
							printf ("%s symbol was expected\n", utility_array[stack->key]) ;
						#endif
						#if defined __linux__ || defined __MACH__
							printf (ANSI_BOLD ANSI_RED "%s" ANSI_RESET " symbol was expected\n", utility_array[stack->key]);
						#endif
						current=tk->lineNumber-1;
					}
					
				}
				stacknode* temp=stack;
				if(stack==NULL)
				{
					errParse () ;
					errArrow () ;
					#ifdef _WIN64
						printf ("Stack is empty\n") ;
					#endif
					#if defined __linux__ || defined __MACH__
						printf(ANSI_BOLD ANSI_RED "Stack is empty\n" ANSI_RESET);
					#endif
					break;
				}
				 stack=stack->next;

				free(temp); // Poping the stack
				flag=1; // There is an error
				
			}
		}
		else if (isNonTerminal(stack->key))
		{
			
			rule_index = parsetable[stack->key][tk->id] ;
 
			if (rule_index != -1)
			{
				stack = pushRule (stack , allRules, rule_index) ;
				if(flag==0)
				{
					current_node->gcode = rule_index ;

					addRulesParseTree (current_node , allRules, rule_index);
					current_node = current_node->child ;
 
					if(current_node->tag == TERMINAL && current_node->tnt.term->id == TK_EPS)
					{
						current_node->tnt.term->lineNumber = tk->lineNumber ;
						current_node=nextTreeNode(current_node);
					}
				}
			}
			else
			{
				root->syntax_error=1;
				if(current != tk->lineNumber)
				{
					errParse () ;
					printf ("at line ");
					errLine (tk->lineNumber) ;
					printf ("\n") ;
					current=tk->lineNumber;
				}

				if(stack==NULL)
				{
					errParse () ;
					errArrow () ;
					#ifdef _WIN64
						printf ("Stack is empty\n") ;
					#endif
					#if defined __linux__ || defined __MACH__
						printf(ANSI_BOLD ANSI_RED "Stack is empty\n" ANSI_RESET);
					#endif
					break;
				}
				flag=1;
					while(1)
					{
						node* temp=NULL;
						temp=follows[stack->key];
						while(temp != NULL && temp->key != tk->id)
							temp=temp->next;

						if(stack!=NULL && temp!=NULL)
						{
							stack=pop(stack);
							break;
						}

						else
						{
							if(tk->id != TK_EOF)
							{
								tk=getNextToken(twinBuf);
								
								if(tk->id == TK_LEXERROR)
								{
									root->syntax_error=1;
									errLex () ;
									printf ("at line ") ;
									errLine (tk->lineNumber) ;
									printf (" ") ;
									errArrow () ;
									#ifdef _WIN64
										printf (" %s\n", tk->lexeme) ;
									#endif
									#if defined __linux__ || defined __MACH__
										printf (ANSI_BOLD ANSI_RED " %s\n" ANSI_RESET, tk->lexeme) ;
									#endif
								}
								break;
							}

							if(tk->id == TK_EOF)
								goto labelabc;
							
						}
					}
			}
		}
 
		while_count++ ;
	}

 	labelabc:
 	lexer_destroy (twinBuf) ;

	return root ;
}

void deletePT (treeNode *node)
{
	if (node == NULL)
		return ;

	treeNode *ptr ;
	ptr = node->child ;
	free (node) ;

	while (ptr != NULL)
	{
		deletePT (ptr) ;
		ptr = ptr->next ;
	}
}