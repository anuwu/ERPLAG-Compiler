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

extern int moduleGeneration (astNode *node, int localBase, int rspDepth, moduleST *lst, varST *vst, FILE *fp) ;

int get_label()
{
	x++;
	return x;
}

int getStaticOffset (varST *vst, astNode *node, int size)
{
	return vst->offset - size*(atoi(vst->arrayIndices->tokRight->lexeme) - atoi(node->child->tok->lexeme)) ;
}

void staticArrBoundCheck (astNode *node, moduleST *lst, varST *vst, FILE *fp)
{
	int leftLim, rightLim, start_label, end_label ;
	varST *indexVar = searchVar (realBase, lst, node->child->tok->lexeme) ;

	leftLim = atoi (vst->arrayIndices->tokLeft->lexeme) ;
	rightLim = atoi (vst->arrayIndices->tokRight->lexeme) ;
	tf |= 1 << staticBoundCheck ;

	fprintf (fp, "\t\tMOV AX, %d\n", leftLim) ;
	fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", indexVar->offset) ;
	fprintf (fp, "\t\tMOV CX, %d\n", rightLim) ;
	fprintf (fp, "\t\tMOV DX, %d\n", vst->offset) ;
	fprintf (fp, "\t\tCALL staticBoundCheck\n\n") ;
}


void dynamicArrBoundCheck (astNode *node, moduleST *lst, varST *vst, FILE *fp)
{
	int start_label, end_label ;

	fprintf (fp, "\t\tMOV RDI, [RBP - %d]\n", vst->offset) ;
	if (isdigit(vst->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\t\tMOV AX, %s\n", vst->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", vst->offset-10) ;

	if (isdigit(node->child->tok->lexeme[0]))
		fprintf (fp, "\n\t\tMOV BX, %s\n", node->child->tok->lexeme) ;
	else
	{
		varST *indexVar ;
		indexVar = searchVar (realBase, lst, node->child->tok->lexeme) ;
		fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", indexVar->offset) ;
	}

	if (isdigit(vst->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\t\tMOV CX, %s\n", vst->arrayIndices->tokRight->lexeme) ;		
	else
		fprintf (fp, "\t\tMOV CX, [RBP - %d]\n", vst->offset-8) ;

	tf |= 1 << dynamicBoundCheck ;
	fprintf (fp, "\t\tCALL dynamicBoundCheck\n\n") ;
}

void dynamicArrDX (varST *vst, FILE *fp)
{
	if (isdigit(vst->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\t\tMOV AX, %s\n", vst->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", vst->offset-10) ;

	if (isdigit(vst->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\t\tMOV BX, %s", vst->arrayIndices->tokRight->lexeme) ;
	else
		fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", vst->offset-8) ;

	fprintf (fp, "\t\tMOV DX, BX\n") ;
	fprintf (fp, "\t\tSUB DX, AX\n") ;
	fprintf (fp, "\t\tADD DX, 1\n") ;
	fprintf (fp, "\t\tADD DX, DX\n") ;

}

void IDGeneration (astNode *node, moduleST *lst, FILE* fp)
{
	if (node->parent->id!=TK_ASSIGNOP)
	{
		if (node->prev == NULL)
		{
			if(node->id == TK_ID && node->child==NULL)
			{
				fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
				fprintf (fp, "\t\tPUSH AX\n");	
			}

			else if(node->id == TK_ID && node->child!=NULL) // var:=A[i]+j;
			{
				varST *vst ;
				vst = searchVar (realBase, lst,node->tok->lexeme);

				if (isVarStaticArr(vst))
				{
					if (node->child->id == TK_ID)
					{
						staticArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\t\tMOV AX, [RBP + RBX]\n") ;
					}
					else
						fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", getStaticOffset (vst, node, 2)) ;
				}
				else
				{
					dynamicArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp, "\t\tMOV AX, [RDI + RBX]\n") ;
				}
				
				fprintf (fp, "\t\tPUSH AX\n");	
			}
		}
		else
		{
			if(node->id == TK_ID && node->child==NULL)
			{
				fprintf (fp, "\t\tMOV BX, [RBP - %d]\n",searchVar(realBase, lst, node->tok->lexeme)->offset);
				fprintf (fp, "\t\tPUSH BX\n");	
			}

			else if(node->id == TK_ID && node->child!=NULL) // var:=j+A[i];
			{
				varST *vst ;
				vst=searchVar (realBase, lst,node->tok->lexeme);

				if (isVarStaticArr(vst))
				{
					if (node->child->id == TK_ID)
					{
						staticArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\t\tMOV BX, [RBP + RBX]\n") ;
					}
					else
						fprintf (fp, "\t\tMOV BX, [RBP - %d]\n",getStaticOffset(vst,node,2)) ;
				}
				else
				{
					dynamicArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp, "\t\tMOV BX, [RDI + RBX]\n") ;
				}
					
				fprintf (fp, "\t\tPUSH BX\n");	
			}
		}
	}
	else
	{
		if(node->id == TK_ID && node->child==NULL && node->prev!=NULL)
		{
			fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			fprintf (fp, "\t\tPUSH AX\n");	
		}
		else if(node->id == TK_ID && node->child!=NULL && node->prev!=NULL) //type checking required i:=A[j]
		{
			varST *vst ;
			vst = searchVar (realBase, lst,node->tok->lexeme);

			if (isVarStaticArr(vst))
			{
				if (node->child->id == TK_ID)
				{
					staticArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp, "\t\tMOV AX, [RBP + RBX]\n") ;
				}
				else
					fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", getStaticOffset(vst, node, 2)) ;
			}
			else
			{
				dynamicArrBoundCheck (node, lst, vst, fp) ;
				fprintf (fp, "\t\tMOV AX, [RDI + RBX]\n") ;
			}
			
			fprintf (fp, "\t\tPUSH AX\n");	
			
		}
		else if(node->id == TK_ID && node->child==NULL && node->prev==NULL)
		{
			fprintf (fp, "\t\tPOP AX\n");
			fprintf (fp, "\t\tMOV [RBP - %d],AX\t\t\t\t\t\t\t\t;Store\n", searchVar(realBase, lst, node->tok->lexeme)->offset);	
		}
		else if(node->id == TK_ID && node->child!=NULL && node->prev==NULL) // A[i]:=j
		{
			varST* vst ;
			vst = searchVar(realBase, lst,node->tok->lexeme);

			if (isVarStaticArr(vst))
			{
				if(node->child->id == TK_ID)
				{
					staticArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp, "\n\t\tPOP AX\n") ;
					fprintf (fp, "\t\tMOV[RBP + RBX], AX\n") ;
				}
				else
				{
					fprintf (fp, "\t\tPOP AX\n");	
					fprintf (fp, "\t\tMOV [RBP - %d], AX\t\t\t\t\t\t\t\t;Store\n" , getStaticOffset(vst,node,2)) ;
				}
			}
			else
			{
				dynamicArrBoundCheck (node, lst, vst, fp) ;
				fprintf (fp, "\n\t\tPOP AX\n") ;
				fprintf (fp, "\t\tMOV[RDI + RBX], AX\n") ;
			}
		}
	}
}

void printArrayIntBool (tokenID baseType, FILE *fp)
{
	if (baseType == TK_INTEGER)
	{
		df |= 1 << printInt ;

		fprintf (fp, "\t\tMOV RDI, printInt\n") ;
		fprintf (fp, "\t\tMOVSX RSI, AX\n") ;
	}
	else
	{
		df |= 1 << DATA_true ;
		df |= 1 << DATA_false ;

		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\n\t\tCMP AX, 01\n") ;
		fprintf (fp, "\t\tJE .true\n") ;
		fprintf (fp, "\t\tMOV RDI, false\n") ;
		fprintf (fp, "\t\tJMP .afterBool\n") ;
		
		fprintf (fp, "\n\t.true:\n") ;
		fprintf (fp, "\t\tMOV RDI, true\n") ;
		fprintf (fp, "\n\t.afterBool:\n") ;
	}
}

void printGeneration (astNode *node, moduleST *lst, FILE *fp)
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
			fprintf (fp, "\t\tMOV AX, %s\n", node->tok->lexeme) ;
			fprintf (fp, "\t\tCALL printInteger\n") ;
		}
		else
		{
			tf |= 1 << printBoolean ;
			fprintf (fp, "\t\tMOV AX, %d\n", node->tok->lexeme[0]=='t'?1:0) ;
			fprintf (fp, "\t\tCALL printBoolean\n") ;
		}

		return ;
	}

	if (searchedVar->datatype == TK_INTEGER)
	{
		tf |= 1 << printInteger ;
		fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", searchedVar->offset) ;
		fprintf (fp, "\t\tCALL printInteger\n") ;
	}
	else if (searchedVar->datatype == TK_BOOLEAN)
	{
		tf |= 1 << printBoolean ;
		fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", searchedVar->offset) ;
		fprintf (fp, "\t\tCALL printBoolean\n") ;
	}
	else // Array type
	{	
		if (isVarStaticArr (searchedVar))
		{
			if (node->child != NULL && node->child->id == TK_NUM)	// Static array, static index
			{
				fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", getStaticOffset(searchedVar,node,2)) ;
				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					tf |= 1 << printInteger ;
					fprintf (fp, "\t\tCALL printInteger\n") ;
				}
				else
				{
					tf |= 1 << printBoolean ;
					fprintf (fp, "\t\tCALL printBoolean\n") ;
				}
			}	
			else if (node->child != NULL && node->child->id == TK_ID)
			{
				staticArrBoundCheck (node, lst, searchedVar, fp) ;
				fprintf (fp, "\t\tMOV AX, [RBP + RBX]\n") ;

				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					tf |= 1 << printInteger ;
					fprintf (fp, "\t\tCALL printInteger\n") ;
				}
				else
				{
					tf |= 1 << printBoolean ;
					fprintf (fp, "\t\tCALL printBoolean\n") ;
				}
			}
			else if (node->child == NULL)
			{
				int leftLim, rightLim ;
				leftLim  = atoi (searchedVar->arrayIndices->tokLeft->lexeme) ;
				rightLim = atoi (searchedVar->arrayIndices->tokRight->lexeme) ;

				//printArrayIntBool (searchedVar->arrayIndices->type, fp) ;
				fprintf (fp, "\t\tMOV RBX, -%d\n", searchedVar->offset - 2*(rightLim - leftLim)) ;
				fprintf (fp, "\t\tMOV RDX, %d\n", 2*(rightLim - leftLim + 1)) ;
				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					tf |= 1 << printStaticIntegerArr ;
					fprintf (fp, "\t\tCALL printStaticIntegerArr\n\n") ;
				}
				else
				{
					tf |= 1 << printStaticBooleanArr ;
					fprintf (fp, "\t\tCALL printStaticBooleanArr\n\n") ;
				}
			}
		}
		else
		{
			if (node->child != NULL)	// Dynamic array, static/dynamic index
			{
				dynamicArrBoundCheck (node, lst, searchedVar, fp) ;
				fprintf (fp, "\t\tMOV AX, [RDI + RBX]\n") ;

				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					tf |= 1 << printInteger ;
					fprintf (fp, "\t\tCALL printInteger\n") ;
				}
				else
				{
					tf |= 1 << printBoolean ;
					fprintf (fp, "\t\tCALL printBoolean\n") ;
				}
			}
			else 						// Print the whole array
			{
				reserveLabel[0] = get_label () ;
				df |= 1 << printFormatArray ;

				fprintf (fp, "\n\t\tMOV RDI, printFormatArray\t\t\t\t\t\t\t\t; printing array output prompt\n") ;
				fprintf (fp, "\t\tXOR RSI, RSI\n") ;
				fprintf (fp, "\t\tXOR RAX, RAX\n") ;
				fprintf (fp, "\t\tCALL printf\n\n") ;

				fprintf (fp, "\t\tMOV CX, 0\n") ;
				fprintf (fp, "LABEL%d:\t\t\t\t\t\t\t\t; printing array\n" , reserveLabel[0]) ;

				fprintf (fp, "\t\tMOV RDI, [RBP - %d]\n", searchedVar->offset) ;
				fprintf (fp, "\t\tMOVSX RBX, CX\n") ;

				fprintf (fp, "\t\tMOV AX, [RDI + RBX]\n") ;
				printArrayIntBool (searchedVar->arrayIndices->type, fp) ;

				fprintf (fp, "\t\tXOR RAX, RAX\n") ;
				fprintf (fp, "\t\tPUSH CX\n") ;
				fprintf (fp, "\t\tCALL printf\n") ;
				fprintf (fp, "\t\tPOP CX\n\n") ;

				fprintf (fp, "\t\tADD CX, 2\n") ;
				
				dynamicArrDX (searchedVar, fp) ;
				fprintf (fp, "\t\tCMP CX, DX\n") ;
				fprintf (fp, "\t\tJNE LABEL%d\n\n", reserveLabel[0]) ;

				df |= 1 << printNewLine ;
				fprintf (fp, "\n\t\tMOV RDI, printNewLine\t\t\t\t\t\t\t\t; newline after array print\n") ;
				fprintf (fp, "\t\tXOR RSI, RSI\n") ;
				fprintf (fp, "\t\tXOR RAX, RAX\n") ;
				fprintf (fp, "\t\tCALL printf\n") ;

			}
		}
	}
}

void getValueRSPAlign (FILE *fp)
{
	fprintf (fp, "\t\tMOV RAX, RSP\n") ;
	fprintf (fp, "\t\tAND RAX, 15\n") ;
	fprintf (fp, "\t\tADD RAX, 8\n") ;
	fprintf (fp, "\t\tSUB RSP, RAX\n") ;
	fprintf (fp, "\t\tPUSH RAX\n") ;
}

void getValueGeneration (moduleST *lst, varST *searchedVar, int rspDepth, FILE *fp)
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
		if (isVarStaticArr (searchedVar))
		{
			int leftLim, rightLim ;
			leftLim  = atoi (searchedVar->arrayIndices->tokLeft->lexeme) ;
			rightLim = atoi (searchedVar->arrayIndices->tokRight->lexeme) ;

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
			fprintf (fp, "\t\tMOV BX, %d\n", leftLim) ;
			fprintf (fp, "\t\tMOV CX, %d\n", rightLim) ;
			fprintf (fp, "\t\tCALL printGetArrPrompt\n\n") ;

			tf |= 1 << getStaticArr ;
			fprintf (fp, "\t\tMOV RBX, -%d\n", searchedVar->offset - 2*(rightLim-leftLim)) ;
			fprintf (fp, "\t\tCALL getStaticArr\n\n") ;
		}
		else
		{
			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				df |= 1 << inputIntArrPrompt ;
				fprintf (fp, "\n\t\tMOV RDI, inputIntArrPrompt\n") ;
			}
			else
			{
				df |= 1 << inputBoolArrPrompt ;
				fprintf (fp, "\n\t\tMOV RDI, inputBoolArrPrompt\n") ;
			}

			if (isdigit(searchedVar->arrayIndices->tokLeft->lexeme[0]))
				fprintf (fp, "\t\tMOV BX, %s\n", searchedVar->arrayIndices->tokLeft->lexeme) ;
			else
				fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", searchedVar->offset-10) ;

			if (isdigit(searchedVar->arrayIndices->tokRight->lexeme[0]))
				fprintf (fp, "\t\tMOV CX, %s\n", searchedVar->arrayIndices->tokRight->lexeme) ;		
			else
				fprintf (fp, "\t\tMOV CX, [RBP - %d]\n", searchedVar->offset-8) ;

			tf |= 1 << printGetArrPrompt ;
			fprintf (fp, "\t\tCALL printGetArrPrompt\n") ;

			tf |= 1 << getDynamicArr ;
			fprintf (fp, "\n\t\tMOV RDI, [RBP - %d]\n", searchedVar->offset) ;
			fprintf (fp, "\t\tCALL getDynamicArr\n") ;
		}
	}
}

void dynamicDeclareCheck (moduleST *lst, varST *searchedVar, FILE *fp)
{
	int start_label , end_label ;
	varST *leftVar , *rightVar ;
	tf |= 1 << dynamicDeclCheck ;

	fprintf (fp, "\n") ;
	if (isdigit(searchedVar->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\t\tMOV AX, %s\n", searchedVar->arrayIndices->tokLeft->lexeme) ;
	else
	{
		leftVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokLeft->lexeme) ;
		fprintf (fp, "\t\tMOV AX, [RBP - %d]\n", leftVar->offset) ;
	}

	if (isdigit(searchedVar->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\t\tMOV BX, %s\n", searchedVar->arrayIndices->tokRight->lexeme) ;
	else
	{
		rightVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokRight->lexeme) ;
		fprintf (fp, "\t\tMOV BX, [RBP - %d]\n", rightVar->offset) ;
	}

	fprintf (fp, "\t\tCALL dynamicDeclCheck\n\n") ;
}

void dynamicDeclareGeneration (moduleST *lst, varST *searchedVar, int declCount, FILE *fp)
{
	int start_label ;
	dynamicDeclareCheck (lst, searchedVar, fp) ;
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
		fprintf (fp, "\n\t\tCALL malloc\n") ;
		fprintf (fp, "\t\tMOV [RBP - %d], RAX\n", searchedVar->offset) ;
		fprintf (fp, "\t\tPOP AX\n") ;
		fprintf (fp, "\t\tMOV [RBP - %d], AX\n", searchedVar->offset - 10) ;
		fprintf (fp, "\t\tPOP BX\n") ;
		fprintf (fp, "\t\tMOV [RBP - %d], BX\n\n", searchedVar->offset - 8) ;
	}
}

void preamble (FILE *fp)
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


void postamble (FILE *fp)
{
	if (isFlagSet (tf, staticBoundCheck))
	{
		tf |= 1 << boundERROR ;

		fprintf (fp, "\nstaticBoundCheck:\n") ;
		fprintf (fp, "\t\tCMP BX, AX\n") ;
		fprintf (fp, "\t\tJGE .leftLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;

		fprintf (fp, "\n\t.leftLim:\n") ;
		fprintf (fp, "\t\tCMP CX, AX\n") ;
		fprintf (fp, "\t\tJGE .rightLim\n") ;
		fprintf (fp, "\t\tCALL boundERROR\n") ;

		fprintf (fp, "\n\t.rightLim:\n") ;
		fprintf (fp, "\t\tSUB CX, AX\n") ;
		fprintf (fp, "\t\tADD CX, CX\n") ;
		fprintf (fp, "\t\tSUB DX, CX\n\n") ;
		fprintf (fp, "\t\tSUB BX, AX\n") ;
		fprintf (fp, "\t\tADD BX, BX\n") ;
		fprintf (fp, "\t\tADD BX, DX\n") ;
		fprintf (fp,"\t\tNEG BX\n") ;
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
		fprintf (fp, "\t\tMOV DX, CX\n") ;
		fprintf (fp, "\t\tSUB DX, BX\n") ;
		fprintf (fp, "\t\tADD DX, 1\n") ; 
		fprintf (fp, "\t\tMOVSX RSI, DX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH BX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;
		fprintf (fp, "\t\tPOP BX\n") ;

		fprintf (fp, "\n\t\tMOV RDI, leftRange\n") ;
		fprintf (fp, "\t\tMOVSX RSI, BX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH CX\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;
		fprintf (fp, "\t\tPOP CX\n") ;

		fprintf (fp, "\n\t\tMOV RDI, rightRange\n") ;
		fprintf (fp, "\t\tMOVSX RSI, CX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH DX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP DX\n") ;

		fprintf (fp, "\n\t\tADD DX, DX\n") ;
		fprintf (fp, "\t\tMOVSX RDX, DX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet(tf, getStaticArr))
	{
		fprintf (fp, "\ngetStaticArr:\n") ;
		getValueRSPAlign (fp) ;

		fprintf (fp, "\n\t\tMOV RCX, 0\n") ;
		fprintf (fp, "\n\t.getArray:\t\t\t\t\t\t\t\t; getting array\n") ;

		fprintf (fp, "\t\tMOV RDI, inputInt\t\t\t\t\t\t\t\t; get_value\n") ;	
		fprintf (fp, "\t\tMOV RSI, RSP\n") ;
		fprintf (fp, "\t\tSUB RSI, 32\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RCX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tPUSH RSI\n") ;
		fprintf (fp, "\t\tCALL scanf\n") ;
		fprintf (fp, "\t\tPOP RSI\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RCX\n") ;
		fprintf (fp, "\t\tPOP RBX\n\n") ;

		fprintf (fp, "\t\tMOV AX, [RSP - 32]\n") ;
		fprintf (fp, "\t\tMOV [RBP + RBX], AX\n") ;
		fprintf (fp, "\t\tSUB RBX, 2\n") ;
		fprintf (fp, "\t\tADD RCX, 2\n") ;
		fprintf (fp, "\t\tCMP RCX, RDX\n") ;
		fprintf (fp, "\t\tJNE .getArray\n\n") ;
		fprintf (fp, "\t\tPOP RAX\n") ;
		fprintf (fp, "\t\tADD RSP, RAX\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getDynamicArr))
	{
		fprintf (fp, "\ngetDynamicArr:\n");
		getValueRSPAlign (fp) ;

		fprintf (fp, "\n\t\tPUSH RDI\n") ;
		fprintf (fp, "\t\tMOV RCX, 0\n") ;
		fprintf (fp, "\n\t.getArray:\t\t\t\t\t\t\t\t; getting array\n") ;

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
		fprintf (fp, "\t\tCMP RCX, RDX\n"/*, 2*(rightLim-leftLim+1)*/) ;
		fprintf (fp, "\t\tJNE .getArray\n\n") ;

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
		fprintf (fp, "\t\tMOVSX RSI, AX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf , printBoolean))
	{
		df |= 1 << printFalse ;
		df |= 1 << printTrue ;

		fprintf (fp, "\nprintBoolean:\n") ;
		fprintf (fp, "\t\tCMP AX, 01\n") ;
		fprintf (fp, "\t\tJE .true\n") ;
		fprintf (fp, "\t\tMOV RDI, printFalse\n") ;
		fprintf (fp, "\t\tJMP .boolPrint\n") ;

		fprintf (fp, "\n\t.true:\n") ;
		fprintf (fp, "\t\tMOV RDI, printTrue\n") ;
		fprintf (fp, "\n\t.boolPrint:\n") ;

		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printStaticIntegerArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << printInt ;
		df |= 1 << printNewLine ;

		fprintf (fp, "\nprintStaticIntegerArr:\n") ;
		fprintf (fp, "\n\t\tMOV RDI, printFormatArray\t\t\t\t\t\t\t\t; printing array output prompt\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tCALL printf\n\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RBX\n") ;
		
		fprintf (fp, "\t\tMOV RCX, 0\n") ;
		fprintf (fp, "\n\t.printArr:\t\t\t\t\t\t\t\t; printing array\n") ;

		fprintf (fp, "\t\tMOV AX, [RBP + RBX]\n") ;
		fprintf (fp, "\t\tMOV RDI, printInt\n") ;
		fprintf (fp, "\t\tMOVSX RSI, AX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RCX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RCX\n") ;
		fprintf (fp, "\t\tPOP RBX\n\n") ;

		fprintf (fp, "\t\tSUB RBX, 2\n") ;
		fprintf (fp, "\t\tADD RCX, 2\n") ;
		fprintf (fp, "\t\tCMP RCX, RDX\n") ;
		fprintf (fp, "\t\tJNE .printArr\n\n") ;

		fprintf (fp, "\n\t\tMOV RDI, printNewLine\t\t\t\t\t\t\t\t; newline after array print\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;

		fprintf (fp, "\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printStaticBooleanArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << printNewLine ;
		df |= 1 << DATA_true ;
		df |= 1 << DATA_false ;

		fprintf (fp, "\nprintStaticBooleanArr:\n") ;
		fprintf (fp, "\n\t\tMOV RDI, printFormatArray\t\t\t\t\t\t\t\t; printing array output prompt\n") ;
		fprintf (fp, "\t\tXOR RSI, RSI\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tCALL printf\n\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RBX\n") ;
		
		fprintf (fp, "\t\tMOV RCX, 0\n") ;
		fprintf (fp, "\n\t.printArr:\t\t\t\t\t\t\t\t; printing array\n") ;

		fprintf (fp, "\t\tMOV AX, [RBP + RBX]\n") ;
		fprintf (fp, "\n\t\tCMP AX, 01\n") ;
		fprintf (fp, "\t\tJE .true\n") ;
		fprintf (fp, "\t\tMOV RDI, false\n") ;
		fprintf (fp, "\t\tJMP .afterBool\n") ;
		
		fprintf (fp, "\n\t.true:\n") ;
		fprintf (fp, "\t\tMOV RDI, true\n") ;
		fprintf (fp, "\n\t.afterBool:\n") ;

		fprintf (fp, "\t\tMOVSX RSI, AX\n") ;
		fprintf (fp, "\t\tXOR RAX, RAX\n") ;
		fprintf (fp, "\t\tPUSH RBX\n") ;
		fprintf (fp, "\t\tPUSH RCX\n") ;
		fprintf (fp, "\t\tPUSH RDX\n") ;
		fprintf (fp, "\t\tCALL printf\n") ;
		fprintf (fp, "\t\tPOP RDX\n") ;
		fprintf (fp, "\t\tPOP RCX\n") ;
		fprintf (fp, "\t\tPOP RBX\n\n") ;

		fprintf (fp, "\t\tSUB RBX, 2\n") ;
		fprintf (fp, "\t\tADD RCX, 2\n") ;
		fprintf (fp, "\t\tCMP RCX, RDX\n") ;
		fprintf (fp, "\t\tJNE .printArr\n\n") ;

		fprintf (fp, "\n\t\tMOV RDI, printNewLine\t\t\t\t\t\t\t\t; newline after array print\n") ;
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

	/* --------------------------------------------------------------------------------------------- */
	printCommentLineNASM (fp) ;

	if (df)
		fprintf (fp, "\nsection .data\n") ;

	if (isFlagSet (df, boundPrint))
	{
		fprintf (fp, "\t\tboundPrint : ") ;
		fprintf (fp, "db \"Array out of bounds\" , 10, 0\n") ;	
	}
	
	if (isFlagSet (df, declPrint))
	{
		fprintf (fp, "\t\tdeclPrint : ") ;
		fprintf (fp, "db \"Invalid order of bounds in dynamic array declaration\" , 10, 0\n") ;		
	}
	
	if (isFlagSet (df, declNeg))
	{
		fprintf (fp, "\t\tdeclNeg : ") ;
		fprintf (fp, "db \"Negative bound in dynamic array declaration\" , 10, 0\n") ;	
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
			if (statementsNode->child->id == TK_DECLARE)
				return cnt ;

			statementsNode = statementsNode->next->next->next ;
		}
		else
			statementsNode = NULL ;
	}

	return cnt ;
}


int switchDeclareVars (astNode *statementsNode, varST *vst , int rspDepth, FILE* fp)
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
				rspDepth = moduleGeneration (statementNode->child, rspDepth, rspDepth, statementsNode->localST, vst, fp) ;
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
						rspDepth = moduleGeneration (statementNode->child, rspDepth, rspDepth, statementsNode->localST, vst, fp) ;
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

int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels, FILE* fp)
{
	int i, def_label = -1 ;

	for (i = 0 ; i < caseCount ; i++)
		caseLabels[i] = get_label() ;

	varST *switchVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
	fprintf (fp, "\n\t\tMOV AX, [RBP - %d]\n", switchVar->offset) ;

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

void printCommentLineNASM (FILE *fp)
{
	fprintf (fp, "\n;--------------------------------------------------------------------------------------------------\n") ;
}