#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
//#include "ast.h"
//#include "symbolTable.h"
#include "typeChecker.h"
#include "codegen.h"

baseST *realBase ;
int x = 0 ;

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
	start_label = get_label () ;
	end_label = get_label () ;

	leftLim = atoi (vst->arrayIndices->tokLeft->lexeme) ;
	rightLim = atoi (vst->arrayIndices->tokRight->lexeme) ;

	fprintf (fp, "\tMOV AX, [RBP-%d]\n", indexVar->offset) ;
	fprintf (fp, "\tMOV BX, %d\n", leftLim) ;
	fprintf (fp, "\tCMP AX, BX\n") ;
	fprintf (fp, "\tJGE LABEL%d\n", start_label) ;
	fprintf (fp, "\tCALL boundERROR\n") ;

	fprintf (fp, "\nLABEL%d:\n", start_label) ;
	fprintf (fp, "\tMOV BX, %d\n", rightLim) ;
	fprintf (fp, "\tCMP BX, AX\n") ;
	fprintf (fp, "\tJGE LABEL%d\n", end_label) ;
	fprintf (fp, "\tCALL boundERROR\n") ;

	fprintf (fp, "\nLABEL%d:\n", end_label) ;
	fprintf (fp, "\tMOV BX, %d\n", vst->offset - 2*(rightLim-leftLim)) ;
	fprintf (fp, "\tMOV AX, [RBP-%d]\n", indexVar->offset) ;
	fprintf (fp, "\tSUB AX, %d\n", leftLim) ;
	fprintf (fp, "\tADD AX, AX\n") ;
	fprintf (fp, "\tADD BX, AX\n") ;
	fprintf (fp ,"\tNEG BX\n") ;
	fprintf (fp, "\tMOVSX RBX, BX\n") ;
}

void dynamicArrBoundCheck (astNode *node, moduleST *lst, varST *vst, FILE *fp)
{
	int start_label, end_label ;

	start_label = get_label () ;
	end_label = get_label () ;

	if (isdigit(node->child->tok->lexeme[0]))
		fprintf (fp, "\n\tMOV AX, %s\n", node->child->tok->lexeme) ;
	else
	{
		varST *indexVar ;
		indexVar = searchVar (realBase, lst, node->child->tok->lexeme) ;

		fprintf (fp, "\tMOV AX, [RBP-%d]\n", indexVar->offset) ;
	}

	if (isdigit(vst->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\tMOV BX, %s", vst->arrayIndices->tokRight->lexeme) ;		
	else
		fprintf (fp, "\tMOV BX, [RBP-%d]\n", vst->offset-8) ;

	fprintf (fp, "\tCMP BX, AX\n") ;
	fprintf (fp, "\tJGE LABEL%d\n", start_label) ;
	fprintf (fp, "\tCALL boundERROR\n") ;

	fprintf (fp, "\nLABEL%d:\n", start_label) ;
	if (isdigit(vst->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\tMOV BX, %s", vst->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\tMOV BX, [RBP-%d]\n", vst->offset-10) ;

	fprintf (fp, "\tCMP AX, BX\n") ;
	fprintf (fp, "\tJGE LABEL%d\n", end_label) ;
	fprintf (fp, "\tCALL boundERROR\n") ;
	
	fprintf (fp, "\nLABEL%d:\n", end_label) ;
	fprintf (fp, "\tMOV RDI, [RBP-%d]\n", vst->offset) ;
	fprintf (fp, "\tSUB AX, BX\n") ;
	fprintf (fp, "\tADD AX, AX\n") ;
	fprintf (fp, "\tMOVSX RBX, AX\n") ;
}


void printGetValueStaticArrayPrompt (tokenID baseType , int leftLim, int rightLim, FILE* fp)
{
	if (baseType == TK_INTEGER)
		fprintf (fp, "\tMOV RDI, inputIntArrPrompt\n") ;
	else
		fprintf (fp, "\tMOV RDI, inputBoolArrPrompt\n") ;

	fprintf (fp, "\tMOV RSI, %d\n", rightLim - leftLim + 1) ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;

	fprintf (fp, "\n\tMOV RDI, leftRange\n") ;
	fprintf (fp, "\tMOV RSI, %d\n", leftLim) ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;

	fprintf (fp, "\n\tMOV RDI, rightRange\n") ;
	fprintf (fp, "\tMOV RSI, %d\n", rightLim) ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
}


void printGetValueDynArrayPrompt (varST *vst, FILE *fp)
{
	if (vst->arrayIndices->type == TK_INTEGER)
		fprintf (fp, "\n\tMOV RDI, inputIntArrPrompt\n") ;
	else
		fprintf (fp, "\n\tMOV RDI, inputBoolArrPrompt\n") ;

	if (isdigit(vst->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\tMOV AX, %s", vst->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\tMOV AX, [RBP-%d]\n", vst->offset-10) ;

	if (isdigit(vst->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\tMOV BX, %s", vst->arrayIndices->tokRight->lexeme) ;		
	else
		fprintf (fp, "\tMOV BX, [RBP-%d]\n", vst->offset-8) ;

	fprintf (fp, "\tMOV SI, BX\n") ;
	fprintf (fp, "\tSUB SI, AX\n") ;
	fprintf (fp, "\tADD SI, 1\n") ;
	fprintf (fp, "\tMOVSX RSI, SI\n") ;
	fprintf (fp, "\tPUSH AX\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
	fprintf (fp, "\tPOP AX\n") ;

	fprintf (fp, "\n\tMOV RDI, leftRange\n") ;
	fprintf (fp, "\tMOVSX RSI, AX\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;

	fprintf (fp, "\n\tMOV RDI, rightRange\n") ;
	fprintf (fp, "\tMOVSX RSI, BX\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
}

void dynamicArrDX (varST *vst, FILE *fp)
{
	if (isdigit(vst->arrayIndices->tokLeft->lexeme[0]))
		fprintf (fp, "\tMOV AX, %s", vst->arrayIndices->tokLeft->lexeme) ;
	else
		fprintf (fp, "\tMOV AX, [RBP-%d]\n", vst->offset-10) ;

	if (isdigit(vst->arrayIndices->tokRight->lexeme[0]))
		fprintf (fp, "\tMOV BX, %s", vst->arrayIndices->tokRight->lexeme) ;
	else
		fprintf (fp, "\tMOV BX, [RBP-%d]\n", vst->offset-8) ;

	fprintf (fp, "\tMOV DX, BX\n") ;
	fprintf (fp, "\tSUB DX, AX\n") ;
	fprintf (fp, "\tADD DX, 1\n") ;
	fprintf (fp, "\tADD DX, DX\n") ;

}

void dynamicDeclare (moduleST *lst, varST *searchedVar, FILE *fp)
{
	int start_label , end_label ;
	varST *leftVar , *rightVar ;

	if (isdigit(searchedVar->arrayIndices->tokLeft->lexeme[0]))
	{
		fprintf (fp, "\tMOV AX, %s\n", searchedVar->arrayIndices->tokLeft->lexeme) ;
		fprintf (fp, "\tMOV [RBP-%d], AX\n", searchedVar->offset - 10) ;
	}
	else
	{
	 	start_label = get_label () ;

		leftVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokLeft->lexeme) ;
		fprintf (fp, "\tMOV AX, [RBP-%d]\n", leftVar->offset) ;
		fprintf (fp, "\tCMP AX, 0\n") ;
		fprintf (fp, "\tJGE LABEL%d\n", start_label) ;
		fprintf (fp, "\tCALL declNegERROR\n") ;

		fprintf (fp, "\nLABEL%d:\n", start_label) ;
		fprintf (fp, "\tMOV [RBP-%d], AX\n", searchedVar->offset - 10) ;
	}

	if (isdigit(searchedVar->arrayIndices->tokRight->lexeme[0]))
	{
		fprintf (fp, "\tMOV BX, %s\n", searchedVar->arrayIndices->tokRight->lexeme) ;
		fprintf (fp, "\tMOV [RBP-%d], BX\n", searchedVar->offset - 8) ;
	}
	else
	{
		end_label = get_label () ;

		rightVar = searchVar (realBase, lst, searchedVar->arrayIndices->tokRight->lexeme) ;
		fprintf (fp, "\tMOV BX, [RBP-%d]\n", rightVar->offset) ;
		fprintf (fp, "\tCMP BX, 0\n") ;
		fprintf (fp, "\tJGE LABEL%d\n", end_label) ;
		fprintf (fp, "\tCALL declNegERROR\n") ;
		
		fprintf (fp, "\nLABEL%d:\n", end_label) ;
		fprintf (fp, "\tMOV [RBP-%d], BX\n", searchedVar->offset - 8) ;
	}

	int lab ;
	lab = get_label () ;

	fprintf (fp, "\tCMP BX, AX\n") ;
	fprintf (fp, "\tJGE LABEL%d\n", lab) ;
	fprintf (fp, "\tCALL declERROR\n") ;
	fprintf (fp, "\nLABEL%d:\n", lab) ;
	fprintf (fp, "\tSUB BX, AX\n") ;
	fprintf (fp, "\tADD BX, 1\n") ;
	fprintf (fp, "\tADD BX, BX\n") ;
	fprintf (fp, "\tMOVSX RDI, BX\n") ;
}

void IDGeneration (astNode *node, moduleST *lst, varST *vst, FILE* fp)
{
	if (node->parent->id!=TK_ASSIGNOP)
	{
		if (node->prev == NULL)
		{
			if(node->id == TK_ID && node->child==NULL)
			{
				fprintf (fp, "\tMOV AX, [rbp - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
				fprintf (fp, "\tPUSH AX\n");	
			}

			else if(node->id == TK_ID && node->child!=NULL) // var:=A[i]+j;
			{
				varST *vst ;
				vst = searchVar (realBase, lst,node->tok->lexeme);

				if(node->child->id == TK_ID) // For dynamic index
				{
					if (isVarStaticArr(vst))
					{
						staticArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\tMOV AX, [RBP + RBX]\n") ;
					}
					else
					{
						dynamicArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\tMOV AX, [RDI + RBX]\n") ;
					}
				}
				else
					fprintf (fp, "\tMOV AX, [RBP-%d]\n", getStaticOffset (vst, node, 2)) ;
				
				fprintf (fp, "\tPUSH AX\n");	
			}
		}
		else
		{
			if(node->id == TK_ID && node->child==NULL)
			{
				fprintf (fp, "\tMOV BX, [RBP - %d]\n",searchVar(realBase, lst, node->tok->lexeme)->offset);
				fprintf (fp, "\tPUSH BX\n");	
			}

			else if(node->id == TK_ID && node->child!=NULL) // var:=j+A[i];
			{
				varST *vst ;
				vst=searchVar (realBase, lst,node->tok->lexeme);

				if(node->child->id == TK_ID) // For dynamic index
				{
					if (isVarStaticArr(vst))
					{
						staticArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\tMOV BX, [RBP + RBX]\n") ;
					}
					else
					{
						dynamicArrBoundCheck (node, lst, vst, fp) ;
						fprintf (fp, "\tMOV BX, [RDI + RBX]\n") ;
					}
				}
				else
					fprintf (fp, "\tMOV BX, [RBP - %d]\n",getStaticOffset(vst,node,2)) ;
					
				fprintf (fp, "\tPUSH BX\n");	
			}
		}
	}
	else
	{
		if(node->id == TK_ID && node->child==NULL && node->prev!=NULL)
		{
			fprintf (fp, "\tMOV AX, [rbp - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			fprintf (fp, "\tPUSH AX\n");	
		}
		else if(node->id == TK_ID && node->child!=NULL && node->prev!=NULL) //type checking required i:=A[j]
		{
			varST *vst ;
			vst = searchVar (realBase, lst,node->tok->lexeme);
			int size ;

			if(node->child->id == TK_ID)
			{
				if (isVarStaticArr(vst))
				{
					staticArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp , "\tMOV AX, [RBP + RBX]\n") ;
				}
				else
				{
					dynamicArrBoundCheck (node, lst, vst, fp) ;
					fprintf (fp, "\tMOV AX, [RDI + RBX]\n") ;
				}
			}
			else
				fprintf (fp, "\tMOV AX, [RBP - %d]\n", getStaticOffset(vst, node, 2)) ;
			
			fprintf (fp, "\tPUSH AX\n");	
			
		}
		else if(node->id == TK_ID && node->child==NULL && node->prev==NULL)
		{
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tMOV [rbp - %d],AX\t\t;Store\n", searchVar(realBase, lst, node->tok->lexeme)->offset);	
		}
		else if(node->id == TK_ID && node->child!=NULL && node->prev==NULL) // A[i]:=j
		{
			varST* vst ;
			vst = searchVar(realBase, lst,node->tok->lexeme);

			if(node->child->id == TK_ID)
			{
				if (isVarStaticArr (vst))
				{
					staticArrBoundCheck (node, lst, vst, fp) ;

					fprintf (fp, "\n\tPOP AX\n") ;
					fprintf (fp, "\tMOV[RBP + RBX], AX\n") ;
				}
				else
				{
					dynamicArrBoundCheck (node, lst, vst, fp) ;

					fprintf (fp, "\n\tPOP AX\n") ;
					fprintf (fp, "\tMOV[RDI + RBX], AX\n") ;
				}
			}
			else
			{
				fprintf (fp, "\tPOP AX\n");	
				fprintf (fp, "\tMOV [RBP - %d], AX\t\t;Store\n" , getStaticOffset(vst,node,2)) ;
			}
		}
	}
}

void printGeneration (astNode *node, moduleST *lst, varST *searchedVar, FILE *fp)
{
	int start_label, end_label ;
	int reserveLabel[2] ;

	if (searchedVar->datatype == TK_INTEGER)
	{
		fprintf (fp, "\tMOV AX, [RBP - %d]\t\t;printing integer\n", searchedVar->offset) ;
		fprintf (fp, "\tMOV RDI, printFormat\n") ;
		fprintf (fp, "\tMOVSX RSI, AX\n") ;
		fprintf (fp, "\tXOR RAX, RAX\n") ;
		fprintf (fp, "\tCALL printf\n") ;
	}
	else if (searchedVar->datatype == TK_BOOLEAN)
	{
		start_label = get_label () ;
		end_label = get_label () ;

		fprintf (fp, "\tMOV AX, [RBP - %d]\t\t;printing boolean\n", searchedVar->offset) ;
		fprintf (fp, "\tCMP AX, 01\n") ;
		fprintf (fp, "\tJE LABEL%d\n", start_label) ;
		fprintf (fp, "\tMOV RDI, printFalse\n") ;
		fprintf (fp, "\tJMP LABEL%d\n", end_label) ;

		fprintf (fp, "LABEL%d:\n", start_label) ;
		fprintf (fp, "\tMOV RDI, printTrue\n") ;
		fprintf (fp, "LABEL%d:\n", end_label) ;

		fprintf (fp, "\tXOR RSI, RSI\n") ;
		fprintf (fp, "\tXOR RAX, RAX\n") ;
		fprintf (fp, "\tCALL printf\n\n") ;
	}
	else // Array type
	{	
		if (isVarStaticArr (searchedVar))
		{
			if (node->child != NULL && node->child->id == TK_NUM)	// Static array, static index
			{
				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					fprintf (fp, "\tMOV AX, [RBP - %d]\t\t;printing integer\n", getStaticOffset(searchedVar,node,2)) ;
					fprintf (fp, "\tMOV RDI, printFormat\n") ;
					fprintf (fp, "\tMOVSX RSI, AX\n") ;
					fprintf (fp, "\tXOR RAX, RAX\n") ;
					fprintf (fp, "\tCALL printf\n") ;
				}
				else
				{
					start_label = get_label () ;
					end_label = get_label () ;

					fprintf (fp, "\tMOV AX, [RBP - %d]\t\t;printing boolean\n", getStaticOffset(searchedVar,node,2)) ;
					fprintf (fp, "\tCMP AX, 01\n") ;
					fprintf (fp, "\tJE LABEL%d\n", start_label) ;
					fprintf (fp, "\tMOV RDI, printFalse\n") ;
					fprintf (fp, "\tJMP LABEL%d\n", end_label) ;

					fprintf (fp, "LABEL%d:\n", start_label) ;
					fprintf (fp, "\tMOV RDI, printTrue\n") ;
					fprintf (fp, "LABEL%d:\n", end_label) ;

					fprintf (fp, "\tXOR RSI, RSI\n") ;
					fprintf (fp, "\tXOR RAX, RAX\n") ;
					fprintf (fp, "\tCALL printf\n\n") ;
				}
			}	
			else if (node->child != NULL && node->child->id == TK_ID)
			{
				staticArrBoundCheck (node, lst, searchedVar, fp) ;

				fprintf (fp, "\tMOV AX, [RBP + RBX]\t\t;printing integer\n") ;
				fprintf (fp, "\tMOV RDI, printFormat\n") ;
				fprintf (fp, "\tMOVSX RSI, AX\n") ;
				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp, "\tCALL printf\n") ;
			}
			else if (node->child == NULL)
			{
				int leftLim, rightLim ;
				leftLim  = atoi (searchedVar->arrayIndices->tokLeft->lexeme) ;
				rightLim = atoi (searchedVar->arrayIndices->tokRight->lexeme) ;

				reserveLabel[0] = get_label () ;

				fprintf (fp, "\n\tMOV RDI, printFormatArray\t\t;printing array output prompt\n") ;
				fprintf (fp, "\tXOR RSI, RSI\n") ;
				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp , "\tCALL printf\n\n") ;

				fprintf (fp, "\tMOV RCX, 0\n") ;
				fprintf (fp, "LABEL%d:\t\t\t;printing array\n" , reserveLabel[0]) ;

				fprintf (fp, "\tMOV RBX, %d\n", searchedVar->offset - 2*(rightLim-leftLim)) ;
				fprintf (fp, "\tADD RBX, RCX\n\n") ;
				fprintf (fp, "\tNEG RBX\n") ;

				fprintf (fp, "\tMOV AX, [RBP + RBX]\n") ;
				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					fprintf (fp, "\tMOV RDI, printInt\n") ;
					fprintf (fp, "\tMOVSX RSI, AX\n") ;
				}
				else
				{
					start_label = get_label () ;
					end_label = get_label () ;

					fprintf (fp, "\tXOR RSI, RSI\n") ;
					fprintf (fp, "\n\tCMP AX, 01\n") ;
					fprintf (fp, "\tJE LABEL%d\n", start_label) ;
					fprintf (fp, "\tMOV RDI, false\n") ;
					fprintf (fp, "\tJMP LABEL%d\n", end_label) ;

					fprintf (fp, "LABEL%d:\n", start_label) ;
					fprintf (fp, "\tMOV RDI, true\n") ;
					fprintf (fp, "\nLABEL%d:\n", end_label) ;
				}

				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp, "\tPUSH RCX\n") ;
				fprintf (fp, "\tPUSH RBX\n") ;
				fprintf (fp, "\tCALL printf\n") ;
				fprintf (fp, "\tPOP RBX\n") ;
				fprintf (fp, "\tPOP RCX\n\n") ;

				fprintf (fp, "\tADD RCX, 2\n") ;
				fprintf (fp, "\tCMP RCX, %d\n", 2*(rightLim-leftLim+1)) ;
				fprintf (fp, "\tJNE LABEL%d\n\n", reserveLabel[0]) ;

				fprintf (fp, "\n\tMOV RDI, printNewLine\t\t; newline after array print\n") ;
				fprintf (fp, "\tXOR RSI, RSI\n") ;
				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp , "\tCALL printf\n") ;
			}
		}
		else
		{
			if (node->child != NULL)	// Dynamic array, static index
			{
				dynamicArrBoundCheck (node, lst, searchedVar, fp) ;

				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					fprintf (fp, "\tMOV RDI, printFormat\n") ;
					fprintf (fp, "\tMOVSX RSI, AX\n") ;
					fprintf (fp, "\tXOR RAX, RAX\n") ;
					fprintf (fp, "\tCALL printf\n") ;
				}
				else
				{
					start_label = get_label () ;
					end_label = get_label () ;

					fprintf (fp, "\n\tCMP AX, 01\n") ;
					fprintf (fp, "\tJE LABEL%d\n", start_label) ;
					fprintf (fp, "\tMOV RDI, false\n") ;
					fprintf (fp, "\tJMP LABEL%d\n", end_label) ;

					fprintf (fp, "LABEL%d:\n", start_label) ;
					fprintf (fp, "\tMOV RDI, true\n") ;
					fprintf (fp, "\nLABEL%d\n", end_label) ;

					fprintf (fp, "\tXOR RSI, RSI\n") ;
					fprintf (fp, "\tXOR RAX, RAX\n") ;
					fprintf (fp, "\tCALL printf\n\n") ;
				}
			}
			else 						// Print the whole array
			{
				reserveLabel[0] = get_label () ;

				fprintf (fp, "\n\tMOV RDI, printFormatArray\t\t;printing array output prompt\n") ;
				fprintf (fp, "\tXOR RSI, RSI\n") ;
				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp , "\tCALL printf\n\n") ;

				fprintf (fp, "\tMOV CX, 0\n") ;
				fprintf (fp, "LABEL%d:\t\t\t;printing array\n" , reserveLabel[0]) ;

				fprintf (fp, "\tMOV RDI, [RBP - %d]\n", searchedVar->offset) ;
				fprintf (fp, "\tMOVSX RBX, CX\n") ;
				fprintf (fp, "\tMOV AX, [RDI + RBX]\n") ;
				if (searchedVar->arrayIndices->type == TK_INTEGER)
				{
					fprintf (fp, "\tMOV RDI, printInt\n") ;
					fprintf (fp, "\tMOVSX RSI, AX\n") ;
				}
				else
				{
					start_label = get_label () ;
					end_label = get_label () ;

					fprintf (fp, "\n\tCMP AX, 01\n") ;
					fprintf (fp, "\tJE LABEL%d\n", start_label) ;
					fprintf (fp, "\tMOV RDI, false\n") ;
					fprintf (fp, "\tJMP LABEL%d\n", end_label) ;

					fprintf (fp, "LABEL%d:\n", start_label) ;
					fprintf (fp, "\tMOV RDI, true\n") ;
					fprintf (fp, "\nLABEL%d:\n", end_label) ;
				}

				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp, "\tPUSH CX\n") ;
				fprintf (fp, "\tCALL printf\n") ;
				fprintf (fp, "\tPOP CX\n\n") ;

				fprintf (fp, "\tADD CX, 2\n") ;
				
				dynamicArrDX (searchedVar, fp) ;
				fprintf (fp, "\tCMP CX, DX\n") ;
				fprintf (fp, "\tJNE LABEL%d\n\n", reserveLabel[0]) ;

				fprintf (fp, "\n\tMOV RDI, printNewLine\t\t; newline after array print\n") ;
				fprintf (fp, "\tXOR RSI, RSI\n") ;
				fprintf (fp, "\tXOR RAX, RAX\n") ;
				fprintf (fp , "\tCALL printf\n") ;

			}
		}
	}
}

void getValueGeneration (moduleST *lst, varST *searchedVar, int rspDepth, FILE *fp)
{		
	int rspAlign ;
	int reserveLabel[2] ;

	if (searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN)
	{
		rspAlign = 32 - (rspDepth % 16) ;

		if (searchedVar->datatype == TK_INTEGER)
			fprintf (fp, "\n\tMOV RDI, inputIntPrompt\t\t;get_value\n") ;
		else if (searchedVar->datatype == TK_BOOLEAN)
			fprintf (fp , "\n\tMOV RDI, inputBoolPrompt\t\t;get_value\n") ;
		fprintf (fp, "\tXOR RSI, RSI\n") ;
		fprintf (fp, "\tXOR RAX, RAX\n") ;
		fprintf (fp, "\tCALL printf\n") ;

		fprintf (fp, "\n\tMOV RDI, inputInt\t\t;get_value\n") ;
		fprintf (fp, "\tSUB RSP, %d\n", rspAlign) ;
		fprintf (fp, "\tMOV RSI, RSP\n") ;
		fprintf (fp, "\tPUSH RSI\n") ;
		fprintf (fp, "\tCALL scanf\n") ;
		fprintf (fp, "\tPOP RSI\n") ;
		fprintf (fp, "\tMOV AX, WORD [RSP]\n") ;
		fprintf (fp, "\tMOV [RBP - %d], AX\n", searchedVar->offset) ;
		fprintf (fp, "\tADD RSP, %d\n\n", rspAlign) ;
	}
	else // Array type
	{	
		reserveLabel[0] = get_label () ;
		rspAlign = 32 - (rspDepth % 16) ;

		if (isVarStaticArr (searchedVar))
		{
			int leftLim, rightLim ;
			leftLim  = atoi (searchedVar->arrayIndices->tokLeft->lexeme) ;
			rightLim = atoi (searchedVar->arrayIndices->tokRight->lexeme) ;

			printGetValueStaticArrayPrompt (searchedVar->arrayIndices->type, leftLim, rightLim, fp) ;

			fprintf (fp, "\tSUB RSP, %d\n", rspAlign) ;
			fprintf (fp, "\tMOV RCX, 0\n") ;
			fprintf (fp, "LABEL%d:\t\t\t;getting array\n" , reserveLabel[0]) ;
			fprintf (fp, "\tMOV RBX, %d\n", searchedVar->offset - 2*(rightLim-leftLim)) ;
			fprintf (fp, "\tADD RBX, RCX\n\n") ;
			fprintf (fp, "\tNEG RBX\n") ;

			fprintf (fp, "\n\tMOV RDI, inputInt\t\t;get_value\n") ;
			fprintf (fp, "\tMOV RSI, RSP\n") ;
			fprintf (fp, "\tPUSH RBX\n") ;
			fprintf (fp, "\tPUSH RCX\n") ;
			fprintf (fp, "\tPUSH RSI\n") ;
			fprintf (fp, "\tCALL scanf\n") ;
			fprintf (fp, "\tPOP RSI\n") ;
			fprintf (fp, "\tPOP RCX\n") ;
			fprintf (fp, "\tPOP RBX\n") ;

			fprintf (fp, "\tMOV AX, [RSP]\n") ;
			fprintf (fp, "\tMOV [RBP+RBX], AX\n") ;

			fprintf (fp, "\tADD RCX, 2\n") ;
			fprintf (fp, "\tCMP RCX, %d\n", 2*(rightLim-leftLim+1)) ;
			fprintf (fp, "\tJNE LABEL%d\n\n", reserveLabel[0]) ;
			fprintf (fp, "\tADD RSP, %d\n", rspAlign) ;
		}
		else
		{
			printGetValueDynArrayPrompt (searchedVar, fp) ;

			fprintf (fp, "\tSUB RSP, %d\n", rspAlign) ;
			fprintf (fp, "\tMOV RCX, 0\n") ;
			fprintf (fp, "\nLABEL%d:\t\t\t;getting array\n" , reserveLabel[0]) ;

			fprintf (fp, "\tMOV RBX, RCX\n") ;
			fprintf (fp, "\n\tMOV RDI, inputInt\t\t;get_value\n") ;
			fprintf (fp, "\tMOV RSI, RSP\n") ;
			fprintf (fp, "\tPUSH RBX\n") ;
			fprintf (fp, "\tPUSH RCX\n") ;
			fprintf (fp, "\tPUSH RSI\n") ;
			fprintf (fp, "\tCALL scanf\n") ;
			fprintf (fp, "\tPOP RSI\n") ;
			fprintf (fp, "\tPOP RCX\n") ;
			fprintf (fp, "\tPOP RBX\n") ;

			fprintf (fp, "\tMOV AX, [RSP]\n") ;
			fprintf (fp, "\tMOV RDI, [RBP-%d]\n", searchedVar->offset) ;
			fprintf (fp, "\tMOV [RDI + RBX], AX\n") ;

			fprintf (fp, "\tADD RCX, 2\n") ;	// Incrementing counter

			// Finding (rightLim - leftLim + 1) 
			dynamicArrDX (searchedVar, fp) ;

			fprintf (fp, "\tMOVSX RDX, DX\n") ;
			fprintf (fp, "\tCMP RCX, RDX\n"/*, 2*(rightLim-leftLim+1)*/) ;
			fprintf (fp, "\tJNE LABEL%d\n\n", reserveLabel[0]) ;

			fprintf (fp, "\tADD RSP, %d\n", rspAlign) ;
		}
	}
}


void preamble (FILE *fp)
{
	fprintf (fp, "section .data\n") ;

	fprintf (fp, "\tboundPrint : ") ;
	fprintf (fp, "db \"Array out of bounds\" , 10, 0\n") ;

	fprintf (fp, "\tdeclPrint : ") ;
	fprintf (fp, "db \"Invalid order of bounds in dynamic array declaration\" , 10, 0\n") ;	

	fprintf (fp, "\tdeclNeg : ") ;
	fprintf (fp, "db \"Negative bound in dynamic array declaration\" , 10, 0\n") ;	

	fprintf (fp, "\tprintFormatArray : ") ;
	fprintf (fp, "db \"Output : \" , 0\n") ;

	fprintf (fp , "\tprintInt : ") ;
	fprintf (fp, "db \"%%d \", 0\n") ;

	fprintf (fp, "\tprintNewLine : ") ;
	fprintf (fp, "db 10, 0\n") ;

	fprintf (fp, "\tprintFormat : ") ;
	fprintf (fp, "db \"Output :  %%d\" , 10, 0\n") ;

	fprintf (fp, "\tprintTrue : ") ;
	fprintf (fp, "db \"Output : true\" , 10, 0\n") ;

	fprintf (fp, "\tprintFalse : ") ;
	fprintf (fp, "db \"Output : false\" , 10, 0\n") ;

	fprintf (fp, "\ttrue : ") ;
	fprintf (fp, "db \"true \" , 0\n") ;

	fprintf (fp, "\tfalse : ") ;
	fprintf (fp, "db \"false \" , 0\n") ;

	fprintf (fp, "\tinputIntPrompt : ") ;
	fprintf (fp, "db \"Enter an integer : \" , 0\n") ;

	fprintf (fp, "\tinputBoolPrompt : ") ;
	fprintf (fp, "db \"Enter a boolean (0 or 1) : \" , 0\n") ;

	fprintf (fp, "\tinputIntArrPrompt : ") ;
	fprintf (fp, "db \"Enter %%d array elements of integer type for range \", 0\n") ;

	fprintf (fp, "\tinputBoolArrPrompt : ") ;
	fprintf (fp, "db \"Enter %%d array elements of boolean type for range \", 0\n") ;

	fprintf (fp, "\tleftRange : ") ;
	fprintf (fp, "db \"%%d to \" , 0\n") ;

	fprintf (fp, "\trightRange : ") ;
	fprintf (fp, "db \"%%d\" ,10, 0\n") ;

	fprintf (fp, "\tinputInt : ") ;
	fprintf (fp, "db \"%%d\", 0\n") ;

	fprintf (fp, "global main\n") ;
	fprintf (fp, "extern scanf\n") ;
	fprintf (fp, "extern exit\n") ;
	fprintf (fp, "extern malloc\n") ;
	fprintf (fp, "extern printf\n\n") ;

	fprintf (fp, "section .text\n") ;
	fprintf (fp, "boundERROR:\n") ;
	fprintf (fp, "\tPUSH RBP\n") ;
	fprintf (fp, "\tMOV RBP, RSP\n") ;
	fprintf (fp, "\tMOV RDI, boundPrint\n") ;
	fprintf (fp, "\tXOR RSI, RSI\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
	fprintf (fp, "\tMOV EDI, 0\n") ;
	fprintf (fp, "\tcall exit\n") ;

	fprintf (fp, "declERROR:\n") ;
	fprintf (fp, "\tPUSH RBP\n") ;
	fprintf (fp, "\tMOV RBP, RSP\n") ;
	fprintf (fp, "\tMOV RDI, declPrint\n") ;
	fprintf (fp, "\tXOR RSI, RSI\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
	fprintf (fp, "\tMOV EDI, 0\n") ;
	fprintf (fp, "\tcall exit\n") ;

	fprintf (fp, "declNegERROR:\n") ;
	fprintf (fp, "\tPUSH RBP\n") ;
	fprintf (fp, "\tMOV RBP, RSP\n") ;
	fprintf (fp, "\tMOV RDI, declNeg\n") ;
	fprintf (fp, "\tXOR RSI, RSI\n") ;
	fprintf (fp, "\tXOR RAX, RAX\n") ;
	fprintf (fp, "\tCALL printf\n") ;
	fprintf (fp, "\tMOV EDI, 0\n") ;
	fprintf (fp, "\tcall exit\n") ;


	fprintf (fp, "main:\n") ;
	fprintf (fp, "\tPUSH RBP\n") ;
	fprintf (fp, "\tPUSH RBP\n") ;
	fprintf (fp, "\tMOV RBP, RSP\n") ;
}

