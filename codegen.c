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

int get_label()
{
	x++;
	return x;
}

void codeComment (int tabCount, char *comment)
{
	int i ;
	for (i = 0 ; i < tabCount ; i++)
		codePrint ("\t") ;

	codePrint ("; %s\n", comment) ;
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

void loadRegLeftLim (varST *vst, char *reg)
{
	if (isLeftLimStatic (vst))
		codePrint ("\t\tMOV %s, %s\n", reg, vst->arrayIndices->tokLeft->lexeme) ;
	else
		codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset-10)) ;
}

void loadRegRightLim (varST *vst, char *reg)
{
	if (isRightLimStatic (vst))
		codePrint ("\t\tMOV %s, %s\n", reg, vst->arrayIndices->tokRight->lexeme) ;		
	else
		codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset-8)) ;
}

void loadArrBase (varST *vst)
{
	if (isVarStaticArr (vst))
	{
		if (vst->varType == VAR_LOCAL)
		{
			codePrint ("\t\tMOV RDI, RBP\n") ;
			codePrint ("\t\tSUB RDI, %d\n", vst->offset) ;			
		}
		else
			codePrint ("\t\tMOV RDI, [RBP%s]\n", getOffsetStr (vst->offset)) ;
	}
	else
		codePrint ("\t\tMOV RDI, [RBP%s]\n", getOffsetStr(vst->offset)) ;
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
		codePrint ("\t\tMOV BX, %s\n", node->child->tok->lexeme) ;
	else
		codePrint ("\t\tMOV BX, [RBP%s]\n", getOffsetStr (indexVar->offset)) ;

	loadRegRightLim (vst, "CX") ;

	codePrint ("\t\tCALL boundCheck\n\n") ;
}

void assignGeneration (astNode *node, moduleST *lst)
{
	if(node->child == NULL)
	{
		varST *searchedVar = searchVar(realBase, lst, node->tok->lexeme) ;
		codePrint ("\t\tPOP AX\n");
		if (node->next->id == TK_MINUS && node->next->child->next == NULL)
			codePrint ("\t\tNEG AX\n") ;

		codePrint ("\t\tMOV [RBP%s], AX", getOffsetStr (searchedVar->offset)) ;
	}
	else
	{
		varST* vst ;
		vst = searchVar(realBase, lst,node->tok->lexeme);

		if (isVarStaticArr(vst) && node->child->id == TK_NUM)
		{
			codePrint ("\t\tPOP AX\n");	
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				codePrint ("\t\tNEG AX\n") ;

			codePrint ("\t\tMOV [RBP%s], AX" , getOffsetStr(getStaticOffset(vst,node,2))) ;
		}
		else
		{
			boundCheckGeneration (node, lst, vst) ;
			codePrint ("\t\tPOP AX\n") ;
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				codePrint ("\t\tNEG AX\n") ;

			codePrint ("\t\tMOV [RDI + RBX], AX\n") ;
		}
	}
	codeComment (9, "store variable") ;

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

				codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(searchedVar->offset)) ;
				codePrint ("\t\tPUSH %s\n", reg);	
			}
			else
			{
				varST *vst ;
				vst = searchVar (realBase, lst,node->tok->lexeme) ;

				if (isVarStaticArr(vst) && node->child->id == TK_NUM)
					codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(getStaticOffset (vst, node, 2))) ;						
				else
				{
					boundCheckGeneration (node, lst, vst) ;
					codePrint ("\t\tMOV %s, [RDI + RBX]\n", reg) ;
				}
				
				codePrint ("\t\tPUSH %s\n", reg);	
			}

			break ;

		case TK_RNUM :
			printf ("CODEGEN ERROR : NO FLOATING POINT ALLOWED!\n") ;
			exit (0) ;
			break ;

		case TK_NUM : 
			codePrint ("\t\tMOV %s, %s\n", reg, node->tok->lexeme);
			codePrint ("\t\tPUSH %s\n", reg);	
			break ;

		case TK_TRUE :
			codePrint ("\t\tMOV %s, 1\n", reg);
			codePrint ("\t\tPUSH %s\n", reg);	
			break ;

		case TK_FALSE :
			codePrint ("\t\tMOV %s, 0\n", reg);
			codePrint ("\t\tPUSH %s\n", reg);	
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
			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tADD AX,BX\n");
			codePrint ("\t\tPUSH AX\n");

			break ;

		case TK_MINUS :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tSUB AX,BX\n");
			codePrint ("\t\tPUSH AX\n");

			break ;

		case TK_MUL :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tIMUL BX\n");
			codePrint ("\t\tPUSH AX\n");

			break ;

		case TK_DIV :
			exprGeneration (node->child->next, lst) ;
			exprGeneration (node->child, lst) ;
			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tcwd\n") ;
			codePrint ("\t\tIDIV BX\n");
			codePrint ("\t\tPUSH AX\n");
			break ;

		case TK_LT : case TK_GT : case TK_LE : case TK_GE : case TK_NE : case TK_EQ :
			exprGeneration (node->child, lst) ;
			exprGeneration (node->child->next, lst) ;

			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tCMP AX,BX\n");

			switch (node->id)
			{
				case TK_LT :
					codePrint ("\t\tSETL AL\n");
					break ;
				case TK_GT :
					codePrint ("\t\tSETG AL\n");
					break ;
				case TK_LE :
					codePrint ("\t\tSETLE AL\n");
					break ;
				case TK_GE :
					codePrint ("\t\tSETGE AL\n");
					break ;
				case TK_NE :
					codePrint ("\t\tSETNE AL\n");
					break ;
				case TK_EQ :
					codePrint ("\t\tSETE AL\n");
					break ;
			}
			codePrint ("\t\tMOVSX AX, AL\n") ;
			codePrint ("\t\tPUSH AX\n") ;

			break ;

		case TK_AND : case TK_OR :
			exprGeneration (node->child, lst) ;
			exprGeneration (node->child->next, lst) ;

			codePrint ("\t\tPOP BX\n");
			codePrint ("\t\tPOP AX\n"); 

			if (node->id == TK_AND)
				codePrint ("\t\tAND AX, BX\n");
			else
				codePrint ("\t\tOR AX, BX\n");

			codePrint ("\t\tPUSH AX\n") ;

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
			codePrint ("\t\tMOV SI, %s\n", node->tok->lexeme) ;
			codePrint ("\t\tCALL printInteger\n\n") ;
		}
		else
		{
			tf |= 1 << printBoolean ;
			codePrint ("\t\tMOV SI, %d\n", node->tok->lexeme[0]=='t'?1:0) ;
			codePrint ("\t\tCALL printBoolean\n\n") ;
		}

		return ;
	}

	if (searchedVar->datatype == TK_INTEGER)
	{
		tf |= 1 << printInteger ;
		codePrint ("\t\tMOV SI, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
		codePrint ("\t\tCALL printInteger\n\n") ;
	}
	else if (searchedVar->datatype == TK_BOOLEAN)
	{
		tf |= 1 << printBoolean ;
		codePrint ("\t\tMOV SI, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
		codePrint ("\t\tCALL printBoolean\n\n") ;
	}
	else // Array type
	{	
		if (node->child != NULL)
		{
			// Special Case
			if (node->child->id == TK_NUM && isVarStaticArr (searchedVar))
				codePrint ("\t\tMOV SI, [RBP%s]\n", getOffsetStr(getStaticOffset(searchedVar,node,2))) ;
			else
			{
				// General case of static array with dynamic indices, or dynamic array
				boundCheckGeneration (node, lst, searchedVar) ;
				codePrint ("\t\tMOV SI, [RDI + RBX]\n") ;
			}

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printInteger ;
				codePrint ("\t\tCALL printInteger\n\n") ;
			}
			else
			{
				tf |= 1 << printBoolean ;
				codePrint ("\t\tCALL printBoolean\n\n") ;
			}
		}
		else
		{
			loadArrBase (searchedVar) ;
			loadRegLeftLim (searchedVar, "CX") ;
			loadRegRightLim (searchedVar, "DX") ;
			codePrint ("\t\tSUB DX, CX\n") ;
			codePrint ("\t\tADD DX, 1\n") ;
			codePrint ("\t\tADD DX, DX\n") ;

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printIntegerArr ;
				codePrint ("\t\tCALL printIntegerArr\n\n") ;
			}
			else
			{
				tf |= 1 << printBooleanArr ;
				codePrint ("\t\tCALL printBooleanArr\n\n") ;
			}
		}
	}
}

void getValueRSPAlign()
{
	codePrint ("\t\tMOV RAX, RSP\n") ;
	codePrint ("\t\tAND RAX, 15\n") ;
	codePrint ("\t\tADD RAX, 8\n") ;
	codePrint ("\t\tSUB RSP, RAX\n") ;
	codePrint ("\t\tPUSH RAX\n") ;
}

void getValueGeneration (moduleST *lst, varST *vst)
{		
	int rspAlign ;
	df |= 1 << inputInt ;

	if (vst->datatype == TK_INTEGER || vst->datatype == TK_BOOLEAN)
	{
		if (vst->datatype == TK_INTEGER)
		{
			df |= 1 << inputIntPrompt ;
			codePrint ("\n\t\tMOV RDI, inputIntPrompt") ;
		}
		else if (vst->datatype == TK_BOOLEAN)
		{
			df |= 1 << inputBoolPrompt ;
			codePrint ("\n\t\tMOV RDI, inputBoolPrompt") ;
		}
		codeComment (8, "get_value") ;


		tf |= 1 << getValuePrimitive ;
		codePrint ("\t\tMOV RBX, -%d\n", vst->offset) ;
		codePrint ("\t\tCALL getValuePrimitive\n\n") ;		
 	}
	else // Array type
	{	
		if (vst->arrayIndices->type == TK_INTEGER)
		{
			df |= 1 << inputIntArrPrompt ;
			codePrint ("\t\tMOV RDI, inputIntArrPrompt\n") ;
		}
		else
		{
			df |= 1 << inputBoolArrPrompt ;
			codePrint ("\t\tMOV RDI, inputBoolArrPrompt\n") ;
		}

		tf |= 1 << printGetArrPrompt ;
		tf |= 1 << getArr ;

		loadRegLeftLim (vst, "BX") ;
		loadRegRightLim (vst, "CX") ;
		codePrint ("\t\tCALL printGetArrPrompt\n\n") ;

		loadArrBase (vst) ;
		codePrint ("\t\tMOV DX, CX\n") ;
		codePrint ("\t\tSUB DX, BX\n") ;
		codePrint ("\t\tADD DX, 1\n") ;
		codePrint ("\t\tADD DX, DX\n") ;
		codePrint ("\t\tMOVSX RDX, DX\n") ;
		codePrint ("\t\tCALL getArr\n\n") ;
	}
}

void dynamicDeclareCheck (moduleST *lst, varST *vst)
{
	int start_label , end_label ;
	varST *leftVar , *rightVar ;
	tf |= 1 << dynamicDeclCheck ;

	codePrint ("\n") ;
	if (isLeftLimStatic (vst))
		codePrint ("\t\tMOV AX, %s\n", vst->arrayIndices->tokLeft->lexeme) ;
	else
	{
		leftVar = searchVar (realBase, lst, vst->arrayIndices->tokLeft->lexeme) ;
		//codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr(leftVar->offset)) ;
		codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr (leftVar->offset)) ;
	}

	if (isRightLimStatic (vst))
		codePrint ("\t\tMOV BX, %s\n", vst->arrayIndices->tokRight->lexeme) ;
	else
	{
		rightVar = searchVar (realBase, lst, vst->arrayIndices->tokRight->lexeme) ;
		codePrint ("\t\tMOV BX, [RBP%s]\n", getOffsetStr(rightVar->offset)) ;
	}

	codePrint ("\t\tCALL dynamicDeclCheck\n\n") ;
}

void dynamicDeclareGeneration (moduleST *lst, varST *vst, int declCount)
{
	int start_label ;
	dynamicDeclareCheck (lst, vst) ;
	codePrint ("\t\tPUSH BX") ;
	codeComment (12, "saving register for malloc") ;
	codePrint ("\t\tPUSH AX") ;
	codeComment (12, "saving register for malloc") ;

	if (declCount > 1)
	{
		codePrint ("\n\t\tMOV CX, 0\n") ;
		codePrint ("\t\tMOV RBX, %d\n", vst->offset) ;
		codePrint ("\t\tNEG RBX\n") ;
		codePrint ("\n\t.declLoop:\n") ;

		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tPUSH RBX\n") ;
		codePrint ("\t\tPUSH RDI\n") ;
		codePrint ("\t\tCALL malloc\n") ;
		codePrint ("\t\tPOP RDI\n") ;
		codePrint ("\t\tPOP RBX\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tMOV [RBP+RBX], RAX\n") ;

		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tMOV [RBP + RBX + 10], AX\n") ;
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tMOV [RBP + RBX + 8], DX\n") ;

		codePrint ("\t\tPUSH DX\n") ;
		codePrint ("\t\tPUSH AX\n") ;
		codePrint ("\t\tADD RBX, 12\n") ;
		codePrint ("\t\tINC CX\n") ;
		codePrint ("\t\tCMP CX, %d\n", declCount) ;
		codePrint ("\t\tJL .declLoop\n") ;

		codePrint ("\t\tADD RSP, 4") ;
		codeComment (8, "Restoring array limits") ;
	}
	else
	{
		codePrint ("\t\tCALL malloc\n") ;
		codePrint ("\t\tMOV [RBP%s], RAX\n", getOffsetStr(vst->offset)) ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tMOV [RBP%s], AX\n", getOffsetStr(vst->offset - 10)) ;
		codePrint ("\t\tPOP BX\n") ;
		codePrint ("\t\tMOV [RBP%s], BX\n\n", getOffsetStr(vst->offset - 8)) ;
	}
}

void preamble()
{
	codePrint ("extern printf\n") ;
	codePrint ("extern scanf\n") ;
	codePrint ("extern malloc\n") ;
	codePrint ("extern exit\n") ;

	codePrint ("\nglobal main\n") ;
	codePrint ("section .text\n") ;
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

		codePrint ("\nboundCheck:\n") ;
		codePrint ("\t\tCMP BX, AX\n") ;
		codePrint ("\t\tJGE .leftLim\n") ;
		codePrint ("\t\tCALL boundERROR\n") ;

		codePrint ("\n\t.leftLim:\n") ;
		codePrint ("\t\tCMP CX, BX\n") ;
		codePrint ("\t\tJGE .rightLim\n") ;
		codePrint ("\t\tCALL boundERROR\n") ;

		codePrint ("\n\t.rightLim:\n") ;
		codePrint ("\t\tSUB BX, AX\n") ;
		codePrint ("\t\tADD BX, BX\n") ;
		codePrint ("\t\tMOVSX RBX, BX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, dynamicDeclCheck))
	{
		tf |= 1 << declNegERROR ;
		tf |= 1 << declERROR ;

		codePrint ("\ndynamicDeclCheck:\n") ;
		codePrint ("\t\tCMP AX, 0\n") ;
		codePrint ("\t\tJGE .leftNotNeg\n") ;
		codePrint ("\t\tCALL declNegERROR\n") ;

		codePrint ("\n\t.leftNotNeg:\n") ;
		codePrint ("\t\tCMP BX, 0\n") ;
		codePrint ("\t\tJGE .rightNotNeg\n") ;
		codePrint ("\t\tCALL declNegERROR\n") ;

		codePrint ("\n\t.rightNotNeg:\n") ;
		codePrint ("\t\tCMP BX, AX\n") ;
		codePrint ("\t\tJGE .dynChecked\n") ;
		codePrint ("\t\tCALL declERROR\n") ;
		codePrint ("\n\t.dynChecked:\n") ;
		codePrint ("\t\tMOV DX, BX\n") ;
		codePrint ("\t\tSUB DX, AX\n") ;
		codePrint ("\t\tADD DX, 1\n") ;
		codePrint ("\t\tADD DX, DX\n") ;
		codePrint ("\t\tMOVSX RDI, DX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getValuePrimitive))
	{
		codePrint ("\ngetValuePrimitive:\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH RBX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP RBX\n\n") ;

		getValueRSPAlign (fp) ;

		codePrint ("\t\tMOV RDI, inputInt") ;
		codeComment (8, "get_value") ;
		codePrint ("\t\tMOV RSI, RSP\n") ;
		codePrint ("\t\tSUB RSI, 16\n") ;
		codePrint ("\t\tPUSH RBX\n") ;
		codePrint ("\t\tPUSH RSI\n") ;
		codePrint ("\t\tCALL scanf\n") ;
		codePrint ("\t\tPOP RSI\n") ;
		codePrint ("\t\tPOP RBX\n") ;
		codePrint ("\t\tMOV AX, [RSP - 16]\n") ;
		codePrint ("\t\tMOV [RBP + RBX], AX\n") ;

		codePrint ("\n\t\tPOP RAX\n") ;
		codePrint ("\t\tADD RSP, RAX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printGetArrPrompt))
	{
		df |= 1 << leftRange ;
		df |= 1 << rightRange ;

		codePrint ("\nprintGetArrPrompt:\n") ;
		codePrint ("\t\tMOV SI, CX\n") ;
		codePrint ("\t\tSUB SI, BX\n") ;
		codePrint ("\t\tADD SI, 1\n") ; 
		codePrint ("\t\tMOVSX RSI, SI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH BX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP BX\n") ;

		codePrint ("\n\t\tMOV RDI, leftRange\n") ;
		codePrint ("\t\tMOVSX RSI, BX\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH BX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP BX\n") ;

		codePrint ("\n\t\tMOV RDI, rightRange\n") ;
		codePrint ("\t\tMOVSX RSI, CX\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH BX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP BX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getArr))
	{
		codePrint ("\ngetArr:\n");
		getValueRSPAlign (fp) ;

		codePrint ("\n\t\tPUSH RDI\n") ;
		codePrint ("\t\tMOV RCX, 0\n") ;
		codePrint ("\n\t.getArr:") ;
		codeComment (8, "getting array") ;

		codePrint ("\t\tMOV RDI, inputInt\n") ;
		codePrint ("\t\tMOV RSI, RSP\n") ;
		codePrint ("\t\tSUB RSI, 24\n") ;
		codePrint ("\t\tPUSH RCX\n") ;
		codePrint ("\t\tPUSH RDX\n") ;
		codePrint ("\t\tPUSH RSI\n") ;
		codePrint ("\t\tCALL scanf\n") ;
		codePrint ("\t\tPOP RSI\n") ;
		codePrint ("\t\tPOP RDX\n") ;
		codePrint ("\t\tPOP RCX\n") ;

		codePrint ("\t\tMOV RBX, RCX\n") ;
		codePrint ("\t\tMOV AX, [RSP - 24]\n") ;
		codePrint ("\t\tPOP RDI\n") ;
		codePrint ("\t\tPUSH RDI\n") ;
		codePrint ("\t\tMOV [RDI + RBX], AX\n") ;

		codePrint ("\n\t\tADD RCX, 2\n") ;	// Incrementing counter
		codePrint ("\t\tCMP RCX, RDX\n") ;
		codePrint ("\t\tJNE .getArr\n\n") ;

		codePrint ("\t\tPOP RDI\n") ;
		codePrint ("\t\tPOP RAX\n") ;
		codePrint ("\t\tADD RSP, RAX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printInteger))
	{
		df |= 1 << printFormat ;

		codePrint ("\nprintInteger:\n") ;
		codePrint ("\t\tMOV RDI, printFormat\n") ;
		codePrint ("\t\tMOVSX RSI, SI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf , printBoolean))
	{
		df |= 1 << printFalse ;
		df |= 1 << printTrue ;

		codePrint ("\nprintBoolean:\n") ;
		codePrint ("\t\tCMP SI, 0\n") ;
		codePrint ("\t\tJE .boolFalse\n") ;
		codePrint ("\t\tMOV RDI, printTrue\n") ;
		codePrint ("\t\tJMP .boolPrint\n") ;

		codePrint ("\n\t.boolFalse:\n") ;
		codePrint ("\t\tMOV RDI, printFalse\n") ;
		codePrint ("\n\t.boolPrint:\n") ;

		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printIntegerArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << printNewLine ;
		df |= 1 << printInt ;


		codePrint ("\nprintIntegerArr:\n") ;
		codePrint ("\t\tPUSH RDI\n") ;
		codePrint ("\t\tMOV RDI, printFormatArray\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH DX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;

		codePrint ("\t\tMOV CX, 0\n") ;
		codePrint ("\n\t.printArr:") ;
		codeComment (8, "printing array") ;

		codePrint ("\t\tPUSH RDI\n") ;
		codePrint ("\t\tMOVSX RBX, CX\n") ;
		codePrint ("\t\tMOV SI, [RDI + RBX]\n") ;
		codePrint ("\t\tMOV RDI, printInt\n") ;
		codePrint ("\t\tMOVSX RSI, SI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tPUSH DX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;

		codePrint ("\t\tADD CX, 2\n") ;
		codePrint ("\t\tCMP CX, DX\n") ;
		codePrint ("\t\tJNE .printArr\n\n") ;

		codePrint ("\t\tMOV RDI, printNewLine\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printBooleanArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << DATA_true ;
		df |= 1 << DATA_false ;
		df |= 1 << printNewLine ;

		codePrint ("\nprintBooleanArr:\n") ;
		codePrint ("\n\t\tPUSH RDI\n") ;
		codePrint ("\t\tMOV RDI, printFormatArray\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH DX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;

		codePrint ("\t\tMOV CX, 0\n") ;
		codePrint ("\n\t.printArr:") ;
		codeComment (8, "printing array") ;

		codePrint ("\t\tMOVSX RBX, CX\n") ;
		codePrint ("\t\tMOV AX, [RDI + RBX]\n") ;
		codePrint ("\t\tPUSH RDI\n") ;

		codePrint ("\n\t\tCMP AX, 0\n") ;
		codePrint ("\t\tJE .arrFalse\n") ;
		codePrint ("\t\tMOV RDI, true\n") ;
		codePrint ("\t\tJMP .afterBool\n") ;
		
		codePrint ("\n\t.arrFalse:\n") ;
		codePrint ("\t\tMOV RDI, false\n") ;
		codePrint ("\n\t.afterBool:\n") ;

		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tPUSH DX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;

		codePrint ("\t\tADD CX, 2\n") ;
		codePrint ("\t\tCMP CX, DX\n") ;
		codePrint ("\t\tJNE .printArr\n\n") ;

		codePrint ("\t\tMOV RDI, printNewLine\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, boundERROR))
	{
		df |= 1 << boundPrint ;

		codePrint ("\nboundERROR:\n") ;
		codePrint ("\t\tMOV RDI, boundPrint\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tMOV EDI, 0\n") ;
		codePrint ("\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, declERROR))
	{
		df |= 1 << declPrint ;

		codePrint ("\ndeclERROR:\n") ;
		codePrint ("\t\tMOV RDI, declPrint\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tMOV EDI, 0\n") ;
		codePrint ("\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, declNegERROR))
	{
		df |= 1 << declNeg ;

		codePrint ("\ndeclNegERROR:\n") ;
		codePrint ("\t\tMOV RDI, declNeg\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tMOV EDI, 0\n") ;
		codePrint ("\t\tCALL exit\n") ;
	}

	if (isFlagSet (tf, argLimERROR))
	{
		df |= 1 << arrArgMismatch ;

		codePrint ("\nargLimERROR:\n") ;
		codePrint ("\t\tMOV RDI, arrArgMismatch\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tCALL printf\n") ;
		codePrint ("\t\tMOV EDI, 0\n") ;
		codePrint ("\t\tCALL exit\n") ;
	}

	/* --------------------------------------------------------------------------------------------- */
	printCommentLineNASM (fp) ;

	if (df)
		codePrint ("\nsection .data\n") ;

	if (isFlagSet (df, boundPrint))
	{
		codePrint ("\t\tboundPrint : ") ;
		codePrint ("db \"RUNTIME ERROR : Array out of bounds\" , 10, 0\n") ;	
	}
	
	if (isFlagSet (df, declPrint))
	{
		codePrint ("\t\tdeclPrint : ") ;
		codePrint ("db \"RUNTIME ERROR : Invalid order of bounds in dynamic array declaration\" , 10, 0\n") ;		
	}
	
	if (isFlagSet (df, declNeg))
	{
		codePrint ("\t\tdeclNeg : ") ;
		codePrint ("db \"RUNTIME ERROR : Negative bound in dynamic array declaration\" , 10, 0\n") ;	
	}

	if (isFlagSet (df, arrArgMismatch))
	{
		codePrint ("\t\tarrArgMismatch: ") ;
		codePrint ("db \"RUNTIME ERROR : Mismatch in formal and actual array limits\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printFormatArray))
	{
		codePrint ("\t\tprintFormatArray : ") ;
		codePrint ("db \"Output : \" , 0\n") ;
	}

	if (isFlagSet (df, printInt))
	{
		codePrint ("\t\tprintInt : ") ;
		codePrint ("db \"%%d \", 0\n") ;
	}

	if (isFlagSet (df, printNewLine))
	{
		codePrint ("\t\tprintNewLine : ") ;
		codePrint ("db 10, 0\n") ;
	}

	if (isFlagSet (df, printFormat))
	{
		codePrint ("\t\tprintFormat : ") ;
		codePrint ("db \"Output :  %%d\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printTrue))
	{
		codePrint ("\t\tprintTrue : ") ;
		codePrint ("db \"Output : true\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printFalse))
	{
		codePrint ("\t\tprintFalse : ") ;
		codePrint ("db \"Output : false\" , 10, 0\n") ;
	}

	if (isFlagSet (df, DATA_true))
	{
		codePrint ("\t\ttrue : ") ;
		codePrint ("db \"true \" , 0\n") ;
	}

	if (isFlagSet (df, DATA_false))
	{
		codePrint ("\t\tfalse : ") ;
		codePrint ("db \"false \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntPrompt))
	{
		codePrint ("\t\tinputIntPrompt : ") ;
		codePrint ("db \"Enter an integer : \" , 0\n") ;
	}

	if (isFlagSet (df, inputBoolPrompt))
	{
		codePrint ("\t\tinputBoolPrompt : ") ;
		codePrint ("db \"Enter a boolean (0 for false, non-zero for true) : \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntArrPrompt))
	{
		codePrint ("\t\tinputIntArrPrompt : ") ;
		codePrint ("db \"Enter %%d array elements of integer type for range \", 0\n") ;
	}

	if (isFlagSet (df, inputBoolArrPrompt))
	{
		codePrint ("\t\tinputBoolArrPrompt : ") ;
		codePrint ("db \"Enter %%d array elements of boolean type for range \", 0\n") ;
	}

	if (isFlagSet (df, leftRange))
	{
		codePrint ("\t\tleftRange : ") ;
		codePrint ("db \"%%d to \" , 0\n") ;
	}

	if (isFlagSet (df, rightRange))
	{
		codePrint ("\t\trightRange : ") ;
		codePrint ("db \"%%d\" ,10, 0\n") ;
	}

	if (isFlagSet (df, inputInt))
	{
		codePrint ("\t\tinputInt : ") ;
		codePrint ("db \"%%d\", 0\n") ;
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


void switchDeclareVars (astNode *statementsNode)
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
				moduleGeneration (statementNode->child, statementsNode->localST) ;
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
						moduleGeneration (statementNode->child, statementsNode->localST) ;
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
}

int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels)
{
	int i, def_label = -1 ;

	for (i = 0 ; i < caseCount ; i++)
		caseLabels[i] = get_label() ;

	varST *switchVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
	codePrint ("\n\t\tMOV AX, [RBP%s]", getOffsetStr(switchVar->offset)) ;
	codeComment (9, "loading switch variable") ;

	astNode *caseValNode =  node->next->next->next ;
	i = 0 ;

	if (switchVar->datatype == TK_INTEGER)
	{
		while (caseValNode != NULL)
		{
			codePrint ("\n\t\tCMP AX, %s", caseValNode->tok->lexeme) ;
			codeComment (11, "switch case") ;
			codePrint ("\t\tJE LABEL%d\n", caseLabels[i]) ;

			if (caseValNode->next->next->next->id == statements)
			{
				def_label = get_label () ;
				codePrint ("\n\t\tJMP LABEL%d", def_label) ;
				codeComment (11, "default case") ;
				break ;
			}

			i++ ;
			caseValNode = caseValNode->next->next->next ;
		}
	}
	else
	{		
		codePrint ("\n\t\tCMP AX, 0\n") ;
		if (caseValNode->tok->lexeme[0] == 't')
		{
			codePrint ("\t\tJNE LABEL%d", caseLabels[0]) ;
			codeComment (11, "true case") ;
		}
		else
		{
			codePrint ("\t\tJE LABEL%d", caseLabels[0]) ;
			codeComment (11, "false case") ;
		}

		codePrint ("\t\tJMP LABEL%d", caseLabels[1]) ;
		if (caseValNode->tok->lexeme[0] == 't')
			codeComment (11, "false case") ;
		else
			codeComment (11, "true case") ;
	}

	return def_label ;
}

void pushInputGeneration (astNode *inputEnd, varSTentry *varEntry, moduleST *lst)
{
	varST *searchedVar ;

	while (varEntry->thisVarST->varType == VAR_PLACEHOLDER)
		varEntry = varEntry->next ;

	while (inputEnd != NULL)
	{
		searchedVar = searchVar (realBase, lst, inputEnd->tok->lexeme) ;

		if (searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN)
		{
			codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
			codePrint ("\t\tPUSH AX\n") ;
		}
		else if (isVarStaticArr (searchedVar))
		{
			codePrint ("\t\tMOV RAX, RBP\n") ;
			codePrint ("\t\tSUB RAX, %d\n", -searchedVar->offset) ;
			codePrint ("\t\tPUSH RAX\n") ;
		}
		else
		{
			codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr(searchedVar->offset - 10)) ;
			if (isLeftLimStatic(varEntry->thisVarST) && !isLeftLimStatic(searchedVar))
			{
				int lab = get_label () ;
				tf |= 1 << argLimERROR ;

				codePrint ("\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokLeft->lexeme) ;
				codePrint ("\t\tCMP AX, CX\n") ;
				codePrint ("\t\tJE LABEL%d\n", lab) ;
				codePrint ("\t\tCALL argLimERROR\n") ;
				codePrint ("\n\tLABEL%d:\n", lab) ; 
			}

			if (!isVarStaticArr (varEntry->thisVarST))
				codePrint ("\t\tPUSH AX\n") ;

			codePrint ("\t\tMOV BX, [RBP%s]\n", getOffsetStr(searchedVar->offset - 8)) ;
			if (isRightLimStatic (varEntry->thisVarST) && !isRightLimStatic(searchedVar))
			{
				int lab = get_label () ;
				tf |= 1 << argLimERROR ;

				codePrint ("\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokRight->lexeme) ;
				codePrint ("\t\tCMP BX, CX\n") ;
				codePrint ("\t\tJE LABEL%d\n", lab) ;
				codePrint ("\t\tCALL argLimERROR\n") ;
				codePrint ("\n\tLABEL%d:\n", lab) ; 
			}

			if (!isVarStaticArr (varEntry->thisVarST))
				codePrint ("\t\tPUSH BX\n") ;

			codePrint ("\t\tMOV RAX, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
			codePrint ("\t\tPUSH RAX\n") ;
		}

		inputEnd = inputEnd->prev ;
		varEntry = varEntry->next ;
		while (varEntry != NULL && varEntry->thisVarST->varType == VAR_PLACEHOLDER)
			varEntry = varEntry->next ;
	}
}

void popOutputGeneration (astNode *outputHead, moduleST *lst)
{
	varST* searchedVar ;

	while (outputHead != NULL)
	{
		searchedVar = searchVar (realBase, lst, outputHead->tok->lexeme) ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tMOV [RBP%s], AX\n", getOffsetStr(searchedVar->offset)) ;

		outputHead = outputHead->next ;
	}

	codePrint ("\n") ;
}

int moduleGeneration (astNode *node, moduleST *lst)
{
	if (node == NULL)
		return 0 ;

	varST *searchedVar ;
	astNode *idListHead ;
	moduleST *calledModule ;
	varSTentry *varEntry ;

	switch (node->id)
	{
		int start_label, end_label ;
		int reserveLabel[10] ;

		case statements :
			moduleGeneration (node->child, node->localST);		// Access local scope and move below
			break ;

		case statement :
			moduleGeneration(node->child, lst);
			if (node->next == NULL)
			{
				if (lst->tableType == SWITCH_ST)
					break ;

				if (lst->parent == realBase)
				{
					codePrint ("\n\t\tMOV RSP, RBP\n") ;
					codePrint ("\t\tPOP RBP\n") ;
					codePrint ("\t\tret\n") ;
				}
				else
				{
					codePrint ("\t\tADD RSP, %d", lst->scopeSize) ;
					codeComment (10, "restoring to parent scope") ;
				}

			}
			else
				moduleGeneration(node->next, lst);

			break ;

		case TK_DECLARE :
			;
			switchDeclareStatus stat ;
			if (lst->tableType != SWITCH_ST)
				stat = NOT_SWITCH ; // Subtract RSP, and allocate memory if dynamic
			else if (node->parent != node)
				stat = SWITCH_INIT ; // Subtract RSP only, and do not allocate memory for dynamic
			else
			{
				stat = SWITCH_GEN ; // Do not subtract RSP, allocate memory if dynamic AND restore parent
				node->parent = node->next->parent ;
			}

			int declCount = 1 ;
			idListHead = node->next->child ;
			astNode *dtNode = node->next->next ;

			while (idListHead->next != NULL)
			{
				declCount++ ;
				idListHead = idListHead->next ;
			}

			searchedVar = searchVar (realBase, lst, idListHead->tok->lexeme) ;
			if (searchedVar->size > 0 && stat != SWITCH_GEN)
			{	
				codePrint ("\t\tSUB RSP, %d", searchedVar->size * declCount) ;
				if (stat == NOT_SWITCH)
					codeComment (11, "making space for declaration") ;
				else if (stat == SWITCH_INIT)
					codeComment (11, "declaring before switch") ;
			}

			if (dtNode->dtTag == ARRAY && !isVarStaticArr(searchedVar) && stat != SWITCH_GEN)
				dynamicDeclareGeneration (lst, searchedVar, declCount) ;

			
			break ;											

		case TK_ASSIGNOP :
			if ((node->child->next->id == TK_MINUS || node->child->next->id == TK_PLUS) && node->child->next->child->next == NULL)
				exprGeneration(node->child->next->child, lst);
			else
				exprGeneration(node->child->next, lst);

			assignGeneration (node->child, lst) ;

			codePrint ("\n") ;
			break ;

		case TK_FOR :
			node=node->next;
			start_label = get_label () ;
			end_label = get_label () ;

			searchedVar = searchVar(realBase, lst, node->tok->lexeme) ;

			codePrint ("\t\tMOV CX,%s\n", node->next->tok->lexeme);
			codePrint ("\t\tMOV [RBP%s], CX" , getOffsetStr(searchedVar->offset)) ;

			codePrint ("\n\tFOR%d:\n", start_label) ;
			codePrint ("\t\tMOV AX, %s\n", node->next->next->tok->lexeme) ;
			codePrint ("\t\tCMP CX,AX\n");
			codePrint ("\t\tJG FOR%d\n\n", end_label);

			moduleGeneration(node->next->next->next, lst);		// Statements

			codePrint ("\n\t\tMOV CX, [RBP%s]", getOffsetStr(searchedVar->offset)) ;
			codeComment (8, "For loop increment") ;
			codePrint ("\t\tINC CX\n");
			codePrint ("\t\tMOV [RBP%s],CX\n", getOffsetStr(searchedVar->offset)) ;
			codePrint ("\t\tJMP FOR%d\n", start_label) ;
			codePrint ("\n\tFOR%d:\n", end_label) ;
			break ;

		
		case TK_WHILE :
			node=node->next;
			start_label = get_label();
			end_label =  get_label();

			codePrint ("\n\tWHILE%d:\n", start_label) ;

			exprGeneration (node, lst);	// expression

			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tCMP AX, 0\n");
			codeComment (8, "Checking while loop condition") ;
			codePrint ("\t\tJE WHILE%d\n\n", end_label) ;

			moduleGeneration(node->next, lst);		// statements

			codePrint ("\t\tJMP WHILE%d\n", start_label) ;
			codePrint ("\n\tWHILE%d:\n", end_label) ;

			break ;
		
		case TK_PRINT :
			node = node->next ;
			printGeneration (node, lst) ;
			break ;

		case TK_GET_VALUE :
			searchedVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
			getValueGeneration (lst, searchedVar) ;
			break ;

		case TK_SWITCH :
			;

			int i, caseCount, def_label ;
			int *caseLabels ;
			astNode *statementsNode = node->next->next->next->next ;

			switchDeclareVars (node->next->next->next->next) ;
			caseCount = getCaseCount (node->next->next->next->next) ;
			caseLabels = (int *) malloc (sizeof(int) * caseCount) ;
			def_label = switchCaseLabels (node , lst, caseCount, caseLabels) ;

			end_label = get_label () ;
			i = 0 ;
			while (statementsNode != NULL)
			{
				codePrint ("\nLABEL%d:\n", caseLabels[i]) ;
				moduleGeneration (statementsNode, lst) ;
				codePrint ("\n\t\tJMP LABEL%d\n", end_label) ;

				i++ ;
				if (statementsNode->next != NULL)
				{
					if (statementsNode->next->id == TK_DEFAULT)
					{
						statementsNode = statementsNode->next->next ;
						codePrint ("\nLABEL%d:\n", def_label) ;
						moduleGeneration (statementsNode, lst) ;

						break ;
					}
					statementsNode = statementsNode->next->next->next ;
				}
				else
					statementsNode = NULL ;
			}

			statementsNode = node->next->next->next->next ;
			codePrint ("\nLABEL%d:\n", end_label) ;
			codePrint ("\n\t\tADD RSP, %d", statementsNode->localST->scopeSize) ;
			codeComment (11, "restoring to parent scope") ;

			break ;

		case TK_ID :
			calledModule = searchModuleInbaseST (realBase, node->tok->lexeme) ;
			varEntry = calledModule->inputVars[0] ;

			idListHead = node->next->child ; 
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			pushInputGeneration (idListHead, varEntry, lst) ;

			codePrint ("\t\tCALL %s\n", node->tok->lexeme) ;
			codePrint ("\t\tADD RSP, %d\n", calledModule->inputSize) ;
			break ;

		case idList :
			calledModule = searchModuleInbaseST (realBase, node->next->next->tok->lexeme) ;
			varEntry = calledModule->inputVars[0] ;

			idListHead = node->next->next->next->child ;
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			codePrint ("\t\tSUB RSP, %d\n", calledModule->outputSize) ;
			pushInputGeneration (idListHead, varEntry, lst) ;

			codePrint ("\t\tCALL %s\n", node->next->next->tok->lexeme) ;
			codePrint ("\t\tADD RSP, %d\n", calledModule->inputSize) ;

			popOutputGeneration (node->child, lst) ;
			break ;
	}

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
			codePrint ("\n%s:\n", node->child->tok->lexeme) ;
			codePrint ("\t\tPUSH RBP\n") ;
			codePrint ("\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration (node->child->next->next->next, NULL) ;
			codeGeneration (node->next) ;					// Moving on to the next module

			break ;

		case driverModule :
			codePrint ("\nmain:\n") ;
			codePrint ("\t\tPUSH RBP\n") ;
			codePrint ("\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration(node->child, NULL); 				// <statements>
			codeGeneration(node->next); 				// Move to the next child of program

			break ;
	}
}

void printCommentLineNASM ()
{
	codePrint ("\n;--------------------------------------------------------------------------------------------------\n") ;
}