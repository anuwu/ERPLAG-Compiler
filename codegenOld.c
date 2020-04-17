/*
	if(node->id == TK_FOR) //checked restoring original value of k
	{
		node=node->next;

		codeGeneration(node,fp);
		codeGeneration(node->next->next,fp);

		printf ("\tMOV AX,%s\n", node->next->tok->lexeme);
		printf ("\tMOV [%s_cb], AX\n",node->tok->lexeme );


		int start_label = get_label();
		int end_label = get_label();
		printf ("\t\n\n\nLABEL%d:\n",start_label);

		printf ("\tPOP AX\n"); //4
		printf ("\tPUSH AX\n");
		
		printf ("\tMOV BX, [%s_cb]\n",node->tok->lexeme);//variable
		printf ("\tCMP BX,AX\n");
		printf ("\tJG LABEL%d\n",end_label);

		codeGeneration(node->next->next->next,fp);

		printf ("\tMOV BX, [%s_cb]\n",node->tok->lexeme);
		printf ("\tINC BX\n");
		printf ("\tMOV [%s_cb],BX\n",node->tok->lexeme);
		printf ("\tJMP LABEL%d",start_label);
		printf ("\t\n\n\n\nLABEL%d:\n",end_label);
		printf ("\tPOP AX\n");
		printf ("\tPOP AX\n");
		printf ("\tMOV [%s_cb],AX\n",node->tok->lexeme);

	}
	else if(node->id == TK_WHILE)  //checked
	{
		node=node->next;
		int start=get_label();
		int end=  get_label();
		printf ("\t\nLABEL%d:\n",start);
		codeGeneration(node,fp);
		printf ("\tPOP AX\n");
		printf ("\tCMP AX,00000001h\n");
		printf ("\tJNE LABEL%d\n",end);
		codeGeneration(node->next,fp);
		printf ("\tJMP LABEL%d\n",start);
		printf ("\t\nLABEL%d:\n",end);

	}
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

	else if(node->id == TK_AND || node->id == TK_OR) //checked
	{
		int start,mid,end;
		codeGeneration(node->child,fp);
		codeGeneration(node->child->next,fp);

		start=get_label();
		mid=get_label();
		end=get_label();

		printf ("\tPOP BX\n");
		printf ("\tPOP AX\n"); 
		if(node->id == TK_AND)
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
	
	else if(node->id == TK_LT || node->id == TK_GT || node->id == TK_LE ||node->id == TK_GE ||node->id == TK_NE ||node->id == TK_EQ)  //all correct
	{
		codeGeneration(node->child,fp);
		codeGeneration(node->child->next,fp);
		printf ("\tPOP BX\n");
		printf ("\tPOP AX\n");
		int start=get_label();
		int end=get_label();
		printf ("\tCMP AX,BX\n\n");
		if(node->id == TK_LT)
		{
			printf ("\tJL LABEL%d\n",start);
			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end);
		}

		else if(node->id == TK_LE)
		{
			printf ("\tJLE LABEL%d\n",start);
			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end);
		}

		else if(node->id == TK_GT)
		{
			printf ("\tJG LABEL%d\n",start);
			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end);
		}

		else if(node->id == TK_GE)
		{
			printf ("\tJGE LABEL%d\n",start);
			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end);
		}

		else if(node->id == TK_EQ)
		{
			printf ("\tJE LABEL%d\n",start);
			printf ("\tMOV AX,0\n");
			printf ("\tPUSH AX\n");
			printf ("\tJMP LABEL%d\n\n",end);
		}

		else if(node->id == TK_EQ)
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

	else if(node->id == TK_PLUS )
	{
		codeGeneration(node->child,fp);
		codeGeneration(node->child->next,fp);
		printf ("\tPOP AX\n");
		printf ("\tPOP BX\n");
		printf ("\tADD AX,BX\n");
		printf ("\tPUSH AX\n");
	}

	else if(node->id == TK_MINUS )
	{
		codeGeneration(node->child->next,fp);
		codeGeneration(node->child,fp);
		printf ("\tPOP AX\n");
		printf ("\tPOP BX\n");
		printf ("\tSUB AX,BX\n");
		printf ("\tPUSH AX\n");
	}

	else if(node->id == TK_MUL) //working 
	{
		codeGeneration(node->child->next,fp);
		codeGeneration(node->child,fp);
		printf ("\tPOP AX\n");
		printf ("\tPOP BX\n");
		printf ("\tIMUL BX\n");
		printf ("\tPUSH AX\n");
	}

	else if(node->id == TK_DIV) //working
	{
		codeGeneration(node->child,fp);
		codeGeneration(node->child->next,fp);
		printf ("\tPOP BX\n");
		printf ("\tPOP AX\nCWD\n");

		printf ("\tIDIV BX\n");
		printf ("\tPUSH AX\n");
	}

	else if(node->id == TK_NUM || node->id == TK_RNUM || (node->id==TK_ID && node->parent->id!=TK_ASSIGNOP) ||node->id == TK_TRUE || node->id==TK_FALSE) //Add, Sub, Mul,DIv
	{
		if(node->prev == NULL) //AX
		{
			if(node->id == TK_NUM || node->id==TK_RNUM)
			{
				printf ("\tMOV AX, %s\n",node->tok->lexeme);
				printf ("\tPUSH AX\n");		
			}

			else if(node->id == TK_TRUE || node->id==TK_FALSE)
			{
				if(node->id == TK_TRUE)
				{
					printf ("\tMOV AX,00000001H\n");
					printf ("\tPUSH AX\n");	
				}

				else
				{
					if(node->id == TK_FALSE)
					{
						printf ("\tMOV AX,00000000H\n");
						printf ("\tPUSH AX\n");	
					}
				}
			}

			else if(node->id == TK_ID && node->child==NULL)
			{
				printf ("\tMOV AX, [%s_cb]\n",node->tok->lexeme);
				printf ("\tPUSH AX\n");	
			}


			else if(node->id == TK_ID && node->child!=NULL) // var:=A[i]+j;
			{
				vst=searchVarInCurrentModule (lst,node->tok->lexeme);
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
				boundCheck(vst,node,fp);
				//printf ("\tEnded bound checking\n");
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

		else   //BX
		{
			if((node->id == TK_NUM || node->id==TK_RNUM))
			{
				printf ("\tMOV BX, %s\n",node->tok->lexeme);
				printf ("\tPUSH BX\n");		
			}

			else if(node->id == TK_TRUE || node->id==TK_FALSE)
			{
				if(node->id == TK_TRUE)
				{
					printf ("\tMOV BX,00000001H\n");
					printf ("\tPUSH BX\n");	
				}

				else
				{
					if(node->id == TK_FALSE)
					{
						printf ("\tMOV BX,00000000H\n");
						printf ("\tPUSH BX\n");	
					}
				}
			}

			else if(node->id == TK_ID && node->child==NULL)
			{
				printf ("\tMOV BX, [%s_cb]\n",node->tok->lexeme);
				printf ("\tPUSH BX\n");	
			}


			else if(node->id == TK_ID && node->child!=NULL) // var:=j+A[i];
			{
				vst=searchVarInCurrentModule (lst,node->tok->lexeme);
				int size;
				if(vst->arrayIndices->type == TK_BOOLEAN)
					size=1;
				else if(vst->arrayIndices->type == TK_INTEGER)
					size=2;
				else if(vst->arrayIndices->type == TK_REAL)
					size=4;
				boundCheck(vst,node,fp);
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

	else if(node->parent->id == TK_ASSIGNOP )   //simple assignment
	{

		if(node->id == TK_ID && node->child==NULL && node->prev!=NULL)
		{
			printf ("\tMOV AX, [%s_cb]\n",node->tok->lexeme);
			printf ("\tPUSH AX\n");	
		}


		else if(node->id == TK_ID && node->child!=NULL && node->prev!=NULL) //type checking required i:=A[j]
		{
			
			vst=searchVarInCurrentModule (lst,node->tok->lexeme);
			int size;
			if(vst->arrayIndices->type == TK_BOOLEAN)
				size=1;
			else if(vst->arrayIndices->type == TK_INTEGER)
				size=2;
			else if(vst->arrayIndices->type == TK_REAL)
				size=4;

			boundCheck(vst,node,fp);	
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
			printf ("\tMOV [%s_cb],AX\n",node->tok->lexeme);	
		}


		else if(node->id == TK_ID && node->child!=NULL && node->prev==NULL) // A[i]:=j
		{
				printf ("\tPOP AX\n");	
				vst=searchVarInCurrentModule(lst,node->tok->lexeme);
				int size;
				if(vst->arrayIndices->type == TK_BOOLEAN)
					size=1;
				else if(vst->arrayIndices->type == TK_INTEGER)
					size=2;
				else if(vst->arrayIndices->type == TK_REAL)
					size=4;
			boundCheck(vst,node,fp);	
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
*/