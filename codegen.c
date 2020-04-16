#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"


#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule

twinBuffer *twinBuf ;

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
	 	printf("PUSH AX\n");
	 	if(temp->child->id == TK_ID) // variable index
	 		printf("MOV AX,[%s_cb]\n", temp->child->tok->lexeme);
	 	else //static index
	 		printf("MOV AX,%d\n", atoi(temp->child->tok->lexeme));

	 	printf("CMP AX,%d\n", atoi(vst->arrayIndices->tokRight->lexeme));

	 	int l1 = get_label();
	 	int l2 = get_label();
	 	printf("JG LABEL%d\n", l1);//It will give error
	 	printf("CMP AX,%d\n", atoi(vst->arrayIndices->tokLeft->lexeme));
	 	printf("JL LABEL%d\n", l1);
	 	printf("JMP LABEL%d\n", l2);
	 	printf("LABEL%d:\n", l1);

	 	printf("call exit\n");
	 	printf("LABEL%d:\n", l2);
	 	printf("POP AX\n");
	}
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_NUM) // Left bound is variable
	{
   		int l1=get_label();
	 	int l2=get_label();

   		printf("PUSH AX\n");
	 	if(temp->child->id == TK_ID) // variable index
	 		printf("MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf("MOV AX,%d\n",atoi(temp->child->tok->lexeme));

	 	printf("CMP AX,%d\n",atoi(vst->arrayIndices->tokRight->lexeme));
	 	printf("JG LABEL%d\n",l1);//It will give error
	 	printf("PUSH BX\n");
	 	printf("MOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	printf("CMP AX,BX\n");
	 	printf("JL LABEL%d\n",l1);
	 	printf("JMP LABEL%d\n",l2);
	 	printf("LABEL%d:\n",l1);
	 	printf("call exit\n");
	 	printf("LABEL%d:\n",l2);
	 	printf("POP BX\n");
	 	printf("POP AX\n");
	}
	else if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_ID) // Right bound is variable   
	{
   		int l1=get_label();
	 	int l2=get_label();
   		printf("PUSH AX\n");
   		printf("PUSH BX\n");
   		if(temp->child->id == TK_ID) // variable index
	 		printf("MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf("MOV AX,%d\n",atoi(temp->child->tok->lexeme));	
	 	printf("MOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf("CMP AX,BX\n");
	 	printf("JG LABEL%d\n",l1);//It will give error
	 	printf("CMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
	 	//printf("CMP AX,BX\n");
	 	printf("JL LABEL%d\n",l1);// It will give an error
	 	printf("JMP LABEL%d\n",l2);
	 	printf("LABEL%d:\n",l1);
	 	//printf("call error\n");
	 	printf("call exit\n");
	 	printf("LABEL%d:\n",l2);
	 	printf("POP BX\n");
	 	printf("POP AX\n"); 
	}
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_ID) // Both bounds are variable    		
    {
   		int l1=get_label();
	 	int l2=get_label();
   		printf("PUSH AX\n");
   		printf("PUSH BX\n");
	 	if(temp->child->id == TK_ID) // variable index
	 		printf("MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
	 	else //static index
	 		printf("MOV AX,%d\n",atoi(temp->child->tok->lexeme));	
	 	printf("MOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
	 	printf("CMP AX,BX\n");
	 	printf("JG LABEL%d\n",l1);//It will give error
	 	printf("MOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
	 	printf("CMP AX,BX\n");
	 	printf("JL LABEL%d\n",l1);// It will give an error
	 	printf("JMP LABEL%d\n",l2);
	 	printf("LABEL%d:\n",l1);
	 	//printf("call error\n");
	 	printf("call exit\n");
	 	printf("LABEL%d:\n",l2);
	 	printf("POP BX\n");
	 	printf("POP AX\n");
   }
}	   	 



varST* vst;
moduleST* lst;
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

			printf("MOV AX,%s\n", temp->next->tok->lexeme);
			printf("MOV [%s_cb], AX\n",temp->tok->lexeme );


			int start_label = get_label();
			int end_label = get_label();
			printf("\n\n\nLABEL%d:\n",start_label);

			printf("POP AX\n"); //4
			printf("PUSH AX\n");
			
			printf("MOV BX, [%s_cb]\n",temp->tok->lexeme);//variable
			printf("CMP BX,AX\n");
			printf("JG LABEL%d\n",end_label);

			code_generation(temp->next->next->next,fp);

			printf("MOV BX, [%s_cb]\n",temp->tok->lexeme);
			printf("INC BX\n");
			printf("MOV [%s_cb],BX\n",temp->tok->lexeme);
			printf("JMP LABEL%d",start_label);
			printf("\n\n\n\nLABEL%d:\n",end_label);
			printf("POP AX\n");
			printf("POP AX\n");
			printf("MOV [%s_cb],AX\n",temp->tok->lexeme);

		}
		else if(temp->id == TK_WHILE)  //checked
		{
			temp=temp->next;
			int start=get_label();
			int end=  get_label();
			printf("\nLABEL%d:\n",start);
			code_generation(temp,fp);
			printf("POP AX\n");
			printf("CMP AX,00000001h\n");
			printf("JNE LABEL%d\n",end);
			code_generation(temp->next,fp);
			printf("JMP LABEL%d\n",start);
			printf("\nLABEL%d:\n",end);

		}
		else if(temp->id == TK_PRINT) //DYNAMIC ARRAYS IS LEFT
		{
			temp=temp->next;
			vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
			if(vst->datatype == TK_INTEGER || vst->datatype == TK_REAL )
			{
				code_generation(temp,fp);
				printf("POP AX\n");
				printf("call outPut\n");
				printf("call printInt\n");
			}
			//check for boolean using ST			
			else if(vst->datatype == TK_BOOLEAN)
			{
				int start=get_label();
				int end=get_label();
				code_generation(temp,fp);
				printf("POP AX\n");
				printf("CMP AX,00000000h\n");
				printf( "JNE LABEL%d\n",start);
				printf("call outPut\n");
				printf("call faLSE\n");
				printf( "JMP LABEL%d\n",end);
				printf( "\nLABEL%d:\n",start);
				printf("call outPut\n");
				printf("call trUE\n");
				printf( "\nLABEL%d:\n",end);
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
					printf("call outPut\n");
					if(vst->arrayIndices->type == TK_BOOLEAN)  //static array of type boolean
					{
						for(int i=lb;i<=ub;i++)
						{	
							int start=get_label();
							int end=get_label();
							printf("MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
							printf("PUSH AX\n");
							printf("POP AX\n");
							printf("CMP AX,00000000h\n");
							printf( "JNE LABEL%d\n",start);
							//printf("call outPut\n");
							printf("call faLSE\n");
							printf( "JMP LABEL%d\n",end);
							printf( "\nLABEL%d:\n",start);
							//printf("call outPut\n");
							printf("call trUE\n");
							printf( "\nLABEL%d:\n",end);
						}
					}


					else
					{
						for(int i=lb;i<=ub;i++) //static array of type num
						{
							printf("MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
							printf("PUSH AX\n");
							printf("POP AX\n");
							printf("call printInt\n");
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
				printf("call inPut\n");
				printf("call readInt\n");
				printf("MOV [%s_cb],EAX\n",temp->tok->lexeme);
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

			printf("POP BX\n");
			printf("POP AX\n"); 
			if(temp->id == TK_AND)
			{
				printf("CMP AX,00000001h\n");
				printf("JE LABEL%d\n",start);
				printf("MOV AX,00000000h\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n",end);

				printf("\n\nLABEL%d:\n",start);
				printf("CMP BX,00000001h\n");
				printf("JNE LABEL%d\n",mid);
				printf("MOV AX,00000001h\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n",end);
				printf("\n\nLABEL%d:\n",mid);
				printf("MOV AX,00000000h\n");
				printf("PUSH AX\n");
				printf("\n\nLABEL%d:\n",end);

			}

			else
			{
				printf("CMP AX,00000001h\n");
				printf("JNE LABEL%d\n",start);
				printf("MOV AX,00000001h\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n",end);

				printf("\n\nLABEL%d:\n",start);
				printf("CMP BX,00000001h\n");
				printf("JNE LABEL%d\n",mid);
				printf("MOV AX,00000001h\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n",end);
				printf("\n\nLABEL%d:\n",mid);
				printf("MOV AX,00000000h\n");
				printf("PUSH AX\n");
				printf("\n\nLABEL%d:\n",end);

			}
		}
		
		else if(temp->id == TK_LT || temp->id == TK_GT || temp->id == TK_LE ||temp->id == TK_GE ||temp->id == TK_NE ||temp->id == TK_EQ)  //all correct
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			printf("POP BX\n");
			printf("POP AX\n");
			int start=get_label();
			int end=get_label();
			printf("CMP AX,BX\n\n");
			if(temp->id == TK_LT)
			{
				printf("JL LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_LE)
			{
				printf("JLE LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GT)
			{
				printf("JG LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GE)
			{
				printf("JGE LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				printf("JE LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				printf("JNE LABEL%d\n",start);
				printf("MOV AX,0\n");
				printf("PUSH AX\n");
				printf("JMP LABEL%d\n\n",end);
			}

			printf("LABEL%d:\n\n",start);
			printf("MOV AX,1\n");
			printf("PUSH AX\n\n\n");

			printf("LABEL%d:\n\n",end);

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
			printf("POP AX\n");
			printf("POP BX\n");
			printf("ADD AX,BX\n");
			printf("PUSH AX\n");
		}

		else if(temp->id == TK_MINUS )
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			printf("POP AX\n");
			printf("POP BX\n");
			printf("SUB AX,BX\n");
			printf("PUSH AX\n");
		}

		else if(temp->id == TK_MUL) //working 
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			printf("POP AX\n");
			printf("POP BX\n");
			printf("IMUL BX\n");
			printf("PUSH AX\n");
		}

		else if(temp->id == TK_DIV) //working
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			printf("POP BX\n");
			printf("POP AX\nCWD\n");

			printf("IDIV BX\n");
			printf("PUSH AX\n");
		}

		else if(temp->id == TK_NUM || temp->id == TK_RNUM || (temp->id==TK_ID && temp->parent->id!=TK_ASSIGNOP) ||temp->id == TK_TRUE || temp->id==TK_FALSE) //Add, Sub, Mul,DIv
		{
			if(temp->prev == NULL) //AX
			{
				if(temp->id == TK_NUM || temp->id==TK_RNUM)
				{
					printf("MOV AX, %s\n",temp->tok->lexeme);
					printf("PUSH AX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						printf("MOV AX,00000001H\n");
						printf("PUSH AX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							printf("MOV AX,00000000H\n");
							printf("PUSH AX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					printf("MOV AX, [%s_cb]\n",temp->tok->lexeme);
					printf("PUSH AX\n");	
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
					//printf("Starting bound checking\n");
					boundCheck(vst,temp,fp);
					//printf("Ended bound checking\n");
					if(temp->child->id == TK_ID) // For dynamic index
					{
						printf("MOV EAX,%s_cb\n",temp->tok->lexeme);
						printf("PUSH EBX\n");
						printf("MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						printf("SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf("IMUL BX,%d\n",size);
						printf("ADD EBX,EAX\n");
						printf("MOV AX,[EBX]\n");
						printf("POP EBX\n");
					}
					else
					printf("MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					printf("PUSH AX\n");	
				}
			}

			else   //BX
			{
				if((temp->id == TK_NUM || temp->id==TK_RNUM))
				{
					printf("MOV BX, %s\n",temp->tok->lexeme);
					printf("PUSH BX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						printf("MOV BX,00000001H\n");
						printf("PUSH BX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							printf("MOV BX,00000000H\n");
							printf("PUSH BX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					printf("MOV BX, [%s_cb]\n",temp->tok->lexeme);
					printf("PUSH BX\n");	
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
						printf("MOV EBX,%s_cb\n",temp->tok->lexeme);
						printf("PUSH EAX\n");
						printf("AND EAX,00000000H\n");
						printf("MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
						printf("SUB AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf("IMUL AX,%d\n",size);
						printf("ADD EAX,EBX\n");
						printf("MOV BX,[EAX]\n");
						printf("POP EAX\n");
					}
					else
					printf("MOV BX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					printf("PUSH BX\n");	
				}
			}

		}

		else if(temp->parent->id == TK_ASSIGNOP )   //simple assignment
		{
			if(temp->id == TK_NUM || temp->id==TK_RNUM)
			{
				printf("MOV AX, %s\n",temp->tok->lexeme);
				printf("PUSH AX\n");		
			}


			else if(temp->id == TK_ID && temp->child==NULL && temp->prev!=NULL)
			{
				printf("MOV AX, [%s_cb]\n",temp->tok->lexeme);
				printf("PUSH AX\n");	
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
					printf("PUSH AX\n");
					printf("MOV EAX,%s_cb\n",temp->tok->lexeme);
					printf("PUSH EBX\n");
					printf("AND EBX,00000000H\n");
					printf("MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
					printf("SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
					printf("IMUL BX,%d\n",size);
					printf("ADD EBX,EAX\n");
					printf("MOV AX,[EBX]\n");
					printf("POP EBX\n");
				}
				else
					printf("MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);

				printf("PUSH AX\n");	
			}

			else if(temp->id == TK_ID && temp->child==NULL && temp->prev==NULL)
			{
				printf("POP AX\n");
				printf("MOV [%s_cb],AX\n",temp->tok->lexeme);	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev==NULL) // A[i]:=j
			{
					printf("POP AX\n");	
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
						printf("PUSH EBX\n");
						printf("PUSH AX\n");
						printf("MOV EAX,%s_cb\n",temp->tok->lexeme);
						printf("AND EBX,00000000H\n");
						printf("MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						printf("SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						printf("IMUL BX,%d\n",size);
						printf("ADD EBX,EAX\n");
						printf("POP AX\n");
						printf("MOV [EBX],AX\n");
						printf("POP EBX\n");
					}
				else
				printf("MOV [%s_cb+%d],AX \n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
			}
		}
	}
}





int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Incorrect number of arguments\n") ;
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
