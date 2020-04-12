#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

#define SPANOUT 10

extern char* tokenIDToString (tokenID id) ;

astNode* createASTNode (treeNode *PTNode)
{
	astNode *node = (astNode *) malloc (sizeof(astNode)) ;

 	if (PTNode->tag == NON_TERMINAL)
 	{
 		node->id = PTNode->tnt.nonTerm ;
 		node->tok = NULL ;
 	}
 	else
 	{
 		node->id = PTNode->tnt.term->id ;
 		node->tok = PTNode->tnt.term ;
 	}

 	node->parent = NULL ;
 	node->child = NULL ;
 	node->next = NULL ;
 	node->prev = NULL ;
 	node->dt = NULL ;
 	node->localST = NULL ;

 	PTNode->syn = node ;

 	return node ;
}

void inorderAST (astNode *node, int space)
{	
	if (node == NULL)
		return ;

	astNode *sib = node->child ;

	if (sib != NULL)
	{
		while (sib->next != NULL)
			sib = sib->next ;
		
		while (sib != NULL)
		{
			inorderAST (sib, space+1) ;
			sib = sib->prev ;
		}
	}

	for (int i = 0 ; i < 2*space ; i++)
		printf (" ") ;

	
	if (node->tok == NULL)
	{
		if (node->dt == NULL)
			printf ("|(%s,%s)\n", tokenIDToString(node->id) , (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
		else
		{
			printf ("|(%s,%s) ---> ", tokenIDToString(node->id) , (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
			if (node->dtTag == PRIMITIVE)
				printf ("%s", tokenIDToString(node->dt->pType)) ;
			else
				printf ("%s[%s .. %s]", tokenIDToString(node->dt->arrType->type), node->dt->arrType->tokLeft->lexeme, node->dt->arrType->tokRight->lexeme) ;
			printf ("\n") ;
		}
	}
	else	
		printf ("|(%s,%s,%s)\n", tokenIDToString(node->id), node->tok->lexeme, (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
}

void connectChildren (astNode *parent, astNode **siblings, int num)
{
	for (int i = 0 ; i < num - 1; i++)
	{
		siblings[i]->next = siblings[i+1] ;
		siblings[i]->parent = parent ;
	}

	siblings[num-1]->parent = parent ;
	for (int i = num-1 ; i > 0 ; i--)
		siblings[i]->prev = siblings[i-1] ;
}

astNode* applyASTRule (treeNode *PTNode)
{
	if (PTNode == NULL)
		return NULL ;
	
	astNode *node = NULL , *children[SPANOUT] ;
	treeNode *leftChild , *sibling ;
	datType *astDatType ;

	switch (PTNode->gcode)
	{
		case 0 :								// <program> --> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
			node = createASTNode (PTNode) ;


			// <moduleDeclarations>
			leftChild = PTNode->child ;		
			children[0] = createASTNode (leftChild) ; 
			applyASTRule (leftChild) ;	// 1, 2
			children[0]->child = leftChild->syn ;	// children[] is WRAPPERHEAD to leftChild->syn = HEAD of module Declaration list
			while (leftChild->syn != NULL)		// Ensuring that the linked list of module declarations below <moduleDeclaration> node point to the right parent
			{
				leftChild->syn->parent = children[0] ;
				leftChild->syn = leftChild->syn->next ;
			}

			// <otherModules>
			sibling = leftChild->next ;
			children[1] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 4, 5
			children[1]->child = sibling->syn ;		// children[] is WRAPPERHEAD to sibling->syn = HEAD of other Module definitions
			while (sibling->syn != NULL)		// Ensuring that the linked list of module definitions below <moduleDeclaration> node point to the right parent
			{
				sibling->syn->parent = children[1] ;
				sibling->syn = sibling->syn->next ;
			}

			// <driverModule>
			sibling = sibling->next ;		
			children[2] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 6
			children[2]->child = sibling->syn ;		// children[] = DriverModule, its child is statement WRAPPERHEAD
			sibling->syn->parent = children[2] ;	// The parent of STATEMENT WRAPPERHEAD is DriverModule

			// <otherModules>
			sibling = sibling->next ;		
			children[3] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 4, 5
			children[3]->child = sibling->syn ;		// children[] = <otherModules>, sibling->syn = HEAD of other Module definitions
			while (sibling->syn != NULL)	// Ensuring that the linked list of module definitions below <moduleDeclaration> node point to the right parent
			{
				sibling->syn->parent = children[3] ;
				sibling->syn = sibling->syn->next ;
			}

			// Connect the children at the program level.
			node->child = children[0] ;
			connectChildren (node, children, 4) ;
			break ;

		case 1 :								// <moduleDeclarations> --> <moduleDeclaration> <moduleDeclarations>
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			applyASTRule (leftChild) ;		// 3
			sibling->inh = leftChild->syn ;
			if (PTNode->inh != NULL)
			{
				PTNode->inh->next = sibling->inh ;
				sibling->inh->prev = PTNode->inh ;
			}

			applyASTRule (sibling) ;		// Recurse on 1 until case 2
			PTNode->syn = sibling->syn ;	// Making user the WRAPPER_HEAD node points to the HEAD
			break ;

		// <moduleDeclarations>|<otherMoudules>|<IPL>|<OPL>|<statements>|<default_new>|<caseStmt> --> eps
		case 2 : case 5 : case 13 : case 16 : case 18 : case 44 : case 46 : case 101 : 
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 3 :								// <moduleDeclaration> ---> DECLARE MODULE ID SEMICOL
			leftChild = PTNode->child ;
			sibling = leftChild->next->next ;

			PTNode->syn = createASTNode (sibling) ;
			break ;

		// <otherModules> --> <module> <otherModules> | <statements> --> <statement> <statements>
		case 4 : case 17 :								
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			createASTNode (leftChild) ;
			applyASTRule (leftChild) ;		// 7
			sibling->inh = leftChild->syn ;
			if (PTNode-> inh != NULL)
			{
				PTNode->inh->next = sibling->inh ;
				sibling->inh->prev = PTNode->inh ;
			}

			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;
			break ;

		case 6 :								// <driverModule> --> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			while (sibling->next != NULL)
				sibling = sibling->next ;

			// <moduleDef>
			applyASTRule (sibling) ;	// 8
			PTNode->syn = sibling->syn ;		// <driverModule>.syn = <moduleDef>.syn 

			break ;

		case 7 :								// <module> --> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
			leftChild = PTNode->child ;

			// ID
			sibling = leftChild->next->next ;
			children[0] = createASTNode (sibling) ;

			// <input_plist>
			while (sibling->tag != NON_TERMINAL)
				sibling = sibling->next ;

			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;	// 11
			node = sibling->syn->child ;


			// <ret>
			sibling = sibling->next->next->next ;
			children[2] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 9, 10
			children[2]->child = sibling->syn ;			// children [] is the WRAPPERHEAD to sibling->syn = HEAD of OPLs
			while (sibling->syn != NULL)
			{
				sibling->syn->parent = children[2] ;
				sibling->syn = sibling->syn->next ;
			}

			// <moduleDef>
			sibling = sibling->next ;
			applyASTRule (sibling) ;		// 8
			children[3] = sibling->syn ;	// children[] is the WRAPPERHEAD to the statement list


			// Connect the children at the module level
			PTNode->syn->child = children[0] ;
			connectChildren (PTNode->syn, children,4) ;
			break ;

		case 8 :								// <moduleDef> --> START <statements> END
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			// <statements>
			PTNode->syn = createASTNode (sibling) ;		// <moduleDef>.syn is Statements WRAPPERHEAD
			applyASTRule (sibling) ;	// 18
			PTNode->syn->child = sibling->syn;
			while (sibling->syn != NULL)	//Ensuring that statement list parents point to Statements WRAPPERHEAD
			{
				//printf("\t ha ha");
				sibling->syn->parent = PTNode->syn ;
				sibling->syn = sibling->syn->next ;
			}
			break ;

		case 9 :								// <ret> --> RETURNS SQBO <output_plist> SQBC SEMICOL
			leftChild = PTNode->child ;
			sibling = leftChild->next->next ;

			applyASTRule (sibling) ;		// 15
			PTNode->syn = sibling->syn ;
			break ;

		// <ret>|<optional>|<bT>|<aE>|<aT>|<aF>|<whichID> --> EPS
		case 10 : case 38 : case 51 : case 56 : case 59 : case 62 : case 92 :
			PTNode->syn = PTNode->inh ;
			break ;

		// <input_plist> | <output_plist>
		case 11 : case 14 :							// <input_plist> --> ID COLON <dataType> <IPL>
			// ID
			leftChild = PTNode->child ;
			children[0] = createASTNode (leftChild) ;

			// <dataType> | <type>
			sibling = leftChild->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 81, 82, 83, 84
			

			// <IPL>|<OPL> ---> Creating the first IPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the IPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;


			// Will recurse to 12 repeatedly until 13
			applyASTRule (sibling) ;		// 12, 13
			PTNode->syn->child = sibling->syn ;		// <output_plist>|<input_plist> is the WRAPPEDHEAD to the list of IPLs|OPLs.
			while (sibling->syn != NULL)
			{
				sibling->syn->parent = PTNode->syn ;
				sibling->syn = sibling->syn->next ;
			}
			break ;

		// <IPL>|<OPL> --> COMMA ID COLON <dataType>|<type> <IPL>|<OPL>
		case 12 : case 15 :						
			leftChild = PTNode->child ;

			// ID
			sibling = leftChild->next ;
			children[0] = createASTNode (sibling) ;

			// <dataType>
			sibling = sibling->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 81, 82, 83, 84

			// <IPL> ---> Creating the nth IPL|OPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the IPL|OPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL|OPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;

			// Linking the list
			PTNode->inh->next = sibling->inh ;
			sibling->inh->prev = PTNode->inh ;
			applyASTRule (sibling) ;		// Recurse 12 until 13
			

			PTNode->syn = sibling->syn ;			// sibling->syn = <IPL>|<OPL>.syn while the linked list is passed up
			break ;

		// <statement> --> <ioStmt> | <conditionalStmt> | <declareStmt> | <iterativeStmt>
		case 19 : case 21 : case 22 : case 23 :							
			leftChild=PTNode->child;
			applyASTRule(leftChild);	
			PTNode->syn->child = leftChild->syn;
			//leftChild->syn->parent = PTNode->syn;
			break;

		case 20 :								// <statement> --> <simpleStmt>
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;		// case 29, 35
			PTNode->syn->child = leftChild->syn ;
			while (leftChild->syn != NULL)			// Linking the parent here as it is too deep.
			{
				leftChild->syn->parent = PTNode->syn ;
				leftChild->syn = leftChild->syn->next ;
			}
			break ;

		case 24 : 								// <ioStmt> --> GET_VALUE BO ID BC SEMICOL
			leftChild = PTNode->child;
			sibling = leftChild ->next->next;
			astNode* gv_pointer = createASTNode(leftChild);
			leftChild->syn->parent = PTNode->parent->syn;

			createASTNode(sibling);
			leftChild->syn->next = sibling->syn;
			sibling->syn->parent = leftChild->syn->parent;
			sibling->syn->prev =leftChild->syn;
			PTNode->syn = gv_pointer;
			break; 

		case 25 :								// <ioStmt> --> PRINT BO <print_var> BC SEMICOL
			// print
			leftChild = PTNode->child;
			astNode* p_pointer = createASTNode(leftChild);
			p_pointer->parent = PTNode->parent->syn;
			PTNode->syn = p_pointer ;

			// <print_var>
			sibling = leftChild->next->next;
			applyASTRule(sibling);
			p_pointer->next = sibling->syn;
			sibling->syn->prev = p_pointer;
			sibling->syn->parent= p_pointer->parent; 

			break;
		
		case 26 :							// <print_var> --> <var>
			leftChild = PTNode->child;
			applyASTRule(leftChild);
			PTNode->syn = leftChild->syn;
			break;	
		
		// <print_var> --> TRUE | FALSE
		case 27 : case 28 :	
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;
		

		// <simpleStmt> --> <assignmentStmt> | <moduleReuseStmt>
		case 29 : case 35 :						
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;
			PTNode->syn = leftChild->syn ;
			break ;

		case 30 :							// <assignmentStmt> --> ID <whichStmt>
			// ID
			leftChild = PTNode->child ;
			createASTNode (leftChild) ;

			// <whichStmt>
			sibling = leftChild->next ;
			sibling->inh = leftChild->syn ;
			applyASTRule (sibling) ;		// case 31, 32
			PTNode->syn = sibling->syn ;
			break ;

		// <whichStmt> --> <lvalueIDStmt> | <lvalueARRStmt>
		case 31 : case 32 :						
			leftChild = PTNode->child ;
			leftChild->inh = PTNode->inh ;
			applyASTRule (leftChild) ;		// case 33
			PTNode->syn = leftChild->syn ;
			break ;

		case 33 :							// <lvalueIDStmt> --> ASSIGNOP <expression_new> SEMICOL
			// ASSIGNOP
			leftChild = PTNode->child ;
			PTNode->syn = createASTNode (leftChild) ;
			children[0] = PTNode->inh ;				// First child is ID coming down from <assignmentStmt>
			PTNode->syn->child = children[0] ;

			// <expression_new>
			sibling = leftChild->next ;
			applyASTRule (sibling) ;		// 47, 48
			children[1] = sibling->syn ;

			connectChildren (PTNode->syn , children, 2) ;
			break ;

		case 34 :								// <lvalueARRStmt> --> SQBO <index_new> SQBC ASSIGNOP <expression_new> SEMICOL
			children[0] = PTNode->inh ;			// ID will be the first child.
			leftChild = PTNode->child ;

			// <index_new>
			sibling = leftChild->next ;
			applyASTRule (sibling) ;
			children[0]->child = sibling->syn ;
			sibling->syn->parent = children[0] ;

			// ASSIGNOP
			sibling = sibling->next->next ;
			PTNode->syn = createASTNode (sibling) ;
			PTNode->syn->child = children[0] ;

			// <expression_new>
			sibling = sibling->next ;
			applyASTRule (sibling) ;
			children[1] = sibling->syn ;

			connectChildren (PTNode->syn, children, 2) ;

			break ;

		case 36 :							// <moduleReuseStmt> --> <optional> USE MODULE ID WITH PARAMETERS <idList> SEMICOL
			// optional
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;		// case 37, 38
			
			// ID
			sibling = leftChild->next->next->next ;
			children[0] =  createASTNode (sibling) ;

			// Linking the correct synthesized.
			PTNode->syn = sibling->syn ;
			if (leftChild->syn != NULL)
			{
				PTNode->syn = leftChild->syn ;
				leftChild->syn->next->next = sibling->syn ;		// linking ASSIGNOP of <optional>
				sibling->syn->prev = leftChild->syn->next ;
			}

			// idList
			sibling = sibling->next->next->next ;
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;

			connectChildren (NULL, children , 2) ;
			break ;

		case 37 :						// <optional> --> SQBO <idList> SQBC ASSIGNOP
			leftChild = PTNode->child ;

			// <idList>
			sibling = leftChild->next ;
			children[0] = createASTNode (sibling) ;
			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;		// Linking the synthesized. 

			node = sibling->syn->child ;

			// ASSIGNOP 
			sibling = sibling->next->next ;
			children[1] = createASTNode (sibling) ;

			connectChildren (NULL, children, 2) ;
			break ;
		
		case 39 :							// <declareStmt>--> DECLARE <idList> COLON <dataType> SEMICOL
			leftChild = PTNode->child ;
			children[0] = createASTNode (leftChild) ;
			PTNode->syn = children[0] ;

			sibling = leftChild->next ;
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;

			sibling = sibling->next->next ;
			children[2] = createASTNode (sibling) ;
			applyASTRule (sibling) ;

			connectChildren (PTNode->parent->syn, children, 3) ;
			break ;

		case 40 :							// <iterativeStmt> --> FOR BO ID IN <range_new> BC START <statements> END
			
			// for
			leftChild=PTNode->child;        //left child points to FOR node of Parse Tree  
			PTNode->syn = createASTNode(leftChild);//AST node for FOR created;
			PTNode->syn->parent = PTNode->parent->syn; // FOR AST node parent points to statement and not iterative statement
			PTNode->syn->prev=NULL;   //trivial

			// ID
			leftChild=leftChild->next->next; // ID
			PTNode->syn->next = createASTNode(leftChild); // AST node created for ID
			astNode* id_pointer= PTNode->syn->next;       //pointer to ast node of ID
			id_pointer->parent = PTNode->syn->parent;     //parent is same as parent of FOR
			id_pointer->prev = PTNode->syn;               //prev is FOR


			// range_new
			leftChild=leftChild->next->next; //range_new
			applyASTRule(leftChild);         //<range_new>
			id_pointer->next = leftChild->syn; //no need to create new AST node for range_new
			astNode* range_pointer = leftChild->syn; //pointer to ast node of first NUM of the range
			
			// lower NUM1 of range
			range_pointer->prev = id_pointer;
			range_pointer->parent = id_pointer->parent;

			// upper range NUM2
			range_pointer->next->prev = range_pointer;
			range_pointer=range_pointer->next;
			range_pointer->parent = id_pointer->parent;

			// statements
			leftChild = leftChild->next->next->next;		//<statements>
			range_pointer->next = createASTNode(leftChild); //creating AST node for <statements> 
			applyASTRule(leftChild);   //PTnode in case 17
			astNode* stat_pointer =  range_pointer->next;
			stat_pointer->prev = range_pointer;
			stat_pointer->parent = range_pointer->parent;
			astNode* temp = leftChild->syn;
			range_pointer->next->child = leftChild->syn;
			while(temp!=NULL)
			{
				temp->parent = stat_pointer; //stat_pointer === statements ast node
				temp=temp->next;
			}
			break;	

		case 41 :							// <iterativeStmt> --> WHILE BO <expression> BC START <statements> END
			// WHILE
			leftChild = PTNode->child ;
			children[0] = createASTNode (leftChild) ;
			PTNode->syn = children[0] ;

			// expression
			sibling = leftChild->next->next ;
			applyASTRule (sibling) ;
			children[1] = sibling->syn ;
			//applyASTRule (sibling) ; 	// case 49

			// <statements>
			sibling = sibling->next->next->next ;
			children[2] = createASTNode (sibling) ;
			applyASTRule (sibling) ;	// case 17, 18
			children[2]->child = sibling->syn ;
			while (sibling->syn != NULL)
			{
				sibling->syn->parent = children[2] ;
				sibling->syn = sibling->syn->next ;
			}

			connectChildren (PTNode->parent->syn, children, 3) ;
			break ;

		case 42 :							// <conditionalStmt> --> SWITCH BO ID BC START CASE <value> COLON <statements> BREAK SEMICOL <caseStmt> <default_new> END
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			PTNode->syn->parent = PTNode->parent->syn;
			PTNode->syn->prev = NULL;

			leftChild=leftChild->next->next;
			PTNode->syn->next = createASTNode(leftChild);
			astNode* id42_pointer = PTNode->syn->next;
			id42_pointer->prev = PTNode->syn;
			id42_pointer->parent = PTNode->syn->parent;

			//case
			leftChild = leftChild->next->next->next;
			astNode* case_pointer = createASTNode(leftChild);
			id42_pointer->next = case_pointer;
			case_pointer->prev = id42_pointer;
			case_pointer->parent=id42_pointer->parent;


			leftChild=leftChild->next;
			applyASTRule(leftChild);
			case_pointer->next = leftChild->syn;
			astNode* value_pointer = case_pointer->next;
			leftChild->syn->prev = case_pointer;
			leftChild->syn->parent = case_pointer->parent;


			leftChild = leftChild->next->next;		//<statements>
			value_pointer->next = createASTNode(leftChild); //creating AST node for <statements> 
			applyASTRule(leftChild);   //PTnode in case 17
			astNode* stat_pointer42 =  value_pointer->next;
			stat_pointer42->prev = value_pointer;
			stat_pointer42->parent = value_pointer->parent;
			astNode* temp42 = leftChild->syn;
			value_pointer->next->child = leftChild->syn;

			while(temp42!=NULL)
			{
				temp42->parent = stat_pointer42; //stat_pointer === statements ast node
				temp42=temp42->next;
			}
			
			leftChild=leftChild->next->next->next;
			astNode* daddy = createASTNode(leftChild);
			applyASTRule(leftChild);
			temp42 = leftChild->syn;
			while(temp42!=NULL)
			{
				temp42->parent = stat_pointer42->parent; //stat_pointer === statements ast node
				temp42=temp42->next;
			}
			
			if(leftChild->syn != NULL)
			{
				stat_pointer42->next=leftChild->syn;
				leftChild->syn->prev = stat_pointer42;
			}

			else
			{
					stat_pointer42->next=NULL;
			}
			
			astNode* iter = stat_pointer42;
			leftChild=leftChild->next;
			while(iter->next!=NULL)
			{
				iter=iter->next;
			}

			applyASTRule(leftChild);
			iter->next =leftChild->syn;
			if(leftChild->syn != NULL)
			{
				leftChild->syn->prev = iter;
				leftChild->syn->parent = iter->parent;
			}

			break;

		case 43:							// <caseStmt> --> CASE <value> COLON <statements> BREAK SEMICOL <caseStmt>
			leftChild = PTNode->child;
			astNode* case_pointer43 = createASTNode(leftChild);
			leftChild=leftChild->next;
			applyASTRule(leftChild);
			case_pointer43->next = leftChild->syn;
			astNode* value43_pointer = case_pointer43->next;
			value43_pointer->prev = case_pointer43;

			leftChild = leftChild->next->next;		//<statements>
			value43_pointer->next = createASTNode(leftChild); //creating AST node for <statements> 
			applyASTRule(leftChild);   //PTnode in case 17
			astNode* stat_pointer43 =  value43_pointer->next;
			stat_pointer43->prev = value43_pointer;
			astNode* temp43 = leftChild->syn;
			value43_pointer->next->child = leftChild->syn;
			while(temp43!=NULL)
			{
				temp43->parent = stat_pointer43; //stat_pointer === statements ast node
				temp43=temp43->next;
			}
			
			sibling = leftChild->next->next->next;
			sibling->inh = stat_pointer43;
			if (PTNode-> inh != NULL)
			{
				
				PTNode->inh->next = case_pointer43 ;
				sibling->inh->prev->prev->prev = PTNode->inh ;
			}

			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;
			break ;

		case 45:             			 	// <default_new> --> DEFAULT COLON <statements> BREAK SEMICOL
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			
			leftChild->syn->parent = PTNode->parent->parent->syn;

			leftChild = leftChild->next->next;

			PTNode->syn->next = createASTNode(leftChild); //creating AST node for <statements> 
			applyASTRule(leftChild);   //PTnode in case 17
			astNode* stat_pointer45 =  PTNode->syn->next;
			stat_pointer45->prev = PTNode->syn;
			stat_pointer45->parent = PTNode->syn->parent;
			astNode* temp45 = leftChild->syn;
			stat_pointer45->child = leftChild->syn;
			while(temp45!=NULL)
			{
				temp45->parent = stat_pointer45; //stat_pointer === statements ast node
				temp45=temp45->next;
			}
			break;


		// <expression_new> --> <expression> | <U>
		case 47 :							
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;
			PTNode->syn = leftChild->syn ;
			break ;

		// <expression> --> <boolTerm> <bT> | <boolTerm> --> <arithmeticExpr> <aE> | arithmeticExpr --> <term> <aT> | <term> --> <factor> <aF>
		case 49 : case 52 :	case 57 : case 60 :				
			// <boolTerm>
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;

			// <bT>
			sibling = leftChild->next ;
			sibling->inh = leftChild->syn ;
			applyASTRule (sibling) ;

			PTNode->syn = sibling->syn ;		// Linking the synthesized
			break ;

		// <aT> --> <pmop> <term> <aT> || <bT> --> <logicalOp> <boolTerm> <bT>
		case 50 : case 58 : case 61 :							
			// <logicalOp>
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;

			// <boolTerm>
			sibling = leftChild->next ;
			applyASTRule (sibling) ;		// case 67, 68

			// linking child of <logicalOp>, <bT>.inh and term.syn. Also the parents.
			leftChild->syn->child = PTNode->inh ;
			PTNode->inh->next = sibling->syn ;
			sibling->syn->prev = PTNode->inh ;
			PTNode->inh->parent = leftChild->syn ;
			sibling->syn->parent = leftChild->syn ;

			// <aT>
			sibling = sibling->next ;
			sibling->inh = leftChild->syn ;
			applyASTRule (sibling) ;

			PTNode->syn = sibling->syn ;
			break ;

		//  <boolTerm> --> TRUE | FALSE
		case 53 : case 54 :							
			leftChild = PTNode-> child ;
			PTNode->syn = createASTNode (leftChild) ;
			break ;

		case 55 :							// <aE> --> <relationalOp> <arithmeticExpr>
			// <relationalOp>
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;
			PTNode->syn = leftChild->syn ;

			// <arithmeticExpr>
			sibling = leftChild->next ;
			applyASTRule (sibling) ;

			// Linking the relational operator, PTNode.inh and right child
			leftChild->syn->child = PTNode->inh ;
			PTNode->inh->next = sibling->syn ;
			sibling->syn->prev = PTNode->inh ;
			PTNode->inh->parent = leftChild->syn ;
			sibling->syn->parent = leftChild->syn ;
			break ;

		// <U> --> (PLUS|MINUS) <factor_new>
		case 63 : case 64 :							
			leftChild = PTNode->child ;
			createASTNode (leftChild) ;
			PTNode->syn = leftChild->syn ;

			sibling = leftChild->next ;
			applyASTRule (sibling) ;		// case 66, 67
			leftChild->syn->child = sibling->syn ;
			sibling->syn->parent = leftChild->syn ;
			break ;

		// <factor> --> BO <expression> BC || <factor_new> --> BO <arithmeticExpr> BC
		case 65 : case 67 :						
			leftChild = PTNode->child ;

			sibling = leftChild->next ;
			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;
			break ;

		// <factor_new> --> <var> || <factor> --> <var>
		case 66 : case 68 :							
			leftChild = PTNode->child ;
			applyASTRule (leftChild) ;
			PTNode->syn = leftChild->syn ;
			break ;

		// case 69 to 80 ---> <logicalOp>, <relationalOp>, <pmop> and <mdop>
		case 69 : case 70 : case 71 : case 72 : case 73 : case 74 : case 75 : case 76 : case 77 : case 78 : case 79 : case 80 :
			leftChild = PTNode->child ;
			PTNode->syn = createASTNode(leftChild) ;
			break ;
			
		// case 81 to 83 ---> primitive <dataType>
		case 81 : case 82 : case 83 :
			PTNode->syn->dt = (datType *) malloc (sizeof(datType)) ;
			PTNode->syn->dtTag = PRIMITIVE ;
			PTNode->syn->dt->pType = PTNode->child->tnt.term->id ;
			break ;

		case 84 :								// <dataType> --> ARRAY SQBO <range> SQBC OF <type>
			PTNode->syn->dt = (datType *) malloc (sizeof(datType)) ;
			PTNode->syn->dtTag = ARRAY ;

			// <range>
			leftChild = PTNode->child ;
			sibling = leftChild->next->next ;
			sibling->inh = PTNode->syn ;	// Passing on <dataType> AST pointer down to range
			applyASTRule (sibling) ;	// 98

			// <type>
			sibling = sibling->next->next->next ;
			sibling->inh = PTNode->syn ;
			applyASTRule (sibling) ;	//85-87

			break ;

		// <type> --> INTEGER | REAL | BOOLEAN 
		case 85 : case 86 : case 87 :
			if (PTNode->parent->tnt.nonTerm == dataType)		// Array type
				PTNode->inh->dt->arrType->type = PTNode->child->tnt.term->id ;
			else if (PTNode->parent->tnt.nonTerm == output_plist || PTNode->parent->tnt.nonTerm == OPL)
				PTNode->syn->id = PTNode->child->tnt.term->id ;
			else
				printf ("<type> error!\n") ;
			break ;

		case 88:								// <var> --> ID <whichID>
			// <var>
			leftChild=PTNode->child;
			PTNode->syn = createASTNode(leftChild);

			// <whichID>
			sibling = leftChild->next;
			applyASTRule(sibling);
			if(sibling->syn == NULL)
				break;
	
			leftChild->syn->child = sibling->syn;
			sibling->syn->parent = leftChild->syn;	
			break;

		// <var> --> NUM | RNUM
		case 89 : case 90 :								// <var> --> NUM
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;
		
		case 91:								// <whichID> ---> SQBO <index_new> SQBC
			leftChild=PTNode->child->next;
			applyASTRule(leftChild);
			PTNode->syn = leftChild->syn;
			break;
		
		// <index_new> --> NUM | ID
		case 93 : case 94 :
			leftChild = PTNode->child ;

			if (PTNode->parent->tnt.nonTerm == range)		// Array type only
			{
				astDatType = PTNode->inh->dt ;		
				if (astDatType->arrType->tokLeft == NULL && astDatType->arrType->tokRight == NULL)
					astDatType->arrType->tokLeft = leftChild->tnt.term ;
				else if (astDatType->arrType->tokLeft != NULL && astDatType->arrType->tokRight == NULL)
					astDatType->arrType->tokRight = leftChild->tnt.term ;
				else
					printf ("\tError at either of the <index_new> of <dataType>\n") ;
			}
			else
				PTNode->syn = createASTNode (leftChild) ;

			break ;

		// <value> --> NUM|TRUE|FALSE
		case 95 : case 96 : case 97 :
			PTNode->syn = createASTNode(PTNode->child);
			break;	

		case 98 :								// <range> --> <index_new> RANGEOP <index_new>
			// Left <index_new>
			leftChild = PTNode->child ;
			PTNode->inh->dt->arrType = (arrayTypeInfo *) malloc (sizeof(arrayTypeInfo)) ;		// Array type only
			PTNode->inh->dt->arrType->tokLeft = NULL ;
			PTNode->inh->dt->arrType->tokRight = NULL ;

			leftChild->inh = PTNode->inh ;
			applyASTRule(leftChild) ; 	// 93, 94

			// Right <index_new>
			sibling = leftChild->next->next ;
			sibling->inh = PTNode->inh ;
			applyASTRule (sibling) ;	//93, 94
			break ;

		case 99:								// <idList> --> ID <idL>
			leftChild = PTNode->child ;
			createASTNode(leftChild) ;

			sibling = leftChild->next ;
			sibling->inh = leftChild->syn ;
			applyASTRule (sibling) ;
			PTNode->syn->child = sibling->syn ;	
			while (sibling->syn != NULL)	
			{
				sibling->syn->parent = PTNode->syn ;
				sibling->syn = sibling->syn->next ;
			}
			
			break ;
			

		case 100:								// <idL> --> COMMA ID <idL>
			leftChild = PTNode->child->next ;
			sibling = leftChild->next ;

			createASTNode (leftChild) ;
			sibling->inh = leftChild->syn ;
			if (PTNode->inh != NULL)
			{
				PTNode->inh->next = sibling->inh ;
				sibling->inh->prev = PTNode->inh ;
			}

			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;
			break ;
		
		case 102:								// <range_new> --> NUM RANGEOP NUM
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			leftChild=leftChild->next->next;
			PTNode->syn->next = createASTNode(leftChild);  //prev , parent pointer is set in case 39...
			break;
	}

	return node ;
}