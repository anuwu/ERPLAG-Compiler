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
	   	//fprintf(fp,"Entered static array\n");
		 	//fprintf(fp,"Entered static array, variable index type checking\n");
		 	fprintf(fp,"PUSH AX\n");
		 	if(temp->child->id == TK_ID) // variable index
		 		fprintf(fp,"MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
		 	else //static index
		 		fprintf(fp,"MOV AX,%d\n",atoi(temp->child->tok->lexeme));
		 	fprintf(fp,"CMP AX,%d\n",atoi(vst->arrayIndices->tokRight->lexeme));
		 	int l1=get_label();
		 	int l2=get_label();
		 	fprintf(fp,"JG LABEL%d\n",l1);//It will give error
		 	fprintf(fp,"CMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
		 	fprintf(fp,"JL LABEL%d\n",l1);
		 	fprintf(fp,"JMP LABEL%d\n",l2);
		 	fprintf(fp,"LABEL%d:\n",l1);
		 	//fprintf(fp,"call error\n");
		 	fprintf(fp,"call exit\n");
		 	fprintf(fp,"LABEL%d:\n",l2);
		 	fprintf(fp,"POP AX\n");
	   }
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_NUM) // Left bound is variable
	   {
	   //fprintf(fp,"Entered left dynamic array\n");
	   		int l1=get_label();
		 	int l2=get_label();
	   		fprintf(fp,"PUSH AX\n");
		 	if(temp->child->id == TK_ID) // variable index
		 		fprintf(fp,"MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
		 	else //static index
		 		fprintf(fp,"MOV AX,%d\n",atoi(temp->child->tok->lexeme));
		 	fprintf(fp,"CMP AX,%d\n",atoi(vst->arrayIndices->tokRight->lexeme));
		 	fprintf(fp,"JG LABEL%d\n",l1);//It will give error
		 	fprintf(fp,"PUSH BX\n");
		 	fprintf(fp,"MOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
		 	fprintf(fp,"CMP AX,BX\n");
		 	fprintf(fp,"JL LABEL%d\n",l1);
		 	fprintf(fp,"JMP LABEL%d\n",l2);
		 	fprintf(fp,"LABEL%d:\n",l1);
		 	//fprintf(fp,"call error\n");
		 	fprintf(fp,"call exit\n");
		 	fprintf(fp,"LABEL%d:\n",l2);
		 	fprintf(fp,"POP BX\n");
		 	fprintf(fp,"POP AX\n");
	   }
	else if(vst->arrayIndices->tokLeft->id == TK_NUM && vst->arrayIndices->tokRight->id == TK_ID) // Right bound is variable   
	   {
	   //fprintf(fp,"Entered right dynamic array\n");
	   	 
	   		int l1=get_label();
		 	int l2=get_label();
	   		fprintf(fp,"PUSH AX\n");
	   		fprintf(fp,"PUSH BX\n");
	   		if(temp->child->id == TK_ID) // variable index
		 		fprintf(fp,"MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
		 	else //static index
		 		fprintf(fp,"MOV AX,%d\n",atoi(temp->child->tok->lexeme));	
		 	fprintf(fp,"MOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
		 	fprintf(fp,"CMP AX,BX\n");
		 	fprintf(fp,"JG LABEL%d\n",l1);//It will give error
		 	fprintf(fp,"CMP AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
		 	//fprintf(fp,"CMP AX,BX\n");
		 	fprintf(fp,"JL LABEL%d\n",l1);// It will give an error
		 	fprintf(fp,"JMP LABEL%d\n",l2);
		 	fprintf(fp,"LABEL%d:\n",l1);
		 	//fprintf(fp,"call error\n");
		 	fprintf(fp,"call exit\n");
		 	fprintf(fp,"LABEL%d:\n",l2);
		 	fprintf(fp,"POP BX\n");
		 	fprintf(fp,"POP AX\n");
	 
	   }
	else if(vst->arrayIndices->tokLeft->id == TK_ID && vst->arrayIndices->tokRight->id == TK_ID) // Both bounds are variable    		
	   {
	  // fprintf(fp,"Entered dynamic array\n");
	   		int l1=get_label();
		 	int l2=get_label();
	   		fprintf(fp,"PUSH AX\n");
	   		fprintf(fp,"PUSH BX\n");
		 	if(temp->child->id == TK_ID) // variable index
		 		fprintf(fp,"MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
		 	else //static index
		 		fprintf(fp,"MOV AX,%d\n",atoi(temp->child->tok->lexeme));	
		 	fprintf(fp,"MOV BX,[%s_cb]\n",vst->arrayIndices->tokRight->lexeme);
		 	fprintf(fp,"CMP AX,BX\n");
		 	fprintf(fp,"JG LABEL%d\n",l1);//It will give error
		 	fprintf(fp,"MOV BX,[%s_cb]\n",vst->arrayIndices->tokLeft->lexeme);
		 	fprintf(fp,"CMP AX,BX\n");
		 	fprintf(fp,"JL LABEL%d\n",l1);// It will give an error
		 	fprintf(fp,"JMP LABEL%d\n",l2);
		 	fprintf(fp,"LABEL%d:\n",l1);
		 	//fprintf(fp,"call error\n");
		 	fprintf(fp,"call exit\n");
		 	fprintf(fp,"LABEL%d:\n",l2);
		 	fprintf(fp,"POP BX\n");
		 	fprintf(fp,"POP AX\n");
	   }
	 //fprintf(fp,"Leaving bound checking\n");  		
  }	   	 
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


varST* vst;
moduleST* lst;
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

		else if(temp->id == TK_PRINT) //DYNAMIC ARRAYS IS LEFT
		{
			temp=temp->next;
			vst=searchVarInCurrentModule (lst,temp->tok->lexeme);
			if(vst->datatype == TK_INTEGER || vst->datatype == TK_REAL )
			{
				code_generation(temp,fp);
				fprintf(fp,"POP AX\n");
				fprintf(fp,"call outPut\n");
				fprintf(fp,"call printInt\n");
			}
			//check for boolean using ST			
			else if(vst->datatype == TK_BOOLEAN)
			{
				int start=get_label();
				int end=get_label();
				code_generation(temp,fp);
				fprintf(fp,"POP AX\n");
				fprintf(fp,"CMP AX,00000000h\n");
				fprintf(fp, "JNE LABEL%d\n",start);
				fprintf(fp,"call outPut\n");
				fprintf(fp,"call faLSE\n");
				fprintf(fp, "JMP LABEL%d\n",end);
				fprintf(fp, "\nLABEL%d:\n",start);
				fprintf(fp,"call outPut\n");
				fprintf(fp,"call trUE\n");
				fprintf(fp, "\nLABEL%d:\n",end);
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
					fprintf(fp,"call outPut\n");
					if(vst->arrayIndices->type == TK_BOOLEAN)  //static array of type boolean
					{
						for(int i=lb;i<=ub;i++)
						{	
							int start=get_label();
							int end=get_label();
							fprintf(fp,"MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
							fprintf(fp,"PUSH AX\n");
							fprintf(fp,"POP AX\n");
							fprintf(fp,"CMP AX,00000000h\n");
							fprintf(fp, "JNE LABEL%d\n",start);
							//fprintf(fp,"call outPut\n");
							fprintf(fp,"call faLSE\n");
							fprintf(fp, "JMP LABEL%d\n",end);
							fprintf(fp, "\nLABEL%d:\n",start);
							//fprintf(fp,"call outPut\n");
							fprintf(fp,"call trUE\n");
							fprintf(fp, "\nLABEL%d:\n",end);
						}
					}


					else
					{
						for(int i=lb;i<=ub;i++) //static array of type num
						{
							fprintf(fp,"MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (i-lb)*size);
							fprintf(fp,"PUSH AX\n");
							fprintf(fp,"POP AX\n");
							fprintf(fp,"call printInt\n");
						}
					}
				}

				else  //dynamic array
				{

				}		
				 
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


				else if(temp->id == TK_ID && temp->child!=NULL) // var:=A[i]+j;
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
					//fprintf(fp,"Starting bound checking\n");
					boundCheck(vst,temp,fp);
					//fprintf(fp,"Ended bound checking\n");
					if(temp->child->id == TK_ID) // For dynamic index
					{
						fprintf(fp,"MOV EAX,%s_cb\n",temp->tok->lexeme);
						fprintf(fp,"PUSH EBX\n");
						fprintf(fp,"AND EBX,00000000H\n");
						fprintf(fp,"MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						fprintf(fp,"SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf(fp,"IMUL BX,%d\n",size);
						fprintf(fp,"ADD EBX,EAX\n");
						fprintf(fp,"MOV AX,[EBX]\n");
						fprintf(fp,"POP EBX\n");
					}
					else
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


				else if(temp->id == TK_ID && temp->child!=NULL) // var:=j+A[i];
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
					boundCheck(vst,temp,fp);
					if(temp->child->id == TK_ID) // For dynamic index
					{
						fprintf(fp,"MOV EBX,%s_cb\n",temp->tok->lexeme);
						fprintf(fp,"PUSH EAX\n");
						fprintf(fp,"AND EAX,00000000H\n");
						fprintf(fp,"MOV AX,[%s_cb]\n",temp->child->tok->lexeme);
						fprintf(fp,"SUB AX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf(fp,"IMUL AX,%d\n",size);
						fprintf(fp,"ADD EAX,EBX\n");
						fprintf(fp,"MOV BX,[EAX]\n");
						fprintf(fp,"POP EAX\n");
					}
					else
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


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev!=NULL) //type checking required i:=A[j]
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
				boundCheck(vst,temp,fp);	
				if(temp->child->id == TK_ID)
					{
						fprintf(fp,"PUSH AX\n");
						fprintf(fp,"MOV EAX,%s_cb\n",temp->tok->lexeme);
						fprintf(fp,"PUSH EBX\n");
						fprintf(fp,"AND EBX,00000000H\n");
						fprintf(fp,"MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						fprintf(fp,"SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf(fp,"IMUL BX,%d\n",size);
						fprintf(fp,"ADD EBX,EAX\n");
						fprintf(fp,"MOV AX,[EBX]\n");
						fprintf(fp,"POP EBX\n");
					}
				else
					fprintf(fp,"MOV AX, [%s_cb+%d]\n",temp->tok->lexeme, (atoi(temp->child->tok->lexeme)-atoi(vst->arrayIndices->tokLeft->lexeme))*size);
				fprintf(fp,"PUSH AX\n");	
			}

			else if(temp->id == TK_ID && temp->child==NULL && temp->prev==NULL)
			{
				fprintf(fp,"POP AX\n");
				fprintf(fp,"MOV [%s_cb],AX\n",temp->tok->lexeme);	
			}


			else if(temp->id == TK_ID && temp->child!=NULL && temp->prev==NULL) // A[i]:=j
			{
					fprintf(fp,"POP AX\n");	
					vst=searchVarInCurrentModule(lst,temp->tok->lexeme);
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
				boundCheck(vst,temp,fp);	
				if(temp->child->id == TK_ID)
					{
						fprintf(fp,"PUSH EBX\n");
						fprintf(fp,"PUSH AX\n");
						fprintf(fp,"MOV EAX,%s_cb\n",temp->tok->lexeme);
						fprintf(fp,"AND EBX,00000000H\n");
						fprintf(fp,"MOV BX,[%s_cb]\n",temp->child->tok->lexeme);
						fprintf(fp,"SUB BX,%d\n",atoi(vst->arrayIndices->tokLeft->lexeme));
						fprintf(fp,"IMUL BX,%d\n",size);
						fprintf(fp,"ADD EBX,EAX\n");
						fprintf(fp,"POP AX\n");
						fprintf(fp,"MOV [EBX],AX\n");
						fprintf(fp,"POP EBX\n");
					}
				else
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
	printf("Code prefix successfu\n");
	print1();
	printf("Target opening ...\n");
	if(fopen("code.asm","a")==NULL)
		printf("Error in locating target\n");
	target=fopen("code.asm","a");
	printf("Target opened\n");
	code_generation(astRoot,target);
	fclose(target);
	print2();
	printf("Here");
	return 0;
}
