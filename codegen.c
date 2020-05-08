#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include "typeChecker.h"
#include "codegen.h"

baseST *realBase ;
int x = 0 ;
int tf = 0 ;
int df = 0 ;
char offsetStr[10] ;

#define codePrint(x, ...) fprintf (fp, x, ##__VA_ARGS__ )

extern int moduleGeneration (astNode *node, int localBase, int rspDepth, moduleST *lst, varST *vst) ;

int get_label()
{
	x++;
	return x;
}

char* getOffsetStr (int offset)
{	
	if (offset > 0)
		sprintf (offsetStr, " - %d", offset) ;
	else if (offset < 0)
		sprintf (offsetStr, " + %d", -offset) ;
	else
		memset (offsetStr, 0, 10) ;

	return offsetStr ;
}

int isIndexStatic (astNode *node)
{
	return isdigit (node->child->tok->lexeme[0]) ;
}

void loadRegLeftLim (varST *searchedVar, char *reg)
{
	if (isLeftLimStatic (searchedVar))
		fprintf (fp, "\t\tMOV %s, %s\n", reg, searchedVar->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(searchedVar->offset-10)) ;
}

void loadRegRightLim (varST *vst, char *reg)
{
	if (isRightLimStatic (vst))
		fprintf (fp, "\t\tMOV %s, %s\n", reg, vst->arrayIndices->tokRight->lexeme) ;		
	else
		fprintf (fp, "\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset-8)) ;
}

void loadArrBase (varST *vst)
{
	if (isVarStaticArr (vst))
	{
		if (vst->varType == VAR_LOCAL)
		{
			fprintf (fp, "\t\tMOV RDI, RBP\n") ;
			fprintf (fp, "\t\tSUB RDI, %d\n", vst->offset) ;			
		}
		else
			fprintf (fp, "\t\tMOV RDI, [RBP%s]\n", getOffsetStr (vst->offset)) ;
	}
	else
		fprintf (fp, "\t\tMOV RDI, [RBP%s]\n", getOffsetStr(vst->offset)) ;
}


int getStaticOffset (varST *vst, astNode *node, int size)
{
	return vst->offset - size*(atoi(node->child->tok->lexeme) - atoi(vst->arrayIndices->tokLeft->lexeme)) ;
}

void boundCheckGeneration (astNode *node, moduleST *lst, varST *vst)
{
	varST *indexVar = NULL ;
	if (!isIndexStatic (node))	
		indexVar = searchVar (realBase, lst, node->child->tok->lexeme) ;

	tf |= 1 << boundCheck ;

	loadArrBase (vst) ;
	loadRegLeftLim (vst, "AX") ;

	if (indexVar == NULL)
		fprintf (fp, "\t\tMOV BX, %s\n", node->child->tok->lexeme) ;
	else
		fprintf (fp, "\t\tMOV BX, [RBP%s]\n", getOffsetStr (indexVar->offset)) ;

	loadRegRightLim (vst, "CX") ;

	fprintf (fp, "\t\tCALL boundCheck\n\n") ;
}

void assignGeneration (astNode *node, moduleST *lst)
{
	if(node->child == NULL)
	{
		varST *searchedVar = searchVar(realBase, lst, node->tok->lexeme) ;
		fprintf (fp, "\t\tPOP AX\n");
		if (node->next->id == TK_MINUS && node->next->child->next == NULL)
			fprintf (fp, "\t\tNEG AX\n") ;

		fprintf (fp, "\t\tMOV [RBP%s],AX\t\t\t\t\t\t\t\t;Store\n", getOffsetStr(searchedVar->offset)) ;	
	}
	else
	{
		varST* vst ;
		vst = searchVar(realBase, lst,node->tok->lexeme);

		if (isVarStaticArr(vst) && node->child->id == TK_NUM)
		{
			fprintf (fp, "\t\tPOP AX\n");	
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				fprintf (fp, "\t\tNEG AX\n") ;

			fprintf (fp, "\t\tMOV [RBP%s], AX\t\t\t\t\t\t\t\t;Store\n" , getOffsetStr(getStaticOffset(vst,node,2))) ;
		}
		else
		{
			boundCheckGeneration (node, lst, vst) ;
			fprintf (fp, "\t\tPOP AX\n") ;
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				fprintf (fp, "\t\tNEG AX\n") ;

			fprintf (fp, "\t\tMOV [RDI + RBX], AX\n") ;
		}
	}
}

void exprLeafGeneration (astNode *node, moduleST *lst)
{
	char reg[3] ;
	if (node->prev == NULL)
		strcpy (reg, "AX") ;
	else
		strcpy (reg, "BX") ;

	switch (node->id)
	{
		case TK_ID :
			if (node->child == NULL)
			{
				varST *searchedVar = searchVar(realBase, lst, node->tok->lexeme) ;

				fprintf (fp, "\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(searchedVar->offset)) ;
				fprintf (fp, "\t\tPUSH %s\n", reg);	
			}
			else
			{
				varST *vst ;
				vst = searchVar (realBase, lst,node->tok->lexeme) ;

				if (isVarStaticArr(vst) && node->child->id == TK_NUM)
					fprintf (fp, "\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(getStaticOffset (vst, node, 2))) ;						
				else
				{
					boundCheckGeneration (node, lst, vst) ;
					fprintf (fp, "\t\tMOV %s, [RDI + RBX]\n", reg) ;
				}
				
				fprintf (fp, "\t\tPUSH %s\n", reg);	
			}

			break ;

		case TK_RNUM :
			printf ("CODEGEN ERROR : NO FLOATING POINT ALLOWED!\n") ;
			exit (0) ;
			break ;

		case TK_NUM : 
			fprintf (fp, "\t\tMOV %s, %s\n", reg, node->tok->lexeme);
			fprintf (fp, "\t\tPUSH %s\n", reg);	
			break ;

		case TK_TRUE :
			fprintf (fp, "\t\tMOV %s, 1\n", reg);
			fprintf (fp, "\t\tPUSH %s\n", reg);	
			break ;

		case TK_FALSE :
			fprintf (fp, "\t\tMOV %s, 0\n", reg);
			fprintf (fp, "\t\tPUSH %s\n", reg);	
			break ;
	}
}

void exprGeneration (astNode *node, moduleST *lst)
{
	if (node->child == NULL || node->child->next == NULL)
		exprLeafGeneration (node, lst) ;

	switch (node->id)
	{
		case TK_PLUS :
			exprGeneration (node->child, lst); 
			exprGeneration (node->child->next, lst) ;
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tADD AX,BX\n");
			fprintf (fp, "\t\tPUSH AX\n");

			break ;

		case TK_MINUS :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tSUB AX,BX\n");
			fprintf (fp, "\t\tPUSH AX\n");

			break ;

		case TK_MUL :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tIMUL BX\n");
			fprintf (fp, "\t\tPUSH AX\n");

			break ;

		case TK_DIV :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tcwd\n") ;
			fprintf (fp, "\t\tIDIV BX\n");
			fprintf (fp, "\t\tPUSH AX\n");
			break ;

		case TK_LT : case TK_GT : case TK_LE : case TK_GE : case TK_NE : case TK_EQ :
			exprGeneration (node->child, lst) ;
			exprGeneration (node->child->next, lst) ;

			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tCMP AX,BX\n");

			switch (node->id)
			{
				case TK_LT :
					fprintf (fp, "\t\tSETL AL\n");
					break ;
				case TK_GT :
					fprintf (fp, "\t\tSETG AL\n");
					break ;
				case TK_LE :
					fprintf (fp, "\t\tSETLE AL\n");
					break ;
				case TK_GE :
					fprintf (fp, "\t\tSETGE AL\n");
					break ;
				case TK_NE :
					fprintf (fp, "\t\tSETNE AL\n");
					break ;
				case TK_EQ :
					fprintf (fp, "\t\tSETE AL\n");
					break ;
			}
			fprintf (fp, "\t\tMOVSX AX, AL\n") ;
			fprintf (fp, "\t\tPUSH AX\n") ;

			break ;

		case TK_AND : case TK_OR :
			exprGeneration (node->child, lst) ;
			exprGeneration (node->child->next, lst) ;

			fprintf (fp, "\t\tPOP BX\n");
			fprintf (fp, "\t\tPOP AX\n"); 

			if (node->id == TK_AND)
				fprintf (fp, "\t\tAND AX, BX\n");
			else
				fprintf (fp, "\t\tOR AX, BX\n");

			fprintf (fp, "\t\tPUSH AX\n") ;

			break ;
	}
}

void printGeneration (astNode *node, moduleST *lst)
{
	int start_label, end_label ;
	int reserveLabel[2] ;

	varST *searchedVar ;
	if (node->id == TK_ID)
		searchedVar = searchVar (realBase, lst, node->tok->lexeme) ;
	else
	{
		if (node->id == TK_NUM)
		{
			tf |= 1 << printInteger ;
			fprintf (fp, "\t\tMOV SI, %s\n", node->tok->lexeme) ;
			fprintf (fp, "\t\tCALL printInteger\n\n") ;
		}
		else
		{
			tf |= 1 << printBoolean ;
			fprintf (fp, "\t\tMOV SI, %d\n", node->tok->lexeme[0]=='t'?1:0) ;
			fprintf (fp, "\t\tCALL printBoolean\n\n") ;
		}

		return ;
	}

	if (searchedVar->datatype == TK_INTEGER)
	{
		tf |= 1 << printInteger ;
		fprintf (fp, "\t\tMOV SI, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
		fprintf (fp, "\t\tCALL printInteger\n\n") ;
	}
	else if (searchedVar->datatype == TK_BOOLEAN)
	{
		tf |= 1 << printBoolean ;
		fprintf (fp, "\t\tMOV SI, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
		fprintf (fp, "\t\tCALL printBoolean\n\n") ;
	}
	else // Array type
	{	
		if (node->child != NULL)
		{
			// Special Case
			if (node->child->id == TK_NUM && isVarStaticArr (searchedVar))
				fprintf (fp, "\t\tMOV SI, [RBP%s]\n", getOffsetStr(getStaticOffset(searchedVar,node,2))) ;
			else
			{
				// General case of static array with dynamic indices, or dynamic array
				boundCheckGeneration (node, lst, searchedVar) ;
				fprintf (fp, "\t\tMOV SI, [RDI + RBX]\n") ;
			}

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printInteger ;
				fprintf (fp, "\t\tCALL printInteger\n\n") ;
			}
			else
			{
				tf |= 1 << printBoolean ;
				fprintf (fp, "\t\tCALL printBoolean\n\n") ;
			}
		}
		else
		{
			loadArrBase (searchedVar) ;
			loadRegLeftLim (searchedVar, "CX") ;
			loadRegRightLim (searchedVar, "DX") ;
			fprintf (fp, "\t\tSUB DX, CX\n") ;
			fprintf (fp, "\t\tADD DX, 1\n") ;
			fprintf (fp, "\t\tADD DX, DX\n") ;

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printIntegerArr ;
				fprintf (fp, "\t\tCALL printIntegerArr\n\n") ;
			}
			else
			{
				tf |= 1 << printBooleanArr ;
				fprintf (fp, "\t\tCALL printBooleanArr\n\n") ;
			}
		}
	}
}

void getValueRSPAlign()
{
	fprintf (fp, "\t\tMOV RAX, RSP\n") ;
	fprintf (fp, "\t\tAND RAX, 15\n") ;
	fprintf (fp, "\t\tADD RAX, 8\n") ;
	fprintf (fp, "\t\tSUB RSP, RAX\n") ;
	fprintf (fp, "\t\tPUSH RAX\n") ;
}

void getValueGeneration (moduleST *lst, varST *searchedVar, int rspDepth)
{		
	int rspAlign ;
	df |= 1 << inputInt ;

	if (searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN)
	{
		if (searchedVar->datatype == TK_INTEGER)
		{
			df |= 1 << inputIntPrompt ;
			fprintf (fp, "\n\t\tMOV RDI, inputIntPrompt\t\t\t\t\t\t\t\t; get_value\n") ;
		}
		else if (searchedVar->datatype == TK_BOOLEAN)
		{
			df |= 1 << inputBoolPrompt ;
			fprintf (fp, "\n\t\tMOV RDI, inputBoolPrompt\t\t\t\t\t\t\t\t; get_value\n") ;
		}

		tf |= 1 << getValuePrimitive ;
		fprintf (fp, "\t\tMOV RBX, -%d\n", searchedVar->offset) ;
		fprintf (fp, "\t\tCALL getValuePrimitive\n\n") ;		
 	}
	else // Array type
	{	
		if (searchedVar->arrayIndices->type == TK_INTEGER)
		{
			df |= 1 << inputIntArrPrompt ;
			fprintf (fp, "\t\tMOV RDI, inputIntArrPrompt\n") ;
		}
		else
		{
			df |= 1 << inputBoolArrPrompt ;
			fprintf (fp, "\t\tMOV RDI, inputBoolArrPrompt\n") ;
		}

		tf |= 1 << printGetArrPrompt ;
		tf |= 1 << getArr ;

		loadRegLeftLim (searchedVar, "BX") ;
		loadRegRightLim (searchedVar, "CX") ;
		fprintf (fp, "\t\tCALL printGetArrPrompt\n\n") ;

		loadArrBase (searchedVar) ;
		fprintf (fp, "\t\tMOV DX, CX\n") ;
		fprintf (fp, "\t\tSUB DX, BX\n") ;
		fprintf (fp, "\t\tADD DX, 1\n") ;
		fprintf (fp, "\t\tADD DX, DX\n") ;
		fprintf (fp, "\t\tMOVSX RDX, DX\n") ;
		fprintf (fp, "\t\tCALL getArr\n\n") ;
	}
}

void dynamicDeclareCheck (moduleST *lst, varST *searchedVar)
{
	int start_label , end_label ;
	varST *leftVar , *rightVar ;
	tf |= 1 << dynamicDeclCheck ;

	fprintf (fp, "\n") ;
	if (isLeftLimStatic (searchedVar))
		fprintf (fp, "\t\tMOV AX, %s\n", searchedVar->arrayIndices->tokLeft->lexeme) ;
	else
	{
		leftVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokLeft->lexeme) ;
		//fprintf (fp, "\t\tMOV AX, [RBP%s]\n", getOffsetStr(leftVar->offset)) ;
		codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr (leftVar->offset)) ;
	}

	if (isRightLimStatic (searchedVar))
		fprintf (fp, "\t\tMOV BX, %s\n", searchedVar->arrayIndices->tokRight->lexeme) ;
	else
	{
		rightVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokRight->lexeme) ;
		fprintf (fp, "\t\tMOV BX, [RBP%s]\n", getOffsetStr(rightVar->offset)) ;
	}

	fprintf (fp, "\t\tCALL dynamicDeclCheck\n\n") ;
}

void dynamicDeclareGeneration (moduleST *lst, varST *searchedVar, int declCount)
{
	int start_label ;
	dynamicDeclareCheck (lst, searchedVar) ;
	fprintf (fp, "\t\tPUSH BX\t\t\t\t\t\t\t\t;  saving register for malloc\n") ;
	fprintf (fp, "\t\tPUSH AX\t\t\t\t\t\t\t\t;  saving register for malloc\n") ;

	if (declCount > 1)
	{
		fprintf (fp, "\n\t\tMOV CX, 0\n") ;
		fprintf (fp, "\t\tMOV RBX, %d\n", searchedVar->offset) ;
		fprintf (fp, "\t\tNEG RBX\n") ;
		fprintf (fp, "\n\t.declLoop:\n") ;

		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tCALL malloc\n") ;
		fprintf (fp, "\t\tPOP RDI\n") ;
		fprintf (fp, "\t\tPOP RBX\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tMOV [RBP+RBX], RAX\n") ;

		fprintf (fp, "\t\tPOP AX\n") ;
		fprintf (fp, "\t\tMOV [RBP + RBX + 10], AX\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tMOV [RBP + RBX + 8], DX\n") ;

		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tPUSH AX\n") ;
		fprintf (fp, "\t\tADD RBX, 12\n") ;
		fprintf (fp, "\t\tINC CX\n") ;
		fprintf (fp, "\t\tCMP CX, %d\n", declCount) ;
		fprintf (fp, "\t\tJL .declLoop\n") ;

		fprintf (fp, "\t\tADD RSP, 4\t\t\t\t\t\t\t\t;  Restoring array limits\n");
	}
	else
	{
		fprintf (fp, "\t\tCALL malloc\n") ;
		fprintf (fp, "\t\tMOV [RBP%s], RAX\n", getOffsetStr(searchedVar->offset)) ;
		fprintf (fp, "\t\tPOP AX\n") ;
		fprintf (fp, "\t\tMOV [RBP%s], AX\n", getOffsetStr(searchedVar->offset - 10)) ;
		fprintf (fp, "\t\tPOP BX\n") ;
		fprintf (fp, "\t\tMOV [RBP%s], BX\n\n", getOffsetStr(searchedVar->offset - 8)) ;
	}
}

void preamble()
{
	fprintf (fp, "extern printf\n") ;
	fprintf (fp, "extern scanf\n") ;
	fprintf (fp, "extern malloc\n") ;
	fprintf (fp, "extern exit\n") ;

	fprintf (fp, "\nglobal main\n") ;
	fprintf (fp, "section .text\n") ;
}


int isFlagSet (int flagInt , int id)
{
	int test = flagInt ;
	test = test >> id ;

	return test % 2 ? 1 : 0 ;
}


void postamble()
{
	if (isFlagSet (tf, boundCheck))
	{
		tf |= 1 << boundERROR ;

		fprintf (fp, "\nboundCheck:\n") ;
		fprintf (fp, "\t\tCMP BX, AX\n") ;
		fprintf (fp, "\t\tJGE .leftLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;

		fprintf (fp, "\n\t.leftLim:\n") ;
		fprintf (fp, "\t\tCMP CX, BX\n") ;
		fprintf (fp, "\t\tJGE .rightLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;

		fprintf (fp, "\n\t.rightLim:\n") ;
		fprintf (fp, "\t\tSUB BX, AX\n") ;
		fprintf (fp,"\t\tADD BX, BX\n") ;
		fprintf (fp, "\t\tMOVSX RBX, BX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, dynamicBoundCheck))
	{
		tf |= 1 << boundERROR ;

		fprintf (fp, "\ndynamicBoundCheck:\n") ;
		fprintf (fp, "\t\tCMP BX, AX\n") ;
		fprintf (fp, "\t\tJGE .leftLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;

		fprintf (fp, "\n\t.leftLim:\n") ;
		fprintf (fp, "\t\tCMP CX, BX\n") ;
		fprintf (fp, "\t\tJGE .rightLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;
		
		fprintf (fp, "\n\t.rightLim:\n") ;
		fprintf (fp, "\t\tSUB BX, AX\n") ;
		fprintf (fp, "\t\tADD BX, BX\n") ;
		fprintf (fp, "\t\tMOVSX RBX, BX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, dynamicDeclCheck))
	{
		tf |= 1 << declNegERROR ;
		tf |= 1 << declERROR ;

		fprintf (fp, "\ndynamicDeclCheck:\n") ;
		fprintf (fp, "\t\tCMP AX, 0\n") ;
		fprintf (fp, "\t\tJGE .leftNotNeg\n") ;
		fprintf (fp, "\t\tCALL declNegERROR\n") ;

		fprintf (fp, "\n\t.leftNotNeg:\n") ;
		fprintf (fp, "\t\tCMP BX, 0\n") ;
		fprintf (fp, "\t\tJGE .rightNotNeg\n") ;
		fprintf (fp, "\t\tCALL declNegERROR\n") ;

		fprintf (fp, "\n\t.rightNotNeg:\n") ;
		fprintf (fp, "\t\tCMP BX, AX\n") ;
		fprintf (fp, "\t\tJGE .dynChecked\n") ;
		fprintf (fp, "\t\tCALL declERROR\n") ;
		fprintf (fp, "\n\t.dynChecked:\n") ;
		fprintf (fp, "\t\tMOV DX, BX\n") ;
		fprintf (fp, "\t\tSUB DX, AX\n") ;
		fprintf (fp, "\t\tADD DX, 1\n") ;
		fprintf (fp, "\t\tADD DX, DX\n") ;
		fprintf (fp, "\t\tMOVSX RDI, DX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getValuePrimitive))
	{
		fprintf (fp, "\ngetValuePrimitive:\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP RBX\n\n") ;

		getValueRSPAlign (fp) ;

		fprintf (fp, "\t\tMOV RDI, inputInt\t\t\t\t\t\t\t\t; get_value\n") ;
		fprintf (fp, "\t\tMOV RSI, RSP\n") ;
		fprintf (fp, "\t\tSUB RSI, 16\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RSI\n") ;
		fprintf (fp, "\t\tCALL scanf\n") ;
		fprintf (fp, "\t\tPOP RSI\n") ;
		fprintf (fp, "\t\tPOP RBX\n") ;
		fprintf (fp, "\t\tMOV AX, [RSP - 16]\n") ;
		fprintf (fp, "\t\tMOV [RBP + RBX], AX\n") ;

		fprintf (fp, "\n\t\tPOP RAX\n") ;
		fprintf (fp, "\t\tADD RSP, RAX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printGetArrPrompt))
	{
		df |= 1 << leftRange ;
		df |= 1 << rightRange ;

		fprintf (fp, "\nprintGetArrPrompt:\n") ;
		fprintf (fp, "\t\tMOV SI, CX\n") ;
		fprintf (fp, "\t\tSUB SI, BX\n") ;
		fprintf (fp, "\t\tADD SI, 1\n") ; 
		fprintf (fp, "\t\tMOVSX RSI, SI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH BX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tPOP BX\n") ;

		fprintf (fp, "\n\t\tMOV RDI, leftRange\n") ;
		fprintf (fp, "\t\tMOVSX RSI, BX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH BX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tPOP BX\n") ;

		fprintf (fp, "\n\t\tMOV RDI, rightRange\n") ;
		fprintf (fp, "\t\tMOVSX RSI, CX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH BX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tPOP BX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getArr))
	{
		fprintf (fp, "\ngetArr:\n");
		getValueRSPAlign (fp) ;

		fprintf (fp, "\n\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tMOV RCX, 0\n") ;
		fprintf (fp, "\n\t.getArr:\t\t\t\t\t\t\t\t; getting array\n") ;

		fprintf (fp, "\t\tMOV RDI, inputInt\t\t\t\t\t\t\t\t; get_value\n") ;
		fprintf (fp, "\t\tMOV RSI, RSP\n") ;
		fprintf (fp, "\t\tSUB RSI, 24\n") ;
		fprintf (fp, "\t\tPUSH RCX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tPUSH RSI\n") ;
		fprintf (fp, "\t\tCALL scanf\n") ;
		fprintf (fp, "\t\tPOP RSI\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RCX\n") ;

		fprintf (fp, "\t\tMOV RBX, RCX\n") ;
		fprintf (fp, "\t\tMOV AX, [RSP - 24]\n") ;
		fprintf (fp, "\t\tPOP RDI\n") ;
		fprintf (fp, "\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tMOV [RDI + RBX], AX\n") ;

		fprintf (fp, "\n\t\tADD RCX, 2\n") ;	// Incrementing counter
		fprintf (fp, "\t\tCMP RCX, RDX\n") ;
		fprintf (fp, "\t\tJNE .getArr\n\n") ;

		fprintf (fp, "\t\tPOP RDI\n") ;
		fprintf (fp, "\t\tPOP RAX\n") ;
		fprintf (fp, "\t\tADD RSP, RAX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printInteger))
	{
		df |= 1 << printFormat ;

		fprintf (fp, "\nprintInteger:\n") ;
		fprintf (fp, "\t\tMOV RDI, printFormat\n") ;
		fprintf (fp, "\t\tMOVSX RSI, SI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf , printBoolean))
	{
		df |= 1 << printFalse ;
		df |= 1 << printTrue ;

		fprintf (fp, "\nprintBoolean:\n") ;
		fprintf (fp, "\t\tCMP SI, 0\n") ;
		fprintf (fp, "\t\tJE .false\n") ;
		fprintf (fp, "\t\tMOV RDI, printTrue\n") ;
		fprintf (fp, "\t\tJMP .boolPrint\n") ;

		fprintf (fp, "\n\t.false:\n") ;
		fprintf (fp, "\t\tMOV RDI, printFalse\n") ;
		fprintf (fp, "\n\t.boolPrint:\n") ;

		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printIntegerArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << printNewLine ;
		df |= 1 << printInt ;


		fprintf (fp, "\nprintIntegerArr:\n") ;
		fprintf (fp, "\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tMOV RDI, printFormatArray\t\t\t\t\t\t\t\t; printing array output prompt\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tPOP RDI\n\n") ;

		fprintf (fp, "\t\tMOV CX, 0\n") ;
		fprintf (fp, "\n\t.printArr:\t\t\t\t\t\t\t\t; printing array\n") ;

		fprintf (fp, "\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tMOVSX RBX, CX\n") ;
		fprintf (fp, "\t\tMOV SI, [RDI + RBX]\n") ;
		fprintf (fp, "\t\tMOV RDI, printInt\n") ;
		fprintf (fp, "\t\tMOVSX RSI, SI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tPOP RDI\n\n") ;

		fprintf (fp, "\t\tADD CX, 2\n") ;
		fprintf (fp, "\t\tCMP CX, DX\n") ;
		fprintf (fp, "\t\tJNE .printArr\n\n") ;

		fprintf (fp, "\t\tMOV RDI, printNewLine\t\t\t\t\t\t\t\t; newline after array print\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printBooleanArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << DATA_true ;
		df |= 1 << DATA_false ;
		df |= 1 << printNewLine ;

		fprintf (fp, "\nprintDynamicBooleanArr:\n") ;
		fprintf (fp, "\n\t\tMOV RDI, printFormatArray\t\t\t\t\t\t\t\t; printing array output prompt\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tPOP RDI\n\n") ;

		fprintf (fp, "\t\tMOV CX, 0\n") ;
		fprintf (fp, "\n\t.printArr:\t\t\t\t\t\t\t\t; printing array\n") ;

		fprintf (fp, "\t\tMOVSX RBX, CX\n") ;
		fprintf (fp, "\t\tMOV AX, [RDI + RBX]\n") ;

		fprintf (fp, "\n\t\tCMP AX, 0\n") ;
		fprintf (fp, "\t\tJE .false\n") ;
		fprintf (fp, "\t\tMOV RDI, true\n") ;
		fprintf (fp, "\t\tJMP .afterBool\n") ;
		
		fprintf (fp, "\n\t.false:\n") ;
		fprintf (fp, "\t\tMOV RDI, false\n") ;
		fprintf (fp, "\n\t.afterBool:\n") ;

		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP CX\n\n") ;

		fprintf (fp, "\t\tADD CX, 2\n") ;
		fprintf (fp, "\t\tCMP CX, DX\n") ;
		fprintf (fp, "\t\tJNE .printArr\n\n") ;

		fprintf (fp, "\t\tMOV RDI, printNewLine\t\t\t\t\t\t\t\t; newline after array print\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, boundERROR))
	{
		df |= 1 << boundPrint ;

		fprintf (fp, "\nboundERROR:\n") ;
		fprintf (fp, "\t\tMOV RDI, boundPrint\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tMOV EDI, 0\n") ;
		fprintf (fp, "\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, declERROR))
	{
		df |= 1 << declPrint ;

		fprintf (fp, "\ndeclERROR:\n") ;
		fprintf (fp, "\t\tMOV RDI, declPrint\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tMOV EDI, 0\n") ;
		fprintf (fp, "\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, declNegERROR))
	{
		df |= 1 << declNeg ;

		fprintf (fp, "\ndeclNegERROR:\n") ;
		fprintf (fp, "\t\tMOV RDI, declNeg\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tMOV EDI, 0\n") ;
		fprintf (fp, "\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, argLimERROR))
	{
		df |= 1 << arrArgMismatch ;

		fprintf (fp, "\nargLimERROR:\n") ;
		fprintf (fp, "\t\tMOV RDI, arrArgMismatch\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tMOV EDI, 0\n") ;
		fprintf (fp, "\t\tCALL exit\n") ;
	}

	/* --------------------------------------------------------------------------------------------- */
	printCommentLineNASM (fp) ;

	if (df)
		fprintf (fp, "\nsection .data\n") ;

	if (isFlagSet (df, boundPrint))
	{
		fprintf (fp, "\t\tboundPrint : ") ;
		fprintf (fp, "db \"RUNTIME ERROR : Array out of bounds\" , 10, 0\n") ;	
	}
	
	if (isFlagSet (df, declPrint))
	{
		fprintf (fp, "\t\tdeclPrint : ") ;
		fprintf (fp, "db \"RUNTIME ERROR : Invalid order of bounds in dynamic array declaration\" , 10, 0\n") ;		
	}
	
	if (isFlagSet (df, declNeg))
	{
		fprintf (fp, "\t\tdeclNeg : ") ;
		fprintf (fp, "db \"RUNTIME ERROR : Negative bound in dynamic array declaration\" , 10, 0\n") ;	
	}

	if (isFlagSet (df, arrArgMismatch))
	{
		fprintf (fp, "\t\tarrArgMismatch: ") ;
		fprintf (fp, "db \"RUNTIME ERROR : Mismatch in formal and actual array limits\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printFormatArray))
	{
		fprintf (fp, "\t\tprintFormatArray : ") ;
		fprintf (fp, "db \"Output : \" , 0\n") ;
	}

	if (isFlagSet (df, printInt))
	{
		fprintf (fp, "\t\tprintInt : ") ;
		fprintf (fp, "db \"%%d \", 0\n") ;
	}

	if (isFlagSet (df, printNewLine))
	{
		fprintf (fp, "\t\tprintNewLine : ") ;
		fprintf (fp, "db 10, 0\n") ;
	}

	if (isFlagSet (df, printFormat))
	{
		fprintf (fp, "\t\tprintFormat : ") ;
		fprintf (fp, "db \"Output :  %%d\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printTrue))
	{
		fprintf (fp, "\t\tprintTrue : ") ;
		fprintf (fp, "db \"Output : true\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printFalse))
	{
		fprintf (fp, "\t\tprintFalse : ") ;
		fprintf (fp, "db \"Output : false\" , 10, 0\n") ;
	}

	if (isFlagSet (df, DATA_true))
	{
		fprintf (fp, "\t\ttrue : ") ;
		fprintf (fp, "db \"true \" , 0\n") ;
	}

	if (isFlagSet (df, DATA_false))
	{
		fprintf (fp, "\t\tfalse : ") ;
		fprintf (fp, "db \"false \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntPrompt))
	{
		fprintf (fp, "\t\tinputIntPrompt : ") ;
		fprintf (fp, "db \"Enter an integer : \" , 0\n") ;
	}

	if (isFlagSet (df, inputBoolPrompt))
	{
		fprintf (fp, "\t\tinputBoolPrompt : ") ;
		fprintf (fp, "db \"Enter a boolean (0 or 1) : \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntArrPrompt))
	{
		fprintf (fp, "\t\tinputIntArrPrompt : ") ;
		fprintf (fp, "db \"Enter %%d array elements of integer type for range \", 0\n") ;
	}

	if (isFlagSet (df, inputBoolArrPrompt))
	{
		fprintf (fp, "\t\tinputBoolArrPrompt : ") ;
		fprintf (fp, "db \"Enter %%d array elements of boolean type for range \", 0\n") ;
	}

	if (isFlagSet (df, leftRange))
	{
		fprintf (fp, "\t\tleftRange : ") ;
		fprintf (fp, "db \"%%d to \" , 0\n") ;
	}

	if (isFlagSet (df, rightRange))
	{
		fprintf (fp, "\t\trightRange : ") ;
		fprintf (fp, "db \"%%d\" ,10, 0\n") ;
	}

	if (isFlagSet (df, inputInt))
	{
		fprintf (fp, "\t\tinputInt : ") ;
		fprintf (fp, "db \"%%d\", 0\n") ;
	}
}

/*	---------------------------------- SWITCH CASE ------------------------------------- */

int getCaseCount (astNode *statementsNode)
{
	int cnt = 0 ;

	while (statementsNode != NULL)
	{
		cnt++ ;

		if (statementsNode->next != NULL)
		{
			if (statementsNode->next->id == TK_DEFAULT)
				return cnt ;

			statementsNode = statementsNode->next->next->next ;
		}
		else
			statementsNode = NULL ;
	}

	return cnt ;
}


int switchDeclareVars (astNode *statementsNode, varST *vst , int rspDepth)
{
	astNode *statementNode ;

	// Reserving space for switch scope variables
	while (statementsNode != NULL)
	{
		statementNode = statementsNode->child ;
		while (statementNode != NULL)
		{
			if (statementNode->child->id == TK_DECLARE)
			{
				rspDepth = moduleGeneration (statementNode->child, rspDepth, rspDepth, statementsNode->localST, vst) ;
				statementNode->child->parent = statementNode->child ;		// tieing a knot
			}
			statementNode = statementNode->next ;
		}

		if (statementsNode->next != NULL)
		{
			if (statementsNode->next->id == TK_DEFAULT)
			{
				statementsNode = statementsNode->next->next ;
				statementNode = statementsNode->child ;
				while (statementNode != NULL)
				{
					if (statementNode->child->id == TK_DECLARE)
					{
						rspDepth = moduleGeneration (statementNode->child, rspDepth, rspDepth, statementsNode->localST, vst) ;
						statementNode->child->parent = statementNode->child ;		// tieing a knot
					}
					statementNode = statementNode->next ;
				}

				break ;
			}
			statementsNode = statementsNode->next->next->next ;
		}
		else
			statementsNode = NULL ;
	}

	return rspDepth ;
}

int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels)
{
	int i, def_label = -1 ;

	for (i = 0 ; i < caseCount ; i++)
		caseLabels[i] = get_label() ;

	varST *switchVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
	fprintf (fp, "\n\t\tMOV AX, [RBP%s]\n", getOffsetStr(switchVar->offset)) ;

	astNode *caseValNode =  node->next->next->next ;
	i = 0 ;

	while (caseValNode != NULL)
	{
		fprintf (fp, "\n\t\tCMP AX, %s\n", (switchVar->datatype == TK_INTEGER)?caseValNode->tok->lexeme:((caseValNode->tok->lexeme[0] == 't')?"1":"0")) ;
		fprintf (fp, "\t\tJE LABEL%d\n", caseLabels[i]) ;

		if (caseValNode->next->next != NULL)
		{
			if (caseValNode->next->next->id == TK_DEFAULT)		
			{
				def_label = get_label () ;
				fprintf (fp, "\n\t\tJMP LABEL%d\n", def_label) ;
				break ;
			}

			i++ ;
			caseValNode = caseValNode->next->next->next ;
		}
		else
			caseValNode = NULL ;
	}

	return def_label ;
}

int moduleGeneration (astNode *node, int localBase, int rspDepth, moduleST *lst, varST *vst)
{
	if (node == NULL)
		return 0 ;

	varST *searchedVar ;
	astNode *idListHead ;
	moduleST *calledModule ;

	switch (node->id)
	{
		int start_label, end_label ;
		int reserveLabel[10] ;

		case statements :
			moduleGeneration (node->child, localBase, rspDepth, node->localST, vst);		// Access local scope and move below
			break ;

		case statement :
			rspDepth = moduleGeneration(node->child, localBase, rspDepth, lst, vst);
			if (node->next == NULL)
			{
				if (lst->tableType == SWITCH_ST)
					break ;

				if (lst->parent == realBase)
				{
					fprintf (fp, "\n\t\tMOV RSP, RBP\n") ;
					fprintf (fp, "\t\tPOP RBP\n") ;
					fprintf (fp, "\t\tret\n") ;
				}
				else if (rspDepth - localBase > 0)
					fprintf (fp, "\t\tADD RSP, %d\t\t\t\t\t\t\t\t; Restoring RSP to previous scope\n", (rspDepth-localBase)) ;
			}
			else
				moduleGeneration(node->next, localBase, rspDepth, lst, vst);

			break ;

		case TK_DECLARE :
			;
			int switchPass = 0 ;
			if (lst->tableType != SWITCH_ST)
				; // Subtract RSP, and allocate memory if dynamic
			else if (node->parent != node)
				switchPass = 1 ; // Subtract RSP only, and do not allocate memory for dynamic
			else
			{
				switchPass = 2 ; // Do not subtract RSP, allocate memory if dynamic AND restore parent
				node->parent = node->next->parent ;
			}

			int endOffset, declCount = 1 ;
			idListHead = node->next->child ;
			astNode *dtNode = node->next->next ;

			while (idListHead->next != NULL)
			{
				declCount++ ;
				idListHead = idListHead->next ;
			}

			searchedVar = searchVar (realBase, lst, idListHead->tok->lexeme) ;
			endOffset = searchedVar->offset ;
			if (endOffset > 0 && (switchPass == 0 || switchPass == 1))
			{	
				fprintf (fp, "\t\tSUB RSP, %d\t\t\t\t\t\t\t\t; Updating RSP\n", (endOffset - rspDepth)) ;
				rspDepth = endOffset ;
			}

			if (dtNode->dtTag == ARRAY && !isVarStaticArr(searchedVar) && (switchPass == 0 || switchPass == 2))
				dynamicDeclareGeneration (lst, searchedVar, declCount) ;

			
			break ;											

		case TK_ASSIGNOP :
			if ((node->child->next->id == TK_MINUS || node->child->next->id == TK_PLUS) && node->child->next->child->next == NULL)
				exprGeneration(node->child->next->child, lst);
			else
				exprGeneration(node->child->next, lst);

			assignGeneration (node->child, lst) ;
			//moduleGeneration(node->child, localBase, rspDepth, lst, vst);

			fprintf (fp, "\n") ;
			break ;

		case TK_FOR :
			node=node->next;
			start_label = get_label () ;
			end_label = get_label () ;

			searchedVar = searchVar(realBase, lst, node->tok->lexeme) ;

			fprintf (fp, "\t\tMOV CX,%s\n", node->next->tok->lexeme);
			fprintf (fp, "\t\tMOV [RBP%s], CX\t\t\t\t\t\t\t\t; for loop lower lim\n" , getOffsetStr(searchedVar->offset)) ;

			fprintf (fp, "\n\tFOR%d:\n", start_label) ;
			fprintf (fp, "\t\tMOV AX, %s\n", node->next->next->tok->lexeme) ;
			fprintf (fp, "\t\tCMP CX,AX\n");
			fprintf (fp, "\t\tJG FOR%d\n\n", end_label);

			moduleGeneration(node->next->next->next, rspDepth, rspDepth, lst, vst);		// Statements

			fprintf (fp, "\n\t\tMOV CX, [RBP%s]\t\t\t\t\t\t\t\t; Ending increment\n", getOffsetStr(searchedVar->offset)) ;
			fprintf (fp, "\t\tINC CX\n");
			fprintf (fp, "\t\tMOV [RBP%s],CX\n", getOffsetStr(searchedVar->offset)) ;
			fprintf (fp, "\t\tJMP FOR%d\n", start_label) ;
			fprintf (fp, "\n\tFOR%d:\n", end_label) ;
			break ;

		
		case TK_WHILE :
			node=node->next;
			start_label = get_label();
			end_label =  get_label();

			fprintf (fp, "\n\tWHILE%d:\n", start_label) ;

			exprGeneration (node, lst);	// expression

			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tCMP AX, 0\n");
			fprintf (fp, "\t\tJE WHILE%d\n\n", end_label) ;

			moduleGeneration(node->next, rspDepth, rspDepth, lst, vst);		// statements

			fprintf (fp, "\t\tJMP WHILE%d\n", start_label) ;
			fprintf (fp, "\n\tWHILE%d:\n", end_label) ;

			break ;
		
		case TK_PRINT :
			node = node->next ;
			printGeneration (node, lst) ;
			break ;

		case TK_GET_VALUE :
			searchedVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
			getValueGeneration (lst, searchedVar, rspDepth) ;
			break ;

		case TK_SWITCH :
			;

			int i, caseCount, savedRspDepth, def_label ;
			int *caseLabels ;
			astNode *statementsNode = node->next->next->next->next ;

			savedRspDepth = rspDepth ;

			rspDepth = switchDeclareVars (node->next->next->next->next, vst, rspDepth) ;
			caseCount = getCaseCount (node->next->next->next->next) ;
			caseLabels = (int *) malloc (sizeof(int) * caseCount) ;
			def_label = switchCaseLabels (node , lst, caseCount, caseLabels) ;

			end_label = get_label () ;
			i = 0 ;
			while (statementsNode != NULL)
			{
				fprintf (fp, "\nLABEL%d:\n", caseLabels[i]) ;
				moduleGeneration (statementsNode, savedRspDepth, rspDepth, lst, vst) ;
				fprintf (fp ,"\n\t\tJMP LABEL%d\n", end_label) ;

				i++ ;
				if (statementsNode->next != NULL)
				{
					if (statementsNode->next->id == TK_DEFAULT)
					{
						statementsNode = statementsNode->next->next ;
						fprintf (fp, "\nLABEL%d:\n", def_label) ;
						moduleGeneration (statementsNode, savedRspDepth, rspDepth, lst, vst) ;

						break ;
					}
					statementsNode = statementsNode->next->next->next ;
				}
				else
					statementsNode = NULL ;
			}

			fprintf (fp ,"\nLABEL%d:\n", end_label) ;
			fprintf (fp, "\n\t\tADD RSP, %d\n", rspDepth - savedRspDepth) ;
			rspDepth = savedRspDepth ;

			break ;

		case TK_ID :
			calledModule = searchModuleInbaseST (realBase, node->tok->lexeme) ;
			varSTentry* varEntry = calledModule->inputVars[0] ;

			while (varEntry->thisVarST->varType == VAR_PLACEHOLDER)
				varEntry = varEntry->next ;

			idListHead = node->next->child ; 
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			while (idListHead != NULL)
			{
				searchedVar = searchVar (realBase, lst, idListHead->tok->lexeme) ;

				if (searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN)
				{
					fprintf (fp, "\t\tMOV AX, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
					fprintf (fp, "\t\tPUSH AX\n") ;
				}
				else if (isVarStaticArr (searchedVar))
				{
					fprintf (fp, "\t\tMOV RAX, RBP\n") ;
					fprintf (fp, "\t\tSUB RAX, %d\n", -searchedVar->offset) ;
					fprintf (fp, "\t\tPUSH RAX\n") ;
				}
				else
				{
					fprintf (fp, "\t\tMOV AX, [RBP%s]\n", getOffsetStr(searchedVar->offset - 10)) ;
					if (isLeftLimStatic(varEntry->thisVarST) && !isLeftLimStatic(searchedVar))
					{
						int lab = get_label () ;
						tf |= 1 << argLimERROR ;

						fprintf (fp, "\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokLeft->lexeme) ;
						fprintf (fp, "\t\tCMP AX, CX\n") ;
						fprintf (fp, "\t\tJE LABEL%d\n", lab) ;
						fprintf (fp, "\t\tCALL argLimERROR\n") ;
						fprintf (fp, "\n\tLABEL%d:\n", lab) ; 
					}

					if (!isVarStaticArr (varEntry->thisVarST))
						fprintf (fp, "\t\tPUSH AX\n") ;

					fprintf (fp, "\t\tMOV BX, [RBP%s]\n", getOffsetStr(searchedVar->offset - 8)) ;
					if (isRightLimStatic (varEntry->thisVarST) && !isRightLimStatic(searchedVar))
					{
						int lab = get_label () ;
						tf |= 1 << argLimERROR ;

						fprintf (fp, "\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokRight->lexeme) ;
						fprintf (fp, "\t\tCMP BX, CX\n") ;
						fprintf (fp, "\t\tJE LABEL%d\n", lab) ;
						fprintf (fp, "\t\tCALL argLimERROR\n") ;
						fprintf (fp, "\n\tLABEL%d:\n", lab) ; 
					}

					if (!isVarStaticArr (varEntry->thisVarST))
						fprintf (fp, "\t\tPUSH BX\n") ;

					fprintf (fp, "\t\tMOV RAX, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
					fprintf (fp, "\t\tPUSH RAX\n") ;
				}

				idListHead = idListHead->prev ;
				varEntry = varEntry->next ;
				while (varEntry != NULL && varEntry->thisVarST->varType == VAR_PLACEHOLDER)
					varEntry = varEntry->next ;
			}

			fprintf (fp, "\t\tCALL %s\n", node->tok->lexeme) ;
			fprintf (fp, "\t\tADD RSP, %d\n", calledModule->inputSize) ;
			break ;
	}

	return rspDepth ;
}

void codeGeneration(astNode *node)
{
	if (node == NULL)
		return;

	switch (node->id)
	{
		case program :
			codeGeneration (node->child->next) ;		// <otherModules>
			break ;

		case otherModules :
			codeGeneration (node->child) ;					// Do module definitions
			codeGeneration (node->next) ;					// <driverModule> or NULL
			break ;

		case module :
			fprintf (fp, "\n%s:\n", node->child->tok->lexeme) ;
			fprintf (fp, "\t\tPUSH RBP\n") ;
			fprintf (fp, "\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration (node->child->next->next->next, 0, 0, NULL, NULL) ;
			codeGeneration (node->next) ;					// Moving on to the next module

			break ;

		case driverModule :
			fprintf (fp, "\nmain:\n") ;
			fprintf (fp, "\t\tPUSH RBP\n") ;
			fprintf (fp, "\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration(node->child, 0, 0, NULL, NULL); 				// <statements>
			codeGeneration(node->next); 				// Move to the next child of program

			break ;
	}
}

void printCommentLineNASM ()
{
	fprintf (fp, "\n;--------------------------------------------------------------------------------------------------\n") ;
}