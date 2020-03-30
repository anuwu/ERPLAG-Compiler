#include <stdlib.h>
#include "ast.h"
#include "parser.h"

/*
typedef struct _astnode {
	tokenID tkID ;
	TNT tag ;

	struct _astnode *parent , *child , *next ;
} astNode ;
*/


astNode* newASTnonLeaf ( tokenID tkID , astNode * child ) {
	astNode * tmp = ( astNode * ) malloc ( sizeof ( astNode )) ;
	tmp->tag = NON_TERMINAL ;
	tmp->tkID = tkID ;

	tmp->child = child ;
	tmp->parent = tmp->next = NULL ;

	return tmp ;
}

astNode* newASTLeaf ( tokenID tkID , token tkn ) {
	astNode * tmp = (astNode * ) malloc ( sizeof ( astNode )) ;
	tmp->tkID = tkID ;
	tmp->lexeme = tkn.lexeme ;

	tmp->parent = tmp->child = tmp->next = NULL ;

	return tmp ;
}

astNode* generateASTtree ( treeNode* root ) {

	if( root == NULL )
		return NULL ;


	switch ( root->gcode ) {
		case 0: //program moduleDeclarations otherModules driverModule otherModules
			treeNode *one,*two,*three,*four ;
			one = root->child ; 
			two = one->next ;
			three = two->next ;
			four = three->next ;

			// post order traversal
			generateASTtree ( one ) ;
			generateASTtree ( two ) ;
			generateASTtree ( three ) ;
			generateASTtree ( four ) ;

			//processing this node
			if ( one->gcode == 2 ){
				//moduleDeclarations->EPS
				if ( two->gcode == 5 ){
					//otherModules->EPS
					//here driverModule is compulsary

					root->node = newASTnonLeaf ( TK_PROGRAM , three->node ) ;
				}
				else{
					// otherModules->module otherModules
					astNode * tmp = two->node ;
					while ( tmp->next != NULL )
						tmp = tmp->next ;
					tmp->next = three->node ;

					root->node = newASTnonLeaf ( TK_PROGRAM , two->node ) ;
				}
			}
			else {
				//moduleDeclarations->moduleDeclaraion moduleDeclarations
				astNode* tmp = one->node ;
				while( tmp->next != NULL )
					tmp = tmp->next ;
				
				if ( two->gcode == 5 ) //otherModules->EPS
					tmp->next = three->node ;
				else //otherModules->module otherModules
					tmp->next = two->node ;

				root->node = newASTnonLeaf ( TK_PROGRAM , one->node ) ;
			}
			break ;
		
		case 1: //moduleDeclarations moduleDeclaration moduleDeclarations
			treeNode *one, *two ;
			one = root->child ;
			two = one->next ;

			generateASTtree ( one ) ;
			generateASTtree ( two ) ;

			one->node->next = two->node ;
			root->node = one->node ;
			break ;
		
		case 2: //moduleDeclarations EPS
			root->node = NULL ;
			break ;

		case 3: //moduleDeclaration DECLARE MODULE ID SEMICOL
			treeNode * three = root->child->next->next ;

			generateASTtree ( three ) ;

			root->node = newASTLeaf ( TK_MODULE , three->node ) ;
			break ;
		
		case 4 : //otherModules module otherModules
			treeNode *one = root->child ;
			treeNode *two = one->child ;

			generateASTtree ( one ) ;
			generateASTtree ( two ) ;

			one->node->next = two->node ;
			root->node = one->node ;
			break ;

		case 5 : //otherModules EPS
			root->node = NULL ;
			break ;
			
		case 6 :  //driverModule DRIVERDEF DRIVER PROGRAM DRIVERENDDEF moduleDef
			treeNode *five = root->child->next->next->next->next ;

			generateASTtree ( five ) ;

			root->node = newASTnonLeaf ( TK_DRIVER , five->node ) ;
			break ;

		case 7 : //module DEF MODULE ID ENDDEF TAKES INPUT SQBO input_plist SQBC SEMICOL ret moduleDef
			treeNode * three = root->child->next->next ;
			treeNode *eight = root->child->next->next->next->next->next->next->next ;
			treeNode *eleven = eight->next->next->next ;
			treeNode *twelve = eleven->next ;

			generateASTtree ( eight ) ;
			generateASTtree ( eleven ) ;
			generateASTtree ( twelve ) ;

			root->node = newASTnonLeaf ( TK_MODULE , newASTLeaf ( three->tag , three->tnt->term ) ) ;
			root->node->child->next = eight->node ;
			eight->node->next = eleven->node ;
			eleven->node->next = twelve->node ;
			break ;

		case 8 : //moduleDef START statements END
			treeNode *two = root->child->next ;

			generateASTtree ( two ) ;

			root->node = two->node ;
			break ;

		case 9 : //ret RETURNS SQBO output_plist SQBC SEMICOL
			treeNode *three = root->child->next->next ;

			generateASTtree ( three ) ;

			root->node = three->node ;
			break ;

		case 10 : //ret EPS
			root->node = NULL ;
			break ;

		case 11 : //input_plist ID COLON dataType IPL
			treeNode *one = root->child->next ;
			treeNode *three = one->next->next ; // datatype and its next should be id
			treeNode *four = three->next ;

			generateASTtree ( four ) ;
			generateASTtree ( three ) ;

			three->node->next = newASTLeaf ( TK_ID , one->tnt->term ) ;
			three->node->next->next = four->node ;

			root->node = three->node ;
			break ;

		case 12 :	//IPL COMMA ID COLON dataType IPL
			treeNode * two = root->child->next ;
			treeNode * four = two->next->next ;
			treeNode * five = four->next ;

			generateASTtree ( five ) ;
			generateASTtree ( four ) ;

			four->node->next = newASTLeaf ( TK_ID , two->tnt->term ) ;
			four->node->next->next = five->node ;

			root->node = four->node ;
			break ;

		case 13 : //IPL EPS
			root->node = NULL ;
			break ;

		case 14 : //output_plist ID COLON type OPL
			treeNode *one = root->child ;
			treeNode *three = one->next->next ;
			treeNode *four = three->next ;

			generateASTtree ( four ) ;
			generateASTtree ( three ) ;

			three->node->next = newASTLeaf ( TK_ID , one->tnt->term ) ;
			three->node->next->next = four->node ;

			root->node = three->node ;
			break ;

		case 15 : //OPL COMMA ID COLON type OPL
			treeNode * two = root->child->next ;
			treeNode * four = two->next->next ;
			treeNode * five = four->next ;

			generateASTtree ( five ) ;
			generateASTtree ( four ) ;

			four->node->next = newASTLeaf ( TK_ID , two->tnt->term ) ;
			four->node->next->next = five->node ;

			root->node = four->node ;
			break ;

		case 16 : //OPL EPS
			root->node = NULL ;
			break ;

		case 17 : //statements statement statements
			treeNode *one = root->child ;
			treeNode *two = one->next ;

			generateASTtree ( one ) ;
			generateASTtree ( two ) ;

			root->node = one->node ;
			root->node->next = two->node ;
			break ;

		case 18 : //statements EPS
			root->node = NULL ;
			break ;
			
		case 19 : //statement ioStmt
		case 20 : //statement simpleStmt
		case 21 : //statement conditionalStmt
		case 22 : //statement declareStmt
		case 23 : //statement iterativeStmt
			treeNode * one = root->child ;
			
			generateASTtree ( one ) ;

			root->node = one->node ;
			break ;
			
		case 24 : //ioStmt GET_VALUE BO ID BC SEMICOL
			treeNode *one = root->child ;
			treeNode *three = one->next->next ;

			/* here we need to store TK_Getvalue and to which id */
			root->node = newASTnonLeaf ( TK_GET_VALUE , newASTLeaf ( TK_ID , three->tnt->term ) ) ;
			break ;

		case 25 : //ioStmt PRINT BO print_var BC SEMICOL
			treeNode * one = root->child ;
			treeNode * three = one->next->next ;

			generateASTtree ( three ) ;

			root->node = newASTnonLeaf ( TK_PRINT , three->node ) ;
			break ;

		case 26 : // print_var var
			treeNode *one = root->child ;

			generateASTtree ( one ) ;

			root->node = one->node ;
			break ;

		case 27 : //print_var TRUE
			root->node = newASTLeaf ( TK_TRUE , NULL ) ;
			break ;

		case 28 : //print_var FALSE
			root->node = newASTLeaf ( TK_FALSE , NULL ) ;
			break ;

		case 29 : //simpleStmt assignmentStmt
			treeNode * one = root->child ;

			generateASTtree ( one ) ;
			
			root->node = one->node ;
			break ;


		/* doubt */
		case 30 : //assignmentStmt ID whichStmt
			treeNode *one = root->child ;
			treeNode *two = one->next ;

			generateASTtree ( two ) ;
			root->node = newASTnonLeaf ( TK_ASSIGNOP , one->node ) ;
			one->node->next = two->node ;
			/* one->node->next == NULL */
			break ;

		case 31 : //whichStmt lvalueIDStmt
		case 32 : //whichStmt lvalueARRStmt
			treeNode *one = root->child ;

			generateASTtree ( one ) ;

			root->node = one->node ;
			break ;
		
		case 33 : //lvalueIDStmt ASSIGNOP expression_new SEMICOL

			break ;
		case 34 : 
			break ;
		case 35 : 
			break ;
		case 36 : 
			break ;
		case 37 : 
			break ;
		case 38 : 
			break ;
		case 39 : 
			break ;
		case 40 : 
			break ;
		case 41 : 
			break ;
		case 42 : 
			break ;
		case 43 : 
			break ;
		case 44 : 
			break ;
		case 45 : 
			break ;
		case 46 : 
			break ;
		case 47 : 
			break ;
		case 48 : 
			break ;
		case 49 : 
			break ;
		case 50 : 
			break ;
		case 51 : 
			break ;
		case 52 : 
			break ;
		case 53 : 
			break ;
		case 54 : 
			break ;
		case 55 : 
			break ;
		case 56 : 
			break ;
		case 57 : 
			break ;
		case 58 : 
			break ;
		case 59 : 
			break ;
		case 60 : 
			break ;
		case 61 : 
			break ;
		case 62 : 
			break ;
		case 63 : 
			break ;
		case 64 : 
			break ;
		case 65 : 
			break ;
		case 66 : 
			break ;
		case 67 : 
			break ;
		case 68 : 
			break ;
		case 69 : 
			break ;
		case 70 : 
			break ;
		case 71 : 
			break ;
		case 72 : 
			break ;
		case 73 : 
			break ;
		case 74 : 
			break ;
		case 75 : 
			break ;
		case 76 : 
			break ;
		case 77 : 
			break ;
		case 78 : 
			break ;
		case 79 : 
			break ;
		case 80 : 
			break ;
		case 81 : 
			break ;
		case 82 : 
			break ;
		case 83 : 
			break ;
		case 84 : 
			break ;
		case 85 : 
			break ;
		case 86 : 
			break ;
		case 87 : 
			break ;
		case 88 : 
			break ;
		case 89 : 
			break ;
		case 90 : 
			break ;
		case 91 : 
			break ;
		case 92 : 
			break ;
		case 93 : 
			break ;
		case 94 : 
			break ;
		case 95 : 
			break ;
		case 96 : 
			break ;
		case 97 : 
			break ;
		case 98 : 
			break ;
		case 99 : 
			break ;
		case 100 : 
			break ;
		case 101 : 
			break ;
		case 102 : //range_new NUM RANGEOP NUM
			treeNode *one = root->child ;
			treeNode *two = one->next ;
			treeNode *three = two->next ;

			// rangop->child is num1 and num1->next = num2
			astNode * tmp = newASTLeaf (TK_NUM , one->tnt->term )
			root->node = newASTnonLeaf ( TK_RANGEOP , tmp ) ;
			tmp->next = newASTLeaf ( TK_NUM , three->tnt->term ) ;
			break ;
	

	}

	return root->node ;
}