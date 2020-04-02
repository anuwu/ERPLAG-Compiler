#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

#define SPANOUT 10

char* tokenIDToString (tokenID id)
{
	switch (id)
	{
		case TK_REAL :
			return "TK_REAL" ;
		case TK_INTEGER :
			return "TK_INTEGER" ;
		case TK_BOOLEAN :
			return "TK_BOOLEAN" ;
		case TK_ID :
			return "TK_ID" ;
		case TK_NUM :
			return "TK_NUM" ;
		case TK_RNUM :
			return "TK_RNUM" ;
		case TK_OF :
			return "TK_OF" ;
		case TK_ARRAY :
			return "TK_ARRAY" ;
		case TK_START :
			return "TK_START" ;
		case TK_END :
			return "TK_END" ;
		case TK_DECLARE :
			return "TK_DECLARE" ;
		case TK_MODULE :
			return "TK_MODULE" ;
		case TK_DRIVER :
			return "TK_DRIVER" ;
		case TK_PROGRAM :
			return "TK_PROGRAM" ;
		case TK_GET_VALUE :
			return "TK_GET_VALUE" ;
		case TK_PRINT :
			return "TK_PRINT" ;
		case TK_USE :
			return "TK_USE" ;
		case TK_WITH :
			return "TK_WITH" ;
		case TK_PARAMETERS :
			return "TK_PARAMETERS" ;
		case TK_TRUE :
			return "TK_TRUE" ;
		case TK_FALSE :
			return "TK_FALSE" ;
		case TK_TAKES :
			return "TK_TAKES" ;
		case TK_INPUT :
			return "TK_INPUT" ;
		case TK_RETURNS :
			return "TK_RETURNS" ;
		case TK_AND :
			return "TK_AND" ;
		case TK_OR :
			return "TK_OR" ;
		case TK_FOR :
			return "TK_FOR" ;
		case TK_IN :
			return "TK_IN" ;
		case TK_SWITCH:
			return "TK_SWITCH" ;
		case TK_CASE :
			return "TK_CASE" ;
		case TK_BREAK :
			return "TK_BREAK" ;
		case TK_DEFAULT :
			return "TK_DEFAULT" ;
		case TK_WHILE :
			return "TK_WHILE" ;
		case TK_PLUS :
			return "TK_PLUS" ;
		case TK_MINUS :
			return "TK_MINUS" ;
		case TK_MUL :
			return "TK_MUL" ;
		case TK_DIV :
			return "TK_DIV" ;
		case TK_LT :
			return "TK_LT" ;
		case TK_LE :
			return "TK_LE" ;
		case TK_GE :
			return "TK_GE" ;
		case TK_GT :
			return "TK_GT" ;
		case TK_EQ :
			return "TK_EQ" ;
		case TK_NE :
			return "TK_NE" ;
		case TK_DEF :
			return "TK_DEF" ;
		case TK_ENDDEF :
			return "TK_ENDDEF" ;
		case TK_DRIVERDEF :
			return "TK_DRIVERDEF" ;
		case TK_DRIVERENDDEF :
			return "TK_DRIVERENDDEF" ;
		case TK_COLON :
			return "TK_COLON" ;
		case TK_RANGEOP :
			return "TK_RANGEOP" ;
		case TK_SEMICOL :
			return "TK_SEMICOL" ;
		case TK_COMMA :
			return "TK_COMMA" ;
		case TK_ASSIGNOP :
			return "TK_ASSIGNOP" ;
		case TK_SQBO:
			return "TK_SQBO" ;
		case TK_SQBC :
			return "TK_SQBC" ;
		case TK_BO :
			return "TK_BO" ;
		case TK_BC :
			return "TK_BC" ;


		/************************************************************************
		*************************************************************************/


		case program :
			return "program" ;
		case moduleDeclarations :
			return "moduleDeclarations" ;
		case otherModules :
			return "otherModules" ;
		case moduleDeclaration :
			return "moduleDeclaration" ;
		case IPL :
			return "IPL" ;
		case input_plist :
			return "input_plist" ;
		case ret :
			return "ret" ;
		case moduleDef :
			return "moduleDef" ;
		case module :
			return "module" ;
		case driverModule :
			return "driverModule" ;
		case output_plist :
			return "output_plist" ;
		case OPL :
			return "OPL" ;
		case statements :
			return "statements" ;
		case statement :
			return "statement" ;
		case ioStmt :
			return "ioStmt" ;
		case print_var :
			return "print_var" ;
		case simpleStmt :
			return "simpleStmt" ;
		case whichStmt :
			return "whichStmt" ;
		case assignmentStmt :
			return "assignmentStmt" ;
		case lvalueIDStmt :
			return "lvalueIDStmt" ;
		case lvalueARRStmt :
			return "lvalueARRStmt" ;
		case moduleReuseStmt :
			return "moduleReuseStmt" ;
		case optional :
			return "optional" ;
		case declareStmt :
			return "declareStmt" ;
		case iterativeStmt :
			return "iterativeStmt" ;
		case conditionalStmt :
			return "conditionalStmt" ;
		case caseStmt :
			return "caseStmt" ;
		case default_new :
			return "default_new"  ;
		case expression_new :
			return "expression_new" ;
		case U :
			return "U" ;
		case expression :
			return "expression" ;
		case bT :
			return "bT" ;
		case boolTerm :
			return "boolTerm" ;
		case aE :
			return "aE" ;
		case arithmeticExpr :
			return "arithmeticExpr" ;
		case aT :
			return "aT" ;
		case term :
			return "term" ;
		case aF :
			return "aF" ;
		case factor :
			return "factor" ;
		case factor_new :
			return "factor_new" ;
		case logicalOp :
			return "logicalOp" ;
		case relationalOp :
			return "relationalOp" ;
		case pmop :
			return "pmop" ;
		case mdop :
			return "mdop" ;
		case dataType :
			return "dataType" ;
		case type :
			return "type" ;
		case var :
			return "var" ;
		case whichID :
			return "whichId" ;
		case index_new :
			return "index_new" ;
		case value :
			return "value" ;
		case range :
			return "range" ;
		case range_new :
			return "range_new" ;
		case idList :
			return "idList" ;
		case idL :
			return "idL" ;
 	}
}

astNode* createASTNode (treeNode *PTNode)
{
	astNode *node = (astNode *) malloc (sizeof(astNode)) ;

 	if (PTNode->tag == NON_TERMINAL)
 	{
 		//printf ("\tcreateASTNODE : PTNode->tnt.nonTerm = %d\n", PTNode->tnt.nonTerm) ;
 		node->id = PTNode->tnt.nonTerm ;
 		//printf ("\tcreateASTNODE : node->id = %d\n", node->id) ;
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
		{
			//inorderAST (sib, space + 1) ;
			sib = sib->next ;
		}

		
		while (sib != NULL)
		{
			inorderAST (sib, space+1) ;
			sib = sib->prev ;
		}
	}

	for (int i = 0 ; i < 2*space ; i++)
		printf (" ") ;

	
	if (node->tok == NULL)
		printf ("|(%s,%s)\n", tokenIDToString(node->id) , (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
	else	
		printf ("|(%s,%s,%s)\n", tokenIDToString(node->id), node->tok->lexeme, (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;

	//printf ("|(%s,%s)\n", (node->tok==NULL)?tokenIDToString(node->id):node->tok->lexeme, (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
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


		case 2 :								// <moduleDeclarations> --> EPS
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

		case 4 :								// <otherModules> --> <module> <otherModules>
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

		case 5 :								// <otherModules> --> EPS
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
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
			PTNode->syn->child = children[3] ;
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

		case 10 :								// <ret> --> EPS
			PTNode->syn = PTNode->inh ;
			break ;

		case 11 : 								// <input_plist> --> ID COLON <dataType> <IPL>
			// ID
			leftChild = PTNode->child ;
			children[0] = createASTNode (leftChild) ;

			// <dataType>
			sibling = leftChild->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 81, 82, 83, 84
			

			// <IPL> ---> Creating the first IPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the IPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;


			// Will recurse to 12 repeatedly until 13
			applyASTRule (sibling) ;		// 12, 13
			PTNode->syn->child = sibling->syn ;		// <input_plist> is the WRAPPEDHEAD to the list of IPLs.
			while (sibling->syn != NULL)
			{
				sibling->syn->parent = PTNode->syn ;
				sibling->syn = sibling->syn->next ;
			}
			break ;

		case 12 :								// <IPL> --> COMMA ID COLON <dataType> <IPL>
			leftChild = PTNode->child ;

			// ID
			sibling = leftChild->next ;
			children[0] = createASTNode (sibling) ;

			// <dataType>
			sibling = sibling->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 81, 82, 83, 84

			// <IPL> ---> Creating the nth IPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the IPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;

			// Linking the list
			PTNode->inh->next = sibling->inh ;
			sibling->inh->prev = PTNode->inh ;
			applyASTRule (sibling) ;		// Recurse 12 until 13
			

			PTNode->syn = sibling->syn ;			// sibling->syn = <IPL>.syn while the linked list is passed up
			break ;

		case 13 :								// <IPL> --> EPS
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 14 :								// <output_plist> --> ID COLON <type> <OPL>
			// ID
			leftChild = PTNode->child ;
			children[0] = createASTNode (leftChild) ;

			// <type>
			sibling = leftChild->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 85, 86, 87

			// <OPL> ---> Creating the first OPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the OPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;

			// Will recurse to 15 until 16
			applyASTRule (sibling) ;
			PTNode->syn = sibling->syn ;

			break ;

		case 15 :								// <OPL> --> COMMA ID COLON <type> <OPL>
			// ID
			leftChild = PTNode->child ;
			sibling = leftChild->next ;
			children[0] = createASTNode (sibling) ;

			// <type>
			sibling = sibling->next->next ;		
			children[1] = createASTNode (sibling) ;
			applyASTRule (sibling) ;		// 85, 86, 87

			// <OPL> ---> Creating the first OPL list node
			sibling = sibling->next ;
			node = createASTNode (sibling) ;		// node holds the OPL WRAPPERHEAD to the two-list (ID <-> dataType)
			node->child = children[0] ;				// IPL WRAPPERHEAD's leftmost child points to the HEAD of the two-list
			connectChildren (node, children, 2) ;
			sibling->inh = sibling->syn ;
			sibling->syn = NULL ;

			// Linking the list
			PTNode->inh->next = sibling->inh ;
			sibling->inh->prev = PTNode->inh ;
			applyASTRule (sibling) ;		// Recurse on 15 until 16

			PTNode->syn = sibling->syn ;

			break ;

		case 16 :								// <OPL> --> EPS
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 17:								//	<statements> --> <statement> <statements>
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

		case 18 :								// <statements> --> EPS
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 19 :								// <statement> --> <ioStmt>
			leftChild=PTNode->child;
			applyASTRule(leftChild);	
			PTNode->syn->child = leftChild->syn;
			leftChild->syn->parent = PTNode->syn;
			break;
				
		case 22 :                              	// <statement> --> <declareStmt>
			leftChild = PTNode->child;
			applyASTRule(leftChild);			// 39
			/*
			if(leftChild->syn ==NULL || PTNode->syn==NULL)
			{
				printf("LOL");
				break;
			}		
			*/
			PTNode->syn->child=leftChild->syn;
			leftChild->syn->parent = PTNode->syn;						 	    
			break;
		
		case 23 :								// <statement> --> <iterativeStmt>
			leftChild = PTNode->child;
			applyASTRule(leftChild);
			PTNode->syn->child = leftChild->syn;
			leftChild->syn->parent = PTNode->syn;
			break;

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
			leftChild = PTNode->child;
			sibling = leftChild->next->next;
			astNode* p_pointer = createASTNode(leftChild);
			p_pointer->parent = PTNode->parent->syn;

			applyASTRule(sibling);
			p_pointer->next = sibling->syn;
			sibling->syn->prev = p_pointer;
			sibling->syn->parent= p_pointer->parent; 
			PTNode->syn = p_pointer;
			break;
		
		case 26 :							// <print_var> --> <var>
			leftChild = PTNode->child;
			applyASTRule(leftChild);
			PTNode->syn = leftChild->syn;
			break;	
		
		case 27 :							// <print_var> --> TRUE
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;
		
		case 28 :							// <print_var> --> FALSE
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;
		
		case 39 :							// declareStmt--> DECLARE <idList> COLON <dataType> SEMICOL
			//declare
			leftChild = PTNode->child;   //declare
			astNode* declare_pointer = createASTNode(leftChild);   //creating ast node for declare
			declare_pointer->parent = PTNode->parent->syn; 
			declare_pointer->prev=NULL;

			// idList
			leftChild=leftChild->next;   
			declare_pointer->next = createASTNode(leftChild);	
			applyASTRule(leftChild);  	// 99
			astNode * idList_pointer = declare_pointer->next;  //creating pointer that points to idList ast node
			idList_pointer->prev = declare_pointer;
			idList_pointer->parent = declare_pointer->parent;

			//dataType
			leftChild=leftChild->next->next;  
			idList_pointer->next = createASTNode(leftChild);
			applyASTRule(leftChild);
			idList_pointer->next = leftChild->syn; 
			astNode* data_pointer = idList_pointer->next;  //creating pointer that points to dataType ast node
			data_pointer->prev = idList_pointer;
			data_pointer->parent = idList_pointer->parent;
			PTNode->syn = declare_pointer;
			break;	

		case 40 :							// <iterativeStmt> --> FOR BO ID IN range_new BC START statements END
			
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

		case 42:							// <conditionalStmt> --> SWITCH BO ID BC START CASE value COLON statements BREAK SEMICOL caseStmt default_new END
			leftChild= PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			leftChild = leftChild->next->next;
			PTNode->syn->parent = PTNode->parent->syn;

			PTNode->syn->next = createASTNode(leftChild);
			astNode* id42_pointer = PTNode->syn->next;
			id42_pointer->prev = PTNode->syn;
			id42_pointer->parent = PTNode->syn->parent;

			leftChild=leftChild->next->next->next->next;
			applyASTRule(leftChild);
			id42_pointer->next = leftChild->syn;
			leftChild->syn->prev = id42_pointer;
			leftChild->syn->parent = id42_pointer->parent;

			leftChild=leftChild->next->next;
			break ;
			




		case 81 :								// <dataType> --> INTEGER
			PTNode->syn->dt = (datType *) malloc (sizeof(datType)) ;
			PTNode->syn->dtTag = PRIMITIVE ;
			PTNode->syn->dt->pType = PTNode->child->tnt.term->id ;
			//printf("%d", PTNode->child->tnt.term->id);
			break ;

		case 82 : 								// <dataType> --> REAL
			PTNode->syn->dt = (datType *) malloc (sizeof(datType)) ;
			PTNode->syn->dtTag = PRIMITIVE ;
			PTNode->syn->dt->pType = PTNode->child->tnt.term->id ;
			break ;

		case 83 :								// <dataType> --> BOOLEAN
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

		case 85 :								// <type> --> INTEGER
			if (PTNode->parent->tnt.nonTerm == dataType)		// Array type
				PTNode->inh->dt->arrType->type = PTNode->child->tnt.term->id ;
			else if (PTNode->parent->tnt.nonTerm == output_plist || PTNode->parent->tnt.nonTerm == OPL)
				PTNode->syn->id = PTNode->child->tnt.term->id ;
			else
				printf ("<type> error!\n") ;
			break ;

		case 86 :								// <type> --> REAL
			if (PTNode->parent->tnt.nonTerm == dataType)		// Array type
				PTNode->inh->dt->arrType->type = PTNode->child->tnt.term->id ;
			else if (PTNode->parent->tnt.nonTerm == output_plist || PTNode->parent->tnt.nonTerm == OPL)
				PTNode->syn->id = PTNode->child->tnt.term->id ;
			else
				printf ("<type> error\n") ;

			break ;

		case 87 :								// <type> --> BOOLEAN
			if (PTNode->parent->tnt.nonTerm == dataType)		// Array type
				PTNode->inh->dt->arrType->type = PTNode->child->tnt.term->id ;
			else if (PTNode->parent->tnt.nonTerm == output_plist || PTNode->parent->tnt.nonTerm == OPL)
				PTNode->syn->id = PTNode->child->tnt.term->id ;
			else
				printf ("<type> error\n") ;

			break ;

		case 88:								// <var> --> ID <whichID>
			leftChild=PTNode->child;
			sibling = leftChild->next;
			PTNode->syn = createASTNode(leftChild);
			leftChild->syn->parent = PTNode->parent->parent->parent->syn;
			applyASTRule(sibling);
			if(sibling->syn == NULL)
				break;
	
			leftChild->syn->next = sibling->syn;
			sibling->syn->prev = leftChild->syn;
			sibling->syn->parent = leftChild->syn->parent;
			break;

		case 89:								// <var> --> NUM
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;

		case 90:								// <var> --> RNUM
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			break;
		
		case 91:								// <whichID> ---> SQBO <index_new> SQBC
			leftChild=PTNode->child->next;
			applyASTRule(leftChild);
			PTNode->syn = leftChild->syn;
			break;
		
		case 92:							// <whichID> EPS
			PTNode->syn = PTNode->inh ;
			break;

		case 93 :								// <index_new> --> NUM
			leftChild = PTNode->child ;

			if (PTNode->parent->tnt.nonTerm == range)		// Array type only
			{
				astDatType = PTNode->inh->dt ;		
				if (astDatType->arrType->lex1 == NULL && astDatType->arrType->lex2 == NULL)
					astDatType->arrType->lex1 = leftChild->tnt.term->lexeme ;
				else if (astDatType->arrType->lex1 != NULL && astDatType->arrType->lex2 == NULL)
					astDatType->arrType->lex2 = leftChild->tnt.term->lexeme ;
				else
					printf ("\tError at either of the <index_new> of <dataType>\n") ;
			}
			else
				PTNode->syn = createASTNode (leftChild) ;

			break ;

		case 94 :								// <index_new> --> ID
			leftChild = PTNode->child ;

			if (PTNode->parent->tnt.nonTerm == range)
			{
				astDatType = PTNode->inh->dt ;
				if (astDatType->arrType->lex1 == NULL && astDatType->arrType->lex2 == NULL)
					astDatType->arrType->lex1 = leftChild->tnt.term->lexeme ;
				else if (astDatType->arrType->lex1 != NULL && astDatType->arrType->lex2 == NULL)
					astDatType->arrType->lex2 = leftChild->tnt.term->lexeme ;
				else
					printf ("\tError at either of the <index_new> of <dataType>\n") ;
			}
			else
				PTNode->syn = createASTNode (leftChild) ;

			break ;

		case 95:								// value NUM
			PTNode->syn = createASTNode(PTNode->child);
			break;

		case 96:								// value TRUE
			PTNode->syn = createASTNode(PTNode->child);
			break;	

		case 97:								// value FLASE
			PTNode->syn = createASTNode(PTNode->child);
			break;		

		case 98 :								// <range> --> <index_new> RANGEOP <index_new>
			// Left <index_new>
			leftChild = PTNode->child ;
			PTNode->inh->dt->arrType = (arrayTypeInfo *) malloc (sizeof(arrayTypeInfo)) ;		// Array type only
			PTNode->inh->dt->arrType->lex1 = NULL ;
			PTNode->inh->dt->arrType->lex2 = NULL ;

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
		
		case 101:								// <idL> --> EPS		
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 102:										//range_new NUM RANGEOP NUM
			leftChild = PTNode->child;
			PTNode->syn = createASTNode(leftChild);
			leftChild=leftChild->next->next;
			PTNode->syn->next = createASTNode(leftChild);  //prev , parent pointer is set in case 39...
			break;

	}

	return node ;
}
