#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"



moduleST* lst;
varST* vst;

void print1()
{
	FILE *source, *target;
	source=fopen("utility1.txt","r");
	target=fopen("code.asm","a");
	if(target==NULL)
	{
		printf("error in opening file\n");
		exit(0);
	}
	char ch;
	while( ( ch = fgetc(source) ) != EOF )
      fputc(ch, target);
  	fclose(source);
  	fclose(target);
}

void print2()
{
	FILE *source, *target;
	source=fopen("utility2.txt","r");
	target=fopen("code.asm","a");
	char ch;
	if(target==NULL)
	{
		printf("error in opening file\n");
		exit(0);
	}
	while( ( ch = fgetc(source) ) != EOF )
      fputc(ch, target);
  	fclose(source);
  	fclose(target);	
}

int get_label()
{
	static int x=0;
	x++;
	return x;
}


void code_generation(astNode* astRoot, FILE* fp)
{
	astNode* temp = astRoot; // Initially stores the root node of the AST
	if(temp==NULL)
	{
		//fprintf(fp,"null");
		return;
	}

	if(temp->id <= idL)   //NON Terminals
	{
		if(temp->id == program)  // Encounters program as the root node
		{
			code_generation(temp->child,fp);  // Moves to children of program
		}

		else if(temp->id == moduleDeclarations )
		{
			code_generation(temp->next,fp); // It hasn’t been accounted for yet
		}

		else if(temp->id == otherModules )
		{
			code_generation(temp->next,fp); // It hasn’t been accounted for yet
		}	

		else if(temp->id == driverModule )
		{
			//fprintf(fp,"push BP\n");
			//fprintf(fp,"MOV BP,SP\n");
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
			//STORING ORIGINAL VALUE ON STACK
			 // fprintf(fp,"MOV AX, [%s_cb] \n",temp->tok->lexeme);
			 // fprintf(fp,"PUSH AX\n");
			code_generation(temp,fp);

			//storing upper bound 
			 // fprintf(fp,"MOV BX, %s\n",temp->next->next->tok->lexeme);//4
			 // fprintf(fp,"PUSH BX\n");//4
			code_generation(temp->next->next,fp);

			//changing value of loop variable to lower bound
			fprintf(fp,"MOV AX,%s\n", temp->next->tok->lexeme);
			fprintf(fp,"MOV [%s_cb], AX\n",temp->tok->lexeme );

			//fprintf(fp,"MOV AX, [%s_cb]\n",temp->tok->lexeme);
			//fprintf(fp,"PUSH AX\n");
			int start_label = get_label();
			int end_label = get_label();
			fprintf(fp,"\n\n\nLABEL%d:\n",start_label);
			//temp=temp->next->next->next;//statements


			fprintf(fp,"POP AX\n"); //4
			fprintf(fp,"PUSH AX\n");
			
			fprintf(fp,"MOV BX, [%s_cb]\n",temp->tok->lexeme);//variable
			fprintf(fp,"CMP BX,AX\n");
			fprintf(fp,"JG LABEL%d\n",end_label);
			code_generation(temp->next->next->next,fp);
			fprintf(fp,"AND BX,00000000h\n");
			fprintf(fp,"MOV BX, [%s_cb]\n",temp->tok->lexeme);
			fprintf(fp,"INC BX\n");
			fprintf(fp,"MOV [%s_cb],BX\n",temp->tok->lexeme);
			fprintf(fp,"JMP LABEL%d",start_label);
			fprintf(fp,"\n\n\n\nLABEL%d:\n",end_label);
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP AX\n");
			fprintf(fp,"MOV [%s_cb],AX\n",temp->tok->lexeme);

		}

		if(temp->id == TK_WHILE)  //checked
		{
			temp=temp->next;
			int start=get_label();
			int end=  get_label();
			fprintf(fp,"\nLABEL%d:\n",start);
			code_generation(temp,fp);
			fprintf(fp,"AND AX,00000000h\n");
			fprintf(fp,"POP AX\n");
			fprintf(fp,"CMP AX,00000001h\n");
			fprintf(fp,"JNE LABEL%d\n",end);
			code_generation(temp->next,fp);
			fprintf(fp,"JMP LABEL%d\n",start);
			fprintf(fp,"\nLABEL%d:\n",end);

		}

		else if(temp->id == TK_PRINT) //BOOLEAN prints 1/0   ARRAYS NOT DONE
		{
			temp=temp->next;
			if(temp->child==NULL)
			{
				code_generation(temp,fp);
				fprintf(fp,"POP AX\n");
				fprintf(fp,"call outPut\n");
				fprintf(fp,"call printInt\n");
			}
			//check for boolean using ST			

			else
			{
				fprintf(fp,"call outPut\n");
				//run a loop from lower to upper bound and call 
			}        

		}

		else if(temp->id == TK_GET_VALUE) //BOOLEAN CHECK, ARRAY
		{
			temp=temp->next;
			if(temp->child == NULL)//if not an array
			{
				fprintf(fp,"call inPut\n");
				fprintf(fp,"call readInt\n");
				fprintf(fp,"MOV [%s_cb],EAX\n",temp->tok->lexeme);
			}

			else
			{

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

			fprintf(fp,"POP BX\n");
			fprintf(fp,"POP AX\n"); 
			if(temp->id == TK_AND)
			{
				fprintf(fp,"CMP AX,00000001h\n");
				fprintf(fp,"JE LABEL%d\n",start);
				fprintf(fp,"MOV AX,00000000h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n",end);

				fprintf(fp,"\n\nLABEL%d:\n",start);
				fprintf(fp,"CMP BX,00000001h\n");
				fprintf(fp,"JNE LABEL%d\n",mid);
				fprintf(fp,"MOV AX,00000001h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n",end);
				fprintf(fp,"\n\nLABEL%d:\n",mid);
				fprintf(fp,"MOV AX,00000000h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"\n\nLABEL%d:\n",end);

			}

			else
			{
				fprintf(fp,"CMP AX,00000001h\n");
				fprintf(fp,"JNE LABEL%d\n",start);
				fprintf(fp,"MOV AX,00000001h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n",end);

				fprintf(fp,"\n\nLABEL%d:\n",start);
				fprintf(fp,"CMP BX,00000001h\n");
				fprintf(fp,"JNE LABEL%d\n",mid);
				fprintf(fp,"MOV AX,00000001h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n",end);
				fprintf(fp,"\n\nLABEL%d:\n",mid);
				fprintf(fp,"MOV AX,00000000h\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"\n\nLABEL%d:\n",end);

			}
		}
		
		else if(temp->id == TK_LT || temp->id == TK_GT || temp->id == TK_LE ||temp->id == TK_GE ||temp->id == TK_NE ||temp->id == TK_EQ)  //all correct
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			fprintf(fp,"POP BX\n");
			fprintf(fp,"POP AX\n");
			int start=get_label();
			int end=get_label();
			fprintf(fp,"CMP AX,BX\n\n");
			if(temp->id == TK_LT)
			{
				fprintf(fp,"JL LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_LE)
			{
				fprintf(fp,"JLE LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GT)
			{
				fprintf(fp,"JG LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_GE)
			{
				fprintf(fp,"JGE LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				fprintf(fp,"JE LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			else if(temp->id == TK_EQ)
			{
				fprintf(fp,"JNE LABEL%d\n",start);
				fprintf(fp,"MOV AX,0\n");
				fprintf(fp,"PUSH AX\n");
				fprintf(fp,"JMP LABEL%d\n\n",end);
			}

			fprintf(fp,"LABEL%d:\n\n",start);
			fprintf(fp,"MOV AX,1\n");
			fprintf(fp,"PUSH AX\n\n\n");

			fprintf(fp,"LABEL%d:\n\n",end);

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
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP BX\n");
			fprintf(fp,"ADD AX,BX\n");
			fprintf(fp,"PUSH AX\n");
		}

		else if(temp->id == TK_MINUS )
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP BX\n");
			fprintf(fp,"SUB AX,BX\n");
			fprintf(fp,"PUSH AX\n");
		}

		else if(temp->id == TK_MUL) //working 
		{
			code_generation(temp->child->next,fp);
			code_generation(temp->child,fp);
			fprintf(fp,"POP AX\n");
			fprintf(fp,"POP BX\n");
			fprintf(fp,"IMUL BX\n");
			fprintf(fp,"PUSH AX\n");
		}

		else if(temp->id == TK_DIV) //working
		{
			code_generation(temp->child,fp);
			code_generation(temp->child->next,fp);
			fprintf(fp,"POP BX\n");
			fprintf(fp,"POP AX\nCWD\n");

			fprintf(fp,"IDIV BX\n");
			fprintf(fp,"PUSH AX\n");
		}

		//temp->parent->id == TK_PLUS || temp->parent->id==TK_MINUS || temp->parent->id==TK_MUL ||temp->parent->id==TK_DIV  || temp->parent->id==TK_LT ||
		else if(temp->id == TK_NUM || temp->id == TK_RNUM || (temp->id==TK_ID && temp->parent->id!=TK_ASSIGNOP) ||temp->id == TK_TRUE || temp->id==TK_FALSE) //Add, Sub, Mul,DIv
		{
			if(temp->prev == NULL) //AX
			{
				if(temp->id == TK_NUM || temp->id==TK_RNUM)
				{
					fprintf(fp,"AND AX,00000000H\n");
					fprintf(fp,"MOV AX, %s\n",temp->tok->lexeme);
					fprintf(fp,"PUSH AX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						fprintf(fp,"MOV AX,00000001H\n");
						fprintf(fp,"PUSH AX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							fprintf(fp,"MOV AX,00000000H\n");
							fprintf(fp,"PUSH AX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					fprintf(fp,"AND AX,00000000H\n");
					fprintf(fp,"MOV AX, [%s_cb]\n",temp->tok->lexeme);
					fprintf(fp,"PUSH AX\n");	
				}


				else if(temp->id == TK_ID && temp->child!=NULL)
				{
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
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
						// fprintf(fp,"AND EAX,00000000H\n");
						// fprintf(fp,"MOV EAX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
						// fprintf(fp,"PUSH EAX\n");
						// return;
						//strcpy(reg,"R9");
					}
					fprintf(fp,"AND AX,00000000H\n");
					fprintf(fp,"MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					fprintf(fp,"PUSH AX\n");	
				}
			}

			else   //BX
			{
				if((temp->id == TK_NUM || temp->id==TK_RNUM))
				{
					fprintf(fp,"AND BX,00000000H\n");
					fprintf(fp,"MOV BX, %s\n",temp->tok->lexeme);
					fprintf(fp,"PUSH BX\n");		
				}

				else if(temp->id == TK_TRUE || temp->id==TK_FALSE)
				{
					if(temp->id == TK_TRUE)
					{
						fprintf(fp,"MOV BX,00000001H\n");
						fprintf(fp,"PUSH BX\n");	
					}

					else
					{
						if(temp->id == TK_FALSE)
						{
							fprintf(fp,"MOV BX,00000000H\n");
							fprintf(fp,"PUSH BX\n");	
						}
					}
				}

				else if(temp->id == TK_ID && temp->child==NULL)
				{
					fprintf(fp,"AND BX,00000000H\n");
					fprintf(fp,"MOV BX, [%s_cb]\n",temp->tok->lexeme);
					fprintf(fp,"PUSH BX\n");	
				}


				else if(temp->id == TK_ID && temp->child!=NULL)
				{
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
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
					fprintf(fp,"AND BX,00000000H\n");
					fprintf(fp,"MOV BX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
					fprintf(fp,"PUSH BX\n");	
				}
			}

		}

		else if(temp->parent->id == TK_ASSIGNOP )   //simple assignment
		{
			if(temp->id == TK_NUM || temp->id==TK_RNUM)
			{
				fprintf(fp,"MOV AX, %s\n",temp->tok->lexeme);
				fprintf(fp,"PUSH AX\n");		
			}


			else if(temp->id == TK_ID && temp->child==NULL && temp->prev!=NULL)
			{
				fprintf(fp,"MOV AX, [%s_cb]\n",temp->tok->lexeme);
				fprintf(fp,"PUSH AX\n");	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev!=NULL) //type checking required
			{
				
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
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
				fprintf(fp,"MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
				fprintf(fp,"PUSH AX\n");	
			}

			else if(temp->id == TK_ID && temp->child==NULL && temp->prev==NULL)
			{
				fprintf(fp,"POP AX\n");
				fprintf(fp,"MOV [%s_cb],AX\n",temp->tok->lexeme);	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev==NULL) //type checking required
			{
					vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
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
				fprintf(fp,"POP AX\n");
				fprintf(fp,"MOV [%s_cb+%d],AX \n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
				//fprintf(fp,"PUSH AX\n");
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
	inorderAST (astRoot, 0) ;
	baseST* base=fillSymbolTable(astRoot,0);
	printf ("*********************************************************\n") ;
	FILE* target=fopen("code.asm","w");
	fprintf(target,"SECTION .bss\n");
	fprintf(target,"\tlpBuffer: resb 18\n\tBuf_Len: equ $-lpBuffer\n");
	fclose(target);
	//allocate_memory();
	print1();
	target=fopen("code.asm","a");
	code_generation(astRoot,target);
	fclose(target);
	print2();
	//printf("Here");
	return 0;
}