#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"


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
	if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_NUM)
	{
	 	printf ("\tPUSH AX\n");
	 	if(node->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n", node->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n", atoi(node->child->tok->lexeme));

	 	printf ("\tCMP AX,%d\n", atoi(vst->arrayIndices->tokRight->lexeme));

	 	int l1 = get_label();
	 	int l2 = get_label();
	 	printf ("\tJG LABEL%d\n", l1);//It will give error
	 	printf ("\tCMP AX,%d\n", atoi(vst->arrayIndices->tokLeft->lexeme));
	 	printf ("\tJL LABEL%d\n", l1);
	 	printf ("\tJMP LABEL%d\n", l2);
	 	printf ("\tLABEL%d:\n", l1);

	 	printf ("\tcall exit\n");
	 	printf ("\tLABEL%d:\n", l2);
	 	printf ("\tPOP AX\n");
	}
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_NUM) // Left bound is variable
	{
   		int l1=get_label();
	 	int l2=get_label();

   		printf ("\tPUSH AX\n");

	 	if(node->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(node->child->tok->lexeme));

	 	printf ("\tCMP AX,%d\n",atoi(vst->arrayIndices->tokRight->lexeme));
	 	printf ("\tJG LABEL%d\n",l1);//It will give error
	 	printf ("\tPUSH BX\n");
	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJL LABEL%d\n",l1);
	 	printf ("\tJMP LABEL%d\n",l2);
	 	printf ("\tLABEL%d:\n",l1);
	 	printf ("\tcall exit\n");
	 	printf ("\tLABEL%d:\n",l2);
	 	printf ("\tPOP BX\n");
	 	printf ("\tPOP AX\n");
	}
	else if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_ID) // Right bound is variable   
	{
   		int l1=get_label();
	 	int l2=get_label();
   		printf ("\tPUSH AX\n");
   		printf ("\tPUSH BX\n");

   		if(node->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(node->child->tok->lexeme));	

	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJG LABEL%d\n",l1);//It will give error
	 	printf ("\tCMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
	 	printf ("\tJL LABEL%d\n",l1);// It will give an error
	 	printf ("\tJMP LABEL%d\n",l2);
	 	printf ("\tLABEL%d:\n",l1);
	 	printf ("\tcall exit\n");
	 	printf ("\tLABEL%d:\n",l2);
	 	printf ("\tPOP BX\n");
	 	printf ("\tPOP AX\n"); 
	}
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_ID) // Both bounds are variable    		
    {
   		int l1=get_label();
	 	int l2=get_label();
   		printf ("\tPUSH AX\n");
   		printf ("\tPUSH BX\n");

	 	if(node->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(node->child->tok->lexeme));	

	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJG LABEL%d\n",l1);//It will give error
	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJL LABEL%d\n",l1);// It will give an error
	 	printf ("\tJMP LABEL%d\n",l2);
	 	printf ("\tLABEL%d:\n",l1);
	 	printf ("\tcall exit\n");
	 	printf ("\tLABEL%d:\n",l2);
	 	printf ("\tPOP BX\n");
	 	printf ("\tPOP AX\n");
   }
}

int moduleGeneration (astNode *node, int rbpDepth, int rspDepth, moduleST *lst, varST *vst, FILE *fp)
{

	if (node == NULL)
		return 0 ;

	switch (node->id)
	{
		int start_label, end_label ;

		case statements :
			moduleGeneration (node->child, rbpDepth, rspDepth, node->localST, vst, fp);		// Access local scope and move below
			break ;

		case statement :
			rspDepth = moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			if (node->next == NULL)
			{
				if (rspDepth - rbpDepth > 0)
					printf ("\tADD RSP, %d\t\t\t;Restoring RSP to previous scope\n", (rspDepth-rbpDepth)) ;
				if (lst->parent == realBase)
				{
					printf ("\tPOP RBP\n") ;
					printf ("\tret\n") ;
				}
			}
			else
				moduleGeneration(node->next, rbpDepth, rspDepth, lst, vst, fp);
			break ;

		case TK_DECLARE :
			;
			int endOffset ;
			astNode *idListHead = node->next->child ;
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			endOffset = searchVar (realBase, lst, idListHead->tok->lexeme)->offset ;
			printf ("\tSUB RSP, %d\t\t\t;Updating RSP\n", (endOffset - rspDepth)) ;
			rspDepth = endOffset ;
			
			break ;											

		case TK_ASSIGNOP :
			moduleGeneration(node->child->next, rbpDepth, rspDepth,  lst, vst, fp);
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			printf ("\n") ;
			break ;

		case TK_PLUS :
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, rbpDepth, rspDepth,  lst, vst, fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tADD AX,BX\n");
			printf ("\tPUSH AX\n");

			break ;

		case TK_MINUS :
			moduleGeneration(node->child->next, rbpDepth, rspDepth, lst, vst, fp);
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tSUB AX,BX\n");
			printf ("\tPUSH AX\n");

			break ;

		case TK_MUL :
			moduleGeneration(node->child->next, rbpDepth, rspDepth, lst, vst, fp);
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tIMUL BX\n");
			printf ("\tPUSH AX\n");

			break ;

		case TK_DIV :
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, rbpDepth, rspDepth, lst, vst, fp);
			printf ("\tPOP BX\n");
			printf ("\tPOP AX\n\n");

			printf ("\tIDIV BX\n");
			printf ("\tPUSH AX\n");
			break ;

		case TK_RNUM :
			printf ("NO RNUM ALLOWED!\n") ;
			exit (0) ;
			break ;

		case TK_NUM : 
			if (node->prev == NULL)
			{
				printf ("\tMOV AX, %s\n",node->tok->lexeme);
				printf ("\tPUSH AX\n");	
			}
			else
			{
				printf ("\tMOV BX, %s\n",node->tok->lexeme);
				printf ("\tPUSH BX\n");	
			}
			break ;

		case TK_TRUE :
			if (node->prev == NULL)
			{
				printf ("\tMOV AX,00000001H\n");
				printf ("\tPUSH AX\n");	
			}
			else
			{
				printf ("\tMOV BX,00000001H\n");
				printf ("\tPUSH BX\n");	
			}
			break ;

		case TK_FALSE :
			if (node->prev == NULL)
			{
				printf ("\tMOV AX,00000000H\n");
				printf ("\tPUSH AX\n");	
			}
			else
			{
				printf ("\tMOV BX,00000000H\n");
				printf ("\tPUSH BX\n");	
			}
			break ;


		case TK_ID :
			if (node->parent->id!=TK_ASSIGNOP)
			{
				if (node->prev == NULL)
				{
					if(node->id == TK_ID && node->child==NULL)
					{
						printf ("\tMOV AX, [rbp - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
						printf ("\tPUSH AX\n");	
					}

					else if(node->id == TK_ID && node->child!=NULL) // var:=A[i]+j;
					{
						varST *vst ;
						vst = searchVar (realBase, lst,node->tok->lexeme);
						int size;
						if(vst->arrayIndices->type == TK_BOOLEAN)
							size=1;
						else if(vst->arrayIndices->type == TK_INTEGER)
							size=2;
						else if(vst->arrayIndices->type == TK_REAL)
							size=4;

						boundCheck(node,vst,fp);

						if(node->child->id == TK_ID) // For dynamic index
						{
							printf ("\tMOV EAX,%s_cb\n",node->tok->lexeme);
							printf ("\tPUSH EBX\n");
							printf ("\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
							printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
							printf ("\tIMUL BX,%d\n",size);
							printf ("\tADD EBX,EAX\n");
							printf ("\tMOV AX,[EBX]\n");
							printf ("\tPOP EBX\n");
						}
						else
							printf ("\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (atoi(node->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
						
						printf ("\tPUSH AX\n");	
					}
				}
				else
				{
					if(node->id == TK_ID && node->child==NULL)
					{
						printf ("\tMOV BX, [rbp - %d]\n",searchVar(realBase, lst, node->tok->lexeme)->offset);
						printf ("\tPUSH BX\n");	
					}

					else if(node->id == TK_ID && node->child!=NULL) // var:=j+A[i];
					{
						varST *vst ;
						vst=searchVar (realBase, lst,node->tok->lexeme);
						int size;
						if(vst->arrayIndices->type == TK_BOOLEAN)
							size=1;
						else if(vst->arrayIndices->type == TK_INTEGER)
							size=2;
						else if(vst->arrayIndices->type == TK_REAL)
							size=4;

						boundCheck(node,vst,fp);

						if(node->child->id == TK_ID) // For dynamic index
						{
							printf ("\tMOV EBX,%s_cb\n",node->tok->lexeme);
							printf ("\tPUSH EAX\n");
							printf ("\tAND EAX,00000000H\n");
							printf ("\tMOV AX,[%s_cb]\n",node->child->tok->lexeme);
							printf ("\tSUB AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
							printf ("\tIMUL AX,%d\n",size);
							printf ("\tADD EAX,EBX\n");
							printf ("\tMOV BX,[EAX]\n");
							printf ("\tPOP EAX\n");
						}
						else
							printf ("\tMOV BX, [%s_cb+%d]\n",node->tok->lexeme, (atoi(node->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
							
						printf ("\tPUSH BX\n");	
					}
				}
			}
			else
			{
				if(node->id == TK_ID && node->child==NULL && node->prev!=NULL)
				{
					printf ("\tMOV AX, [rbp - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
					printf ("\tPUSH AX\n");	
				}
				else if(node->id == TK_ID && node->child!=NULL && node->prev!=NULL) //type checking required i:=A[j]
				{
					varST *vst ;
					vst = searchVar (realBase, lst,node->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=1;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;

					boundCheck(node,vst,fp);	
					if(node->child->id == TK_ID)
					{
						printf ("\tPUSH AX\n");
						printf ("\tMOV EAX,%s_cb\n",node->tok->lexeme);
						printf ("\tPUSH EBX\n");
						printf ("\tAND EBX,00000000H\n");
						printf ("\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
						printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf ("\tIMUL BX,%d\n",size);
						printf ("\tADD EBX,EAX\n");
						printf ("\tMOV AX,[EBX]\n");
						printf ("\tPOP EBX\n");
					}
					else
						printf ("\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (atoi(node->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					
					printf ("\tPUSH AX\n");	
					
				}
				else if(node->id == TK_ID && node->child==NULL && node->prev==NULL)
				{
					printf ("\tPOP AX\n");
					printf ("\tMOV [rbp - %d],AX\t\t;Store\n", searchVar(realBase, lst, node->tok->lexeme)->offset);	
				}
				else if(node->id == TK_ID && node->child!=NULL && node->prev==NULL) // A[i]:=j
				{
					printf ("\tPOP AX\n");	

					varST* vst ;
					vst = searchVar(realBase, lst,node->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=1;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;

					boundCheck(node,vst,fp);	
					if(node->child->id == TK_ID)
					{
						printf ("\tPUSH EBX\n");
						printf ("\tPUSH AX\n");
						printf ("\tMOV EAX,%s_cb\n",node->tok->lexeme);
						printf ("\tAND EBX,00000000H\n");
						printf ("\tMOV BX,[%s_cb]\n",node->child->tok->lexeme);
						printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf ("\tIMUL BX,%d\n",size);
						printf ("\tADD EBX,EAX\n");
						printf ("\tPOP AX\n");
						printf ("\tMOV [EBX],AX\n");
						printf ("\tPOP EBX\n");
					}
					else
						printf ("\tMOV [%s_cb+%d],AX \n",node->tok->lexeme, (atoi(node->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
				}
			}
			break ;
		
		case TK_FOR :
			node=node->next;

			moduleGeneration(node, rbpDepth, rspDepth, lst, vst, fp);		// ID
			moduleGeneration(node->next->next, rbpDepth, rspDepth, lst, vst, fp);	// Right lim

			printf ("\tMOV AX,%s\n", node->next->tok->lexeme);
			printf ("\tMOV [rbp - %d], AX\t\t;for loop lower lim\n" , searchVar(realBase, lst, node->tok->lexeme)->offset );


			start_label = get_label();
			end_label = get_label();
			printf ("\t\n\n\nLABEL%d:\n",start_label);

			printf ("\tPOP AX\t\t\t\t;Initial check\n"); //4
			printf ("\tPUSH AX\n");
			
			printf ("\tMOV BX, [rbp - %d]\n", searchVar(realBase, lst, node->tok->lexeme)->offset);//variable
			printf ("\tCMP BX,AX\n");
			printf ("\tJG LABEL%d\n\n",end_label);

			moduleGeneration(node->next->next->next, rspDepth, rspDepth, lst, vst, fp);		// Statements

			printf ("\n\tMOV BX, [rbp - %d]\t\t;Ending increment\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			printf ("\tINC BX\n");
			printf ("\tMOV [rbp - %d],BX\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
			printf ("\tJMP LABEL%d",start_label);
			printf ("\t\n\n\n\nLABEL%d:\n",end_label);
			printf ("\tPOP AX\n");
			printf ("\tPOP AX\n");
			printf ("\tMOV [rbp - %d],AX\n", searchVar(realBase, lst ,node->tok->lexeme)->offset);
			break ;

		
		case TK_WHILE :
			node=node->next;
			start_label = get_label();
			end_label =  get_label();
			printf ("\t\nLABEL%d:\n",start_label);

			moduleGeneration(node, rbpDepth, rspDepth, lst, vst, fp);	// expression

			printf ("\tPOP AX\n");
			printf ("\tCMP AX,00000001h\n");
			printf ("\tJNE LABEL%d\n",end_label);

			moduleGeneration(node->next, rspDepth, rspDepth, lst, vst, fp);		// statements

			printf ("\tJMP LABEL%d\n",start_label);
			printf ("\nLABEL%d:\n",end_label);

			break ;

		// else if(node->id == TK_LT || node->id == TK_GT || node->id == TK_LE ||node->id == TK_GE ||node->id == TK_NE ||node->id == TK_EQ)  //all correct
		case TK_LT : case TK_GT : case TK_LE : case TK_GE : case TK_NE : case TK_EQ :
			moduleGeneration(node->child, rbpDepth, rspDepth, lst, vst, fp);
			moduleGeneration(node->child->next, rbpDepth, rspDepth, lst, vst, fp);

			printf ("\tPOP BX\n");
			printf ("\tPOP AX\n");

			start_label=get_label();
			end_label=get_label();
			printf ("\tCMP AX,BX\n\n");

			switch (node->id)
			{
				case TK_LT :
					printf ("\tJL LABEL%d\n",start_label);
					break ;
				case TK_GT :
					printf ("\tJG LABEL%d\n",start_label);
					break ;
				case TK_LE :
					printf ("\tJLE LABEL%d\n",start_label);
					break ;
				case TK_GE :
					printf ("\tJGE KAR%d\n",start_label);
					break ;
				case TK_NE :
					printf ("\tJNE KAR%d\n",start_label);
					break ;
				case TK_EQ :
					printf ("\tJE KAR%d\n",start_label);
					break ;
			}


			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end_label);
			printf ("LABEL%d:\n\n",start_label);
			printf ("\tMOV AX,1\n");
			printf ("\tPUSH AX\n\n\n");
			printf ("LABEL%d:\n\n",end_label);

			break ;

		case TK_AND : case TK_OR :
			int start,mid,end;
			codeGeneration(node->child,fp);
			codeGeneration(node->child->next,fp);

			start_label = get_label();
			int mid_label = get_label();
			end_label = get_label();

			printf ("\tPOP BX\n");
			printf ("\tPOP AX\n"); 

			printf ("\tCMP AX,00000001h\n");
			if (node->id == TK_AND)
			{
				printf ("\tJE LABEL%d\n",start_label);
				printf ("\tMOV AX,00000000h\n");
			}
			else
			{
				printf ("\tJNE LABEL%d\n",start_label);
				printf ("\tMOV AX,00000001h\n");
			}

			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n",end_label);


			printf ("\t\n\nLABEL%d:\n",start_label);
			printf ("\tCMP BX,00000001h\n");
			printf ("\tJNE LABEL%d\n",mid_label);
			printf ("\tMOV AX,00000001h\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n",end_label);
			printf ("LABEL%d:\n",mid_label);
			printf ("\tMOV AX,00000000h\n");
			printf ("\tPUSH AX\n");
			printf ("LABEL%d:\n",end_label);

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
			codeGeneration (node->child, fp) ;				// Do module definitions
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
		if(vst->datatype == TK_INTEGER || vst->datatype == TK_REAL )
		{
			codeGeneration(node,fp);
			printf ("\tPOP AX\n");
			printf ("\tcall outPut\n");
			printf ("\tcall printInt\n");
		}
		//check for boolean using ST			
		else if(vst->datatype == TK_BOOLEAN)
		{
			int start=get_label();
			int end=get_label();
			codeGeneration(node,fp);
			printf ("\tPOP AX\n");
			printf ("\tCMP AX,00000000h\n");
			printf ( "JNE LABEL%d\n",start);
			printf ("\tcall outPut\n");
			printf ("\tcall faLSE\n");
			printf ( "JMP LABEL%d\n",end);
			printf ( "\nLABEL%d:\n",start);
			printf ("\tcall outPut\n");
			printf ("\tcall trUE\n");
			printf ( "\nLABEL%d:\n",end);
		}

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
			else if(vst->arrayIndices->type == TK_REAL)
			{
				size=4;
				//strcpy(reg,"R9");
			}

			if(vst->offset == 0)     //static array
			{
				int lb=atoi(vst->arrayIndices->tokLeft->lexeme);
				int ub=atoi(vst->arrayIndices->tokRight->lexeme);
				printf ("\tcall outPut\n");
				if(vst->arrayIndices->type == TK_BOOLEAN)  //static array of type boolean
				{
					for(int i=lb;i<=ub;i++)
					{	
						int start=get_label();
						int end=get_label();
						printf ("\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (i-lb)*size);
						printf ("\tPUSH AX\n");
						printf ("\tPOP AX\n");
						printf ("\tCMP AX,00000000h\n");
						printf ( "JNE LABEL%d\n",start);
						//printf ("\tcall outPut\n");
						printf ("\tcall faLSE\n");
						printf ( "JMP LABEL%d\n",end);
						printf ( "\nLABEL%d:\n",start);
						//printf ("\tcall outPut\n");
						printf ("\tcall trUE\n");
						printf ( "\nLABEL%d:\n",end);
					}
				}


				else
				{
					for(int i=lb;i<=ub;i++) //static array of type num
					{
						printf ("\tMOV AX, [%s_cb+%d]\n",node->tok->lexeme, (i-lb)*size);
						printf ("\tPUSH AX\n");
						printf ("\tPOP AX\n");
						printf ("\tcall printInt\n");
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
			printf ("\tcall inPut\n");
			printf ("\tcall readInt\n");
			printf ("\tMOV [%s_cb],EAX\n",node->tok->lexeme);
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
		printf ("global main\n") ;
		printf ("extern printf\n\n") ;
		printf ("section .text\n") ;

		printf ("main:\n") ;
		printf ("\tPUSH RBP\n") ;
		printf ("\tMOV RBP, RSP\n") ;
		codeGeneration (astRoot, NULL) ;
	}
	else
		printf ("Semantic errors found. Please check the above messages\n") ;
	return 0;
}