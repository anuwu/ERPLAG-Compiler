#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"
#include "typeChecker.h"



int x = 0 ;
int get_label()
{
	x++;
	return x;
}

baseST *realBase ;


void boundCheck(astNode* node, varST *vst, FILE* fp)
{
	// Case 1 : Static array
	/*
	if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_NUM)
	{
	 	fprintf (fp, "\tPUSH AX\n");
	 	if(node->child->id == TK_ID) // variable index
	 		fprintf (fp, "\tMOV AX,[%s_cb]\n", node->child->tok->lexeme);
	 	else //static index
	 		fprintf (fp, "\tMOV AX,%d\n", atoi(node->child->tok->lexeme));

	 	fprintf (fp, "\tCMP AX,%d\n", atoi(vst->arrayIndices->tokRight->lexeme));

	 	int l1 = get_label();
	 	int l2 = get_label();
	 	fprintf (fp, "\tJG LABEL%d\n", l1);//It will give error
	 	fprintf (fp, "\tCMP AX,%d\n", atoi(vst->arrayIndices->tokLeft->lexeme));
	 	fprintf (fp, "\tJL LABEL%d\n", l1);
	 	fprintf (fp, "\tJMP LABEL%d\n", l2);
	 	fprintf (fp, "\tLABEL%d:\n", l1);

	 	fprintf (fp, "\tcall exit\n");
	 	fprintf (fp, "\tLABEL%d:\n", l2);
	 	fprintf (fp, "\tPOP AX\n");
	}
	*/
	if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_NUM) // Left bound is variable
	{
   		int l1=get_label();
	 	int l2=get_label();

   		fprintf (fp, "\tPUSH AX\n");

	 	if(node->child->id == TK_ID) // variable index
	 		fprintf (fp, "\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		fprintf (fp, "\tMOV AX,%d\n",atoi(node->child->tok->lexeme));

	 	fprintf (fp, "\tCMP AX,%d\n",atoi(vst->arrayIndices->tokRight->lexeme));
	 	fprintf (fp, "\tJG LABEL%d\n",l1);//It will give error
	 	fprintf (fp, "\tPUSH BX\n");
	 	fprintf (fp, "\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	fprintf (fp, "\tCMP AX,BX\n");
	 	fprintf (fp, "\tJL LABEL%d\n",l1);
	 	fprintf (fp, "\tJMP LABEL%d\n",l2);
	 	fprintf (fp, "\tLABEL%d:\n",l1);
	 	fprintf (fp, "\tcall exit\n");
	 	fprintf (fp, "\tLABEL%d:\n",l2);
	 	fprintf (fp, "\tPOP BX\n");
	 	fprintf (fp, "\tPOP AX\n");
	}
	else if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_ID) // Right bound is variable   
	{
   		int l1=get_label();
	 	int l2=get_label();
   		fprintf (fp, "\tPUSH AX\n");
   		fprintf (fp, "\tPUSH BX\n");

   		if(node->child->id == TK_ID) // variable index
	 		fprintf (fp, "\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		fprintf (fp, "\tMOV AX,%d\n",atoi(node->child->tok->lexeme));	

	 	fprintf (fp, "\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	fprintf (fp, "\tCMP AX,BX\n");
	 	fprintf (fp, "\tJG LABEL%d\n",l1);//It will give error
	 	fprintf (fp, "\tCMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
	 	fprintf (fp, "\tJL LABEL%d\n",l1);// It will give an error
	 	fprintf (fp, "\tJMP LABEL%d\n",l2);
	 	fprintf (fp, "\tLABEL%d:\n",l1);
	 	fprintf (fp, "\tcall exit\n");
	 	fprintf (fp, "\tLABEL%d:\n",l2);
	 	fprintf (fp, "\tPOP BX\n");
	 	fprintf (fp, "\tPOP AX\n"); 
	}
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_ID) // Both bounds are variable    		
    {
   		int l1=get_label();
	 	int l2=get_label();
   		fprintf (fp, "\tPUSH AX\n");
   		fprintf (fp, "\tPUSH BX\n");

	 	if(node->child->id == TK_ID) // variable index
	 		fprintf (fp, "\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		fprintf (fp, "\tMOV AX,%d\n",atoi(node->child->tok->lexeme));	

	 	fprintf (fp, "\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	fprintf (fp, "\tCMP AX,BX\n");
	 	fprintf (fp, "\tJG LABEL%d\n",l1);//It will give error
	 	fprintf (fp, "\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	fprintf (fp, "\tCMP AX,BX\n");
	 	fprintf (fp, "\tJL LABEL%d\n",l1);// It will give an error
	 	fprintf (fp, "\tJMP LABEL%d\n",l2);
	 	fprintf (fp, "\tLABEL%d:\n",l1);
	 	fprintf (fp, "\tcall exit\n");
	 	fprintf (fp, "\tLABEL%d:\n",l2);
	 	fprintf (fp, "\tPOP BX\n");
	 	fprintf (fp, "\tPOP AX\n");
   }
}


int getStaticOffset (varST *vst, astNode *node, int size)
{
	return vst->offset - size*(atoi(vst->arrayIndices->tokRight->lexeme) - atoi(node->child->tok->lexeme)) ;
}

void printGetValueArrayPrompt (tokenID baseType , int leftLim, int rightLim, FILE* fp)
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


int moduleGeneration (astNode *node, int localBase, int rspDepth, moduleST *lst, varST *vst, FILE *fp)
{
	if (node == NULL)
		return 0 ;

	switch (node->id)
	{
		int start_label, end_label ;
		int reserveLabel[10] ;
		varST *searchedVar ;

		case statements :
			moduleGeneration (node->child, localBase, rspDepth, node->localST, vst, fp);		// Access local scope and move below
			break ;

		case statement :
			rspDepth = moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			if (node->next == NULL)
			{
				if (lst->parent == realBase)
				{
					fprintf (fp, "\tMOV RSP, RBP\n") ;
					fprintf (fp, "\tPOP RBP\n") ;
					fprintf (fp, "\tPOP RBP\n") ;
					fprintf (fp, "\tret\n") ;
				}
				else if (rspDepth - localBase > 0)
					fprintf (fp, "\tADD RSP, %d\t\t\t;Restoring RSP to previous scope\n", (rspDepth-localBase)) ;
			}
			else
				moduleGeneration(node->next, localBase, rspDepth, lst, vst, fp);
			break ;

		case TK_DECLARE :
			;
			int endOffset ;
			astNode *idListHead = node->next->child ;
			astNode *dtNode = node->next->next ;
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			endOffset = searchVar (realBase, lst, idListHead->tok->lexeme)->offset ;

			if (endOffset > 0)
			{		
				fprintf (fp, "\tSUB RSP, %d\t\t\t;Updating RSP\n\n", (endOffset - rspDepth)) ;
				rspDepth = endOffset ;
			}
			
			break ;											

		case TK_ASSIGNOP :
			moduleGeneration(node->child->next, localBase, rspDepth,  lst, vst, fp);
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			fprintf (fp, "\n") ;
			break ;

		case TK_PLUS :
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, localBase, rspDepth,  lst, vst, fp);
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tADD AX,BX\n");
			fprintf (fp, "\tPUSH AX\n");

			break ;

		case TK_MINUS :
			moduleGeneration(node->child->next, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tSUB AX,BX\n");
			fprintf (fp, "\tPUSH AX\n");

			break ;

		case TK_MUL :
			moduleGeneration(node->child->next, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tIMUL BX\n");
			fprintf (fp, "\tPUSH AX\n");

			break ;

		case TK_DIV :
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, localBase, rspDepth, lst, vst, fp);
			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tPOP AX\n\n");

			fprintf (fp, "\tIDIV BX\n");
			fprintf (fp, "\tPUSH AX\n");
			break ;

		case TK_RNUM :
			fprintf (fp, "NO RNUM ALLOWED!\n") ;
			exit (0) ;
			break ;

		case TK_NUM : 
			if (node->prev == NULL)
			{
				fprintf (fp, "\tMOV AX, %s\n",node->tok->lexeme);
				fprintf (fp, "\tPUSH AX\n");	
			}
			else
			{
				fprintf (fp, "\tMOV BX, %s\n",node->tok->lexeme);
				fprintf (fp, "\tPUSH BX\n");	
			}
			break ;

		case TK_TRUE :
			if (node->prev == NULL)
			{
				fprintf (fp, "\tMOV AX,00000001H\n");
				fprintf (fp, "\tPUSH AX\n");	
			}
			else
			{
				fprintf (fp, "\tMOV BX,00000001H\n");
				fprintf (fp, "\tPUSH BX\n");	
			}
			break ;

		case TK_FALSE :
			if (node->prev == NULL)
			{
				fprintf (fp, "\tMOV AX,00000000H\n");
				fprintf (fp, "\tPUSH AX\n");	
			}
			else
			{
				fprintf (fp, "\tMOV BX,00000000H\n");
				fprintf (fp, "\tPUSH BX\n");	
			}
			break ;


		case TK_ID :
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
						int size;
						if(vst->arrayIndices->type == TK_BOOLEAN)
							size=2;
						else if(vst->arrayIndices->type == TK_INTEGER)
							size=2;
						else if(vst->arrayIndices->type == TK_REAL)
							size=4;

						if (vst->offset)
							boundCheck(node,vst,fp);

						if(node->child->id == TK_ID) // For dynamic index
						{
							fprintf (fp, "\tMOV EAX,%s_cb\n",node->tok->lexeme);
							fprintf (fp, "\tPUSH EBX\n");
							fprintf (fp, "\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
							fprintf (fp, "\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
							fprintf (fp, "\tIMUL BX,%d\n",size);
							fprintf (fp, "\tADD EBX,EAX\n");
							fprintf (fp, "\tMOV AX,[EBX]\n");
							fprintf (fp, "\tPOP EBX\n");
						}
						else
							fprintf (fp, "\tMOV AX, [RBP-%d]\n", getStaticOffset (vst, node, size)) ;
						
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
						int size;
						if(vst->arrayIndices->type == TK_BOOLEAN)
							size=2;
						else if(vst->arrayIndices->type == TK_INTEGER)
							size=2;
						else if(vst->arrayIndices->type == TK_REAL)
							size=4;

						if (vst->offset > 0)
							boundCheck(node,vst,fp);

						if(node->child->id == TK_ID) // For dynamic index
						{
							fprintf (fp, "\tMOV EBX,%s_cb\n",node->tok->lexeme);
							fprintf (fp, "\tPUSH EAX\n");
							fprintf (fp, "\tAND EAX,00000000H\n");
							fprintf (fp, "\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
							fprintf (fp, "\tSUB AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
							fprintf (fp, "\tIMUL AX,%d\n",size);
							fprintf (fp, "\tADD EAX,EBX\n");
							fprintf (fp, "\tMOV BX,[EAX]\n");
							fprintf (fp, "\tPOP EAX\n");
						}
						else
							fprintf (fp, "\tMOV BX, [RBP - %d]\n",getStaticOffset(vst,node,size)) ;
							
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
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=2;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;

					if (vst->offset > 0)
						boundCheck(node,vst,fp);	
					if(node->child->id == TK_ID)
					{
						fprintf (fp, "\tPUSH AX\n");
						fprintf (fp, "\tMOV EAX,%s_cb\n",node->tok->lexeme);
						fprintf (fp, "\tPUSH EBX\n");
						fprintf (fp, "\tAND EBX,00000000H\n");
						fprintf (fp, "\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
						fprintf (fp, "\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf (fp, "\tIMUL BX,%d\n",size);
						fprintf (fp, "\tADD EBX,EAX\n");
						fprintf (fp, "\tMOV AX,[EBX]\n");
						fprintf (fp, "\tPOP EBX\n");
					}
					else
						fprintf (fp, "\tMOV AX, [RBP - %d]\n", getStaticOffset(vst, node, size)) ;
					
					fprintf (fp, "\tPUSH AX\n");	
					
				}
				else if(node->id == TK_ID && node->child==NULL && node->prev==NULL)
				{
					fprintf (fp, "\tPOP AX\n");
					fprintf (fp, "\tMOV [rbp - %d],AX\t\t;Store\n", searchVar(realBase, lst, node->tok->lexeme)->offset);	
				}
				else if(node->id == TK_ID && node->child!=NULL && node->prev==NULL) // A[i]:=j
				{
					fprintf (fp, "\tPOP AX\n");	

					varST* vst ;
					vst = searchVar(realBase, lst,node->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=2;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;

					if (vst->offset < 0)
						boundCheck(node,vst,fp);	
					if(node->child->id == TK_ID)
					{
						fprintf (fp, "\tPUSH EBX\n");
						fprintf (fp, "\tPUSH AX\n");
						fprintf (fp, "\tMOV EAX,%s_cb\n",node->tok->lexeme);
						fprintf (fp, "\tAND EBX,00000000H\n");
						fprintf (fp, "\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
						fprintf (fp, "\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf (fp, "\tIMUL BX,%d\n",size);
						fprintf (fp, "\tADD EBX,EAX\n");
						fprintf (fp, "\tPOP AX\n");
						fprintf (fp, "\tMOV [EBX],AX\n");
						fprintf (fp, "\tPOP EBX\n");
					}
					else
						fprintf (fp, "\tMOV [RBP - %d], AX\t\t;Store\n" , getStaticOffset(vst,node,size)) ;
				}
			}
			break ;
		
		case TK_FOR :
			node=node->next;

			moduleGeneration(node, localBase, rspDepth, lst, vst, fp);		// ID
			moduleGeneration(node->next->next, localBase, rspDepth, lst, vst, fp);	// Right lim

			fprintf (fp, "\tMOV AX,%s\n", node->next->tok->lexeme);
			fprintf (fp, "\tMOV [RBP - %d], AX\t\t;for loop lower lim\n" , searchVar(realBase, lst, node->tok->lexeme)->offset);

			start_label = get_label();
			end_label = get_label();
			fprintf (fp, "\t\n\n\nLABEL%d:\n",start_label);

			fprintf (fp, "\tPOP AX\t\t\t\t;Initial check\n"); //4
			fprintf (fp, "\tPUSH AX\n");
			
			fprintf (fp, "\tMOV BX, [RBP - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);//variable
			fprintf (fp, "\tCMP BX,AX\n");
			fprintf (fp, "\tJG LABEL%d\n\n",end_label);

			moduleGeneration(node->next->next->next, rspDepth, rspDepth, lst, vst, fp);		// Statements

			fprintf (fp, "\n\tMOV BX, [RBP - %d]\t\t;Ending increment\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			fprintf (fp, "\tINC BX\n");
			fprintf (fp, "\tMOV [rbp - %d],BX\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			fprintf (fp, "\tJMP LABEL%d",start_label);
			fprintf (fp, "\t\n\n\n\nLABEL%d:\n",end_label);
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tMOV [RBP - %d],AX\n", searchVar(realBase, lst ,node->tok->lexeme)->offset);
			break ;

		
		case TK_WHILE :
			node=node->next;
			start_label = get_label();
			end_label =  get_label();
			fprintf (fp, "\t\nLABEL%d:\n",start_label);

			moduleGeneration(node, localBase, rspDepth, lst, vst, fp);	// expression

			fprintf (fp, "\tPOP AX\n");
			fprintf (fp, "\tCMP AX,00000001h\n");
			fprintf (fp, "\tJNE LABEL%d\n",end_label);

			moduleGeneration(node->next, rspDepth, rspDepth, lst, vst, fp);		// statements

			fprintf (fp, "\tJMP LABEL%d\n",start_label);
			fprintf (fp, "\nLABEL%d:\n",end_label);

			break ;

		// else if(node->id == TK_LT || node->id == TK_GT || node->id == TK_LE ||node->id == TK_GE ||node->id == TK_NE ||node->id == TK_EQ)  //all correct
		case TK_LT : case TK_GT : case TK_LE : case TK_GE : case TK_NE : case TK_EQ :
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, localBase, rspDepth, lst, vst, fp);

			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tPOP AX\n");

			start_label=get_label();
			end_label=get_label();
			fprintf (fp, "\tCMP AX,BX\n\n");

			switch (node->id)
			{
				case TK_LT :
					fprintf (fp, "\tJL LABEL%d\n",start_label);
					break ;
				case TK_GT :
					fprintf (fp, "\tJG LABEL%d\n",start_label);
					break ;
				case TK_LE :
					fprintf (fp, "\tJLE LABEL%d\n",start_label);
					break ;
				case TK_GE :
					fprintf (fp, "\tJGE LABEL%d\n",start_label);
					break ;
				case TK_NE :
					fprintf (fp, "\tJNE LABEL%d\n",start_label);
					break ;
				case TK_EQ :
					fprintf (fp, "\tJE LABEL%d\n",start_label);
					break ;
			}


			fprintf (fp, "\tMOV AX,0\n");
			fprintf (fp, "\tPUSH AX\n");
			fprintf (fp, "\tJMP LABEL%d\n\n",end_label);
			fprintf (fp, "LABEL%d:\n\n",start_label);
			fprintf (fp, "\tMOV AX,1\n");
			fprintf (fp, "\tPUSH AX\n\n\n");
			fprintf (fp, "LABEL%d:\n\n",end_label);

			break ;

		case TK_AND : case TK_OR :
			moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, localBase, rspDepth, lst, vst, fp);

			start_label = get_label();
			int mid_label = get_label();
			end_label = get_label();

			fprintf (fp, "\tPOP BX\n");
			fprintf (fp, "\tPOP AX\n"); 

			fprintf (fp, "\tCMP AX,00000001h\n");
			if (node->id == TK_AND)
			{
				fprintf (fp, "\tJE LABEL%d\n",start_label);
				fprintf (fp, "\tMOV AX,00000000h\n");
			}
			else
			{
				fprintf (fp, "\tJNE LABEL%d\n",start_label);
				fprintf (fp, "\tMOV AX,00000001h\n");
			}

			fprintf (fp, "\tPUSH AX\n");
			fprintf (fp, "\tJMP LABEL%d\n",end_label);


			fprintf (fp, "\t\n\nLABEL%d:\n",start_label);
			fprintf (fp, "\tCMP BX,00000001h\n");
			fprintf (fp, "\tJNE LABEL%d\n",mid_label);
			fprintf (fp, "\tMOV AX,00000001h\n");
			fprintf (fp, "\tPUSH AX\n");
			fprintf (fp, "\tJMP LABEL%d\n",end_label);
			fprintf (fp, "LABEL%d:\n",mid_label);
			fprintf (fp, "\tMOV AX,00000000h\n");
			fprintf (fp, "\tPUSH AX\n");
			fprintf (fp, "LABEL%d:\n",end_label);

			break ;

		case TK_PRINT :
			searchedVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
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
					if (node->next->child != NULL && node->next->child->id == TK_NUM)	// Static array, static index
					{
						if (searchedVar->arrayIndices->type == TK_INTEGER)
						{
							fprintf (fp, "\tMOV AX, [RBP - %d]\t\t;printing integer\n", getStaticOffset(searchedVar,node->next,2)) ;
							fprintf (fp, "\tMOV RDI, printFormat\n") ;
							fprintf (fp, "\tMOVSX RSI, AX\n") ;
							fprintf (fp, "\tXOR RAX, RAX\n") ;
							fprintf (fp, "\tCALL printf\n") ;
						}
					}	
					else if (node->next->child == NULL)
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
			}
			break ;

		case TK_GET_VALUE :
			searchedVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
			int rspAlign ;
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
				//fprintf (fp, "\tSUB RSI, 4\n") ;
				fprintf (fp, "\tPUSH RSI\n") ;
				fprintf (fp, "\tCALL scanf\n") ;
				fprintf (fp, "\tPOP RSI\n") ;
				fprintf (fp, "\tMOV AX, WORD [RSP]\n") ;
				fprintf (fp, "\tMOV [RBP - %d], AX\n", searchedVar->offset) ;
				fprintf (fp, "\tADD RSP, %d\n\n", rspAlign) ;
			}
			else // Array type
			{	
				if (isVarStaticArr (searchedVar))
				{
					int leftLim, rightLim ;
					leftLim  = atoi (searchedVar->arrayIndices->tokLeft->lexeme) ;
					rightLim = atoi (searchedVar->arrayIndices->tokRight->lexeme) ;

					printGetValueArrayPrompt (searchedVar->arrayIndices->type, leftLim, rightLim, fp) ;
					reserveLabel[0] = get_label () ;

					rspAlign = 32 - (rspDepth % 16) ;


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
			}
			break ;

	}

	return rspDepth ;
}


void codeGeneration(astNode *node, FILE* fp)
{
	if (node == NULL)
		return;

	switch (node->id)
	{
		case program :
			codeGeneration (node->child->next, fp) ;		// <otherModules>
			break ;

		case otherModules :
			codeGeneration (node->child, fp) ;					// Do module definitions
			codeGeneration (node->next, fp) ;					// <driverModule> or NULL
			break ;

		case driverModule :
			moduleGeneration(node->child, 0, 0, NULL, NULL, fp); 				// <statements>
			codeGeneration(node->next, fp); 				// Move to the next child of program
			break ;
	}

	/*

	else if(node->id == TK_PRINT) //DYNAMIC ARRAYS IS LEFT
	{
		node=node->next;
		vst=searchVarInCurrentModule (lst,node->tok->lexeme);

		//check for boolean using ST

		else if(vst->datatype == TK_ARRAY)
		{
			
			int size;
			if(vst->arrayIndices->type == TK_BOOLEAN)
			{
				size=1;
			//strcpy(reg,"R9B");
			}
			else if(vst->arrayIndices->type == TK_INTEGER)
			{
				size=2;
				//strcpy(reg,"R9W");
			}

			if(vst->offset == 0)     //static array
			{
				int lb=atoi(vst->arrayIndices->tokLeft->lexeme);
				int ub=atoi(vst->arrayIndices->tokRight->lexeme);
				fprintf (fp, "\tcall outPut\n");
				if(vst->arrayIndices->type == TK_BOOLEAN)  //static array of type boolean
				{
					for(int i=lb;i<=ub;i++)
					{	
						int start=get_label();
						int end=get_label();
						fprintf (fp, "\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (i-lb)*size);
						fprintf (fp, "\tPUSH AX\n");
						fprintf (fp, "\tPOP AX\n");
						fprintf (fp, "\tCMP AX,00000000h\n");
						fprintf (fp,  "JNE LABEL%d\n",start);
						//fprintf (fp, "\tcall outPut\n");
						fprintf (fp, "\tcall faLSE\n");
						fprintf (fp,  "JMP LABEL%d\n",end);
						fprintf (fp,  "\nLABEL%d:\n",start);
						//fprintf (fp, "\tcall outPut\n");
						fprintf (fp, "\tcall trUE\n");
						fprintf (fp,  "\nLABEL%d:\n",end);
					}
				}


				else
				{
					for(int i=lb;i<=ub;i++) //static array of type num
					{
						fprintf (fp, "\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (i-lb)*size);
						fprintf (fp, "\tPUSH AX\n");
						fprintf (fp, "\tPOP AX\n");
						fprintf (fp, "\tcall printInt\n");
					}
				}
			}
			 
		}        
	}

	else if(node->id == TK_GET_VALUE) //BOOLEAN CHECK, ARRAY
	{
		node=node->next;
		if(node->child == NULL)//if not an array
		{
			fprintf (fp, "\tcall inPut\n");
			fprintf (fp, "\tcall readInt\n");
			fprintf (fp, "\tMOV [%s_cb],EAX\n",node->tok->lexeme);
		}
	}
	*/
}


int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		printf ("\tIncorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parseTree (argv[1]) ;
	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	realBase = fillSymbolTable(astRoot,0);

	if (argc == 3)
	{
		if (strcmp(argv[2], "-printAST") == 0)
			inorderAST (astRoot, 0) ;
		else
		{
			printf ("Incorrect command!\n") ;
			exit (0) ;
		}
	}

	printf ("\n----------------------------------------------\n") ;

	if (!realBase->semanticError)
	{
		FILE *fp = fopen ("code.asm", "w") ;

		fprintf (fp, "section .data\n") ;

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
		fprintf (fp, "extern printf\n\n") ;

		fprintf (fp, "section .text\n") ;

		fprintf (fp, "main:\n") ;
		fprintf (fp, "\tPUSH RBP\n") ;
		fprintf (fp, "\tPUSH RBP\n") ;
		fprintf (fp, "\tMOV RBP, RSP\n") ;
		codeGeneration (astRoot, fp) ;
	}
	else
		printf ("Semantic errors found. Please check the above messages\n") ;
	return 0;
}