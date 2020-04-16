#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"

moduleST* lst;
varST* vst;

int get_label()
{
	static int x=0;
	x++;
	return x;
}

void boundCheck(varST* vst, astNode* temp,FILE* fp)
	{
	// Case 1 : Static array
	if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_NUM)
	{
	 	printf ("\tPUSH AX\n");
	 	if(temp->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n", temp->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n", atoi(temp->child->tok->lexeme));

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
	 	if(temp->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(temp->child->tok->lexeme));

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
   		if(temp->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(temp->child->tok->lexeme));	
	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJG LABEL%d\n",l1);//It will give error
	 	printf ("\tCMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
	 	//printf ("\tCMP AX,BX\n");
	 	printf ("\tJL LABEL%d\n",l1);// It will give an error
	 	printf ("\tJMP LABEL%d\n",l2);
	 	printf ("\tLABEL%d:\n",l1);
	 	//printf ("\tcall error\n");
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
	 	if(temp->child->id == TK_ID) // variable index
	 		printf ("\tMOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf ("\tMOV AX,%d\n",atoi(temp->child->tok->lexeme));	
	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJG LABEL%d\n",l1);//It will give error
	 	printf ("\tMOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	printf ("\tCMP AX,BX\n");
	 	printf ("\tJL LABEL%d\n",l1);// It will give an error
	 	printf ("\tJMP LABEL%d\n",l2);
	 	printf ("\tLABEL%d:\n",l1);
	 	//printf ("\tcall error\n");
	 	printf ("\tcall exit\n");
	 	printf ("\tLABEL%d:\n",l2);
	 	printf ("\tPOP BX\n");
	 	printf ("\tPOP AX\n");
   }
}	   	 


void code_generation(astNode* astRoot, FILE* fp)
{
	astNode* temp = astRoot; // Initially stores the root node of the AST
	if(temp==NULL)
		return;


	if(temp->id <= idL)   //NON Terminals
	{
		if(temp->id == program)  // Encounters program as the root node
			code_generation(temp->child,fp);  // Moves to children of program
		else if(temp->id == moduleDeclarations )
			code_generation(temp->next,fp); // It hasn’t been accounted for yet

		else if(temp->id == otherModules )
			code_generation(temp->next,fp); // It hasn’t been accounted for yet	

		else if(temp->id == driverModule )
		{
			code_generation(temp->child,fp); //Move to first child of driver
			code_generation(temp->next,fp); // Move to the next child of program
		}
		else if(temp->id == statements)
		{	
			lst = temp->localST;
			code_generation(temp->child,fp);
			code_generation(temp->next,fp);
		}
		else if(temp->id == statement)
		{	
			
			code_generation(temp->child,fp);
			code_generation(temp->next,fp);
		}
	}
	else
	{
		if(temp->id == TK_FOR) //checked restoring original value of k
		{
			temp=temp->next;

			code_generation(temp,fp);
			code_generation(temp->next->next,fp);

			printf ("\tMOV AX,%s\n", temp->next->tok->lexeme);
			printf ("\tMOV [%s_cb], AX\n",temp->tok->lexeme );


			int start_label = get_label();
			int end_label = get_label();
			printf ("\t\n\n\nLABEL%d:\n",start_label);

			printf ("\tPOP AX\n"); //4
			printf ("\tPUSH AX\n");
			
			printf ("\tMOV BX, [%s_cb]\n",temp->tok->lexeme);//variable
			printf ("\tCMP BX,AX\n");
			printf ("\tJG LABEL%d\n",end_label);

			code_generation(temp->next->next->next,fp);

			printf ("\tMOV BX, [%s_cb]\n",temp->tok->lexeme);
			printf ("\tINC BX\n");
			printf ("\tMOV [%s_cb],BX\n",temp->tok->lexeme);
			printf ("\tJMP LABEL%d",start_label);
			printf ("\t\n\n\n\nLABEL%d:\n",end_label);
			printf ("\tPOP AX\n");
			printf ("\tPOP AX\n");
			printf ("\tMOV [%s_cb],AX\n",temp->tok->lexeme);

		}
		else if(temp->id == TK_WHILE)  //checked
		{
			temp=temp->next;
			int start=get_label();
			int end=  get_label();
			printf ("\t\nLABEL%d:\n",start);
			code_generation(temp,fp);
			printf ("\tPOP AX\n");
			printf ("\tCMP AX,00000001h\n");
			printf ("\tJNE LABEL%d\n",end);
			code_generation(temp->next,fp);
			printf ("\tJMP LABEL%d\n",start);
			printf ("\t\nLABEL%d:\n",end);

		}
		else if(temp->id == TK_PRINT) //DYNAMIC ARRAYS IS LEFT
		{
			temp=temp->next;
			vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
			if(vst->datatype == TK_INTEGER || vst->datatype == TK_REAL )
			{
				code_generation(temp,fp);
				printf ("\tPOP AX\n");
				printf ("\tcall outPut\n");
				printf ("\tcall printInt\n");
			}
			//check for boolean using ST			
			else if(vst->datatype == TK_BOOLEAN)
			{
				int start=get_label();
				int end=get_label();
				code_generation(temp,fp);
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
							printf ("\tMOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
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
							printf ("\tMOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
							printf ("\tPUSH AX\n");
							printf ("\tPOP AX\n");
							printf ("\tcall printInt\n");
						}
					}
				}
				 
			}        
		}

		else if(temp->id == TK_GET_VALUE) //BOOLEAN CHECK, ARRAY
		{
			temp=temp->next;
			if(temp->child == NULL)//if not an array
			{
				printf ("\tcall inPut\n");
				printf ("\tcall readInt\n");
				printf ("\tMOV [%s_cb],EAX\n",temp->tok->lexeme);
			}
		}

		else if(temp->id == TK_AND || temp->id == TK_OR) //checked
		{
			int start,mid,end;
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);

			start=get_label();
			mid=get_label();
			end=get_label();

			printf ("\tPOP BX\n");
			printf ("\tPOP AX\n"); 
			if(temp->id == TK_AND)
			{
				printf ("\tCMP AX,00000001h\n");
				printf ("\tJE LABEL%d\n",start);
				printf ("\tMOV AX,00000000h\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n",end);

				printf ("\t\n\nLABEL%d:\n",start);
				printf ("\tCMP BX,00000001h\n");
				printf ("\tJNE LABEL%d\n",mid);
				printf ("\tMOV AX,00000001h\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n",end);
				printf ("\t\n\nLABEL%d:\n",mid);
				printf ("\tMOV AX,00000000h\n");
				printf ("\tPUSH AX\n");
				printf ("\t\n\nLABEL%d:\n",end);

			}

			else
			{
				printf ("\tCMP AX,00000001h\n");
				printf ("\tJNE LABEL%d\n",start);
				printf ("\tMOV AX,00000001h\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n",end);

				printf ("\t\n\nLABEL%d:\n",start);
				printf ("\tCMP BX,00000001h\n");
				printf ("\tJNE LABEL%d\n",mid);
				printf ("\tMOV AX,00000001h\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n",end);
				printf ("\t\n\nLABEL%d:\n",mid);
				printf ("\tMOV AX,00000000h\n");
				printf ("\tPUSH AX\n");
				printf ("\t\n\nLABEL%d:\n",end);

			}
		}
		
		else if(temp->id == TK_LT || temp->id == TK_GT || temp->id == TK_LE ||temp->id == TK_GE ||temp->id == TK_NE ||temp->id == TK_EQ)  //all correct
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			printf ("\tPOP BX\n");
			printf ("\tPOP AX\n");
			int start=get_label();
			int end=get_label();
			printf ("\tCMP AX,BX\n\n");
			if(temp->id == TK_LT)
			{
				printf ("\tJL LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_LE)
			{
				printf ("\tJLE LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GT)
			{
				printf ("\tJG LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GE)
			{
				printf ("\tJGE LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				printf ("\tJE LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				printf ("\tJNE LABEL%d\n",start);
				printf ("\tMOV AX,0\n");
				printf ("\tPUSH AX\n");
				printf ("\tJMP LABEL%d\n\n",end);
			}

			printf ("\tLABEL%d:\n\n",start);
			printf ("\tMOV AX,1\n");
			printf ("\tPUSH AX\n\n\n");

			printf ("\tLABEL%d:\n\n",end);

		}

		else if(temp->id == TK_ASSIGNOP)
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
		}

		else if(temp->id == TK_PLUS )
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tADD AX,BX\n");
			printf ("\tPUSH AX\n");
		}

		else if(temp->id == TK_MINUS )
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tSUB AX,BX\n");
			printf ("\tPUSH AX\n");
		}

		else if(temp->id == TK_MUL) //working 
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			printf ("\tPOP AX\n");
			printf ("\tPOP BX\n");
			printf ("\tIMUL BX\n");
			printf ("\tPUSH AX\n");
		}

		else if(temp->id == TK_DIV) //working
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			printf ("\tPOP BX\n");
			printf ("\tPOP AX\nCWD\n");

			printf ("\tIDIV BX\n");
			printf ("\tPUSH AX\n");
		}

		else if(temp->id == TK_NUM || temp->id == TK_RNUM || (temp->id==TK_ID && temp->parent->id!=TK_ASSIGNOP) ||temp->id == TK_TRUE || temp->id==TK_FALSE) //Add, Sub, Mul,DIv
		{
			if(temp->prev == NULL) //AX
			{
				if(temp->id == TK_NUM || temp->id==TK_RNUM)
				{
					printf ("\tMOV AX, %s\n",temp->tok->lexeme);
					printf ("\tPUSH AX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						printf ("\tMOV AX,00000001H\n");
						printf ("\tPUSH AX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							printf ("\tMOV AX,00000000H\n");
							printf ("\tPUSH AX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					printf ("\tMOV AX, [%s_cb]\n",temp->tok->lexeme);
					printf ("\tPUSH AX\n");	
				}


				else if(temp->id == TK_ID && temp->child!=NULL) // var:=A[i]+j;
				{
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=1;
					else if(vst->arrayIndices->type == TK_INTEGER)
					{
						size=2;
						//strcpy(reg,"R9W");
					}
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;
					//printf ("\tStarting bound checking\n");
					boundCheck(vst,temp,fp);
					//printf ("\tEnded bound checking\n");
					if(temp->child->id == TK_ID) // For dynamic index
					{
						printf ("\tMOV EAX,%s_cb\n",temp->tok->lexeme);
						printf ("\tPUSH EBX\n");
						printf ("\tMOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf ("\tIMUL BX,%d\n",size);
						printf ("\tADD EBX,EAX\n");
						printf ("\tMOV AX,[EBX]\n");
						printf ("\tPOP EBX\n");
					}
					else
					printf ("\tMOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					printf ("\tPUSH AX\n");	
				}
			}

			else   //BX
			{
				if((temp->id == TK_NUM || temp->id==TK_RNUM))
				{
					printf ("\tMOV BX, %s\n",temp->tok->lexeme);
					printf ("\tPUSH BX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						printf ("\tMOV BX,00000001H\n");
						printf ("\tPUSH BX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							printf ("\tMOV BX,00000000H\n");
							printf ("\tPUSH BX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					printf ("\tMOV BX, [%s_cb]\n",temp->tok->lexeme);
					printf ("\tPUSH BX\n");	
				}


				else if(temp->id == TK_ID && temp->child!=NULL) // var:=j+A[i];
				{
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=1;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;
					boundCheck(vst,temp,fp);
					if(temp->child->id == TK_ID) // For dynamic index
					{
						printf ("\tMOV EBX,%s_cb\n",temp->tok->lexeme);
						printf ("\tPUSH EAX\n");
						printf ("\tAND EAX,00000000H\n");
						printf ("\tMOV AX,[%s_cb]\n",temp->child->tok->lexeme);
						printf ("\tSUB AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf ("\tIMUL AX,%d\n",size);
						printf ("\tADD EAX,EBX\n");
						printf ("\tMOV BX,[EAX]\n");
						printf ("\tPOP EAX\n");
					}
					else
					printf ("\tMOV BX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					printf ("\tPUSH BX\n");	
				}
			}

		}

		else if(temp->parent->id == TK_ASSIGNOP )   //simple assignment
		{
			if(temp->id == TK_NUM || temp->id==TK_RNUM)
			{
				printf ("\tMOV AX, %s\n",temp->tok->lexeme);
				printf ("\tPUSH AX\n");		
			}


			else if(temp->id == TK_ID && temp->child==NULL && temp->prev!=NULL)
			{
				printf ("\tMOV AX, [%s_cb]\n",temp->tok->lexeme);
				printf ("\tPUSH AX\n");	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev!=NULL) //type checking required i:=A[j]
			{
				
				vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
				int size;
				if(vst->arrayIndices->type == TK_BOOLEAN)
					size=1;
				else if(vst->arrayIndices->type == TK_INTEGER)
					size=2;
				else if(vst->arrayIndices->type == TK_REAL)
					size=4;

				boundCheck(vst,temp,fp);	
				if(temp->child->id == TK_ID)
				{
					printf ("\tPUSH AX\n");
					printf ("\tMOV EAX,%s_cb\n",temp->tok->lexeme);
					printf ("\tPUSH EBX\n");
					printf ("\tAND EBX,00000000H\n");
					printf ("\tMOV BX,[%s_cb]\n",temp->child->tok->lexeme);
					printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
					printf ("\tIMUL BX,%d\n",size);
					printf ("\tADD EBX,EAX\n");
					printf ("\tMOV AX,[EBX]\n");
					printf ("\tPOP EBX\n");
				}
				else
					printf ("\tMOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);

				printf ("\tPUSH AX\n");	
			}

			else if(temp->id == TK_ID && temp->child==NULL && temp->prev==NULL)
			{
				printf ("\tPOP AX\n");
				printf ("\tMOV [%s_cb],AX\n",temp->tok->lexeme);	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev==NULL) // A[i]:=j
			{
					printf ("\tPOP AX\n");	
					vst=searchVarInCurrentModule(lst,temp->tok->lexeme);
					int size;
					if(vst->arrayIndices->type == TK_BOOLEAN)
						size=1;
					else if(vst->arrayIndices->type == TK_INTEGER)
						size=2;
					else if(vst->arrayIndices->type == TK_REAL)
						size=4;
				boundCheck(vst,temp,fp);	
				if(temp->child->id == TK_ID)
					{
						printf ("\tPUSH EBX\n");
						printf ("\tPUSH AX\n");
						printf ("\tMOV EAX,%s_cb\n",temp->tok->lexeme);
						printf ("\tAND EBX,00000000H\n");
						printf ("\tMOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						printf ("\tSUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf ("\tIMUL BX,%d\n",size);
						printf ("\tADD EBX,EAX\n");
						printf ("\tPOP AX\n");
						printf ("\tMOV [EBX],AX\n");
						printf ("\tPOP EBX\n");
					}
				else
				printf ("\tMOV [%s_cb+%d],AX \n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
			}
		}
	}
}





int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf ("\tIncorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parseTree (argv[1]) ;
	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	baseST* base=fillSymbolTable(astRoot,0);

	printf ("global main\n") ;
	printf ("extern printf\n\n") ;
	printf ("section .text\n") ;

	printf ("main:\n") ;
	code_generation(astRoot,NULL);
	return 0;
}