#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "parserDef.h"
#include "symbolTable.h"
#include "typeChecker.h"
#include "codegen.h"


#define setDone(x) for ( int i= 0 ; i<=x ; i++ ) isDone[i] = 1 ;

extern int printNextToken (twinBuffer* twinBuf);
extern baseST *realBase ;
extern int x ;

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
            moduleGeneration (node->child, localBase, rspDepth, node->localST, vst, fp);        // Access local scope and move below
            break ;

        case statement :
            rspDepth = moduleGeneration(node->child, localBase, rspDepth, lst, vst, fp);
            if (node->next == NULL)
            {
                if (lst->tableType == SWITCH_ST)
                    break ;

                if (lst->parent == realBase)
                {
                    fprintf (fp, "\n\tMOV RSP, RBP\n") ;
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
            int switchPass = 0 ;
            if (lst->tableType != SWITCH_ST)
                ; // Subtract RSP, and allocate memory if dynamic
            else if (node->parent != node)
                switchPass = 1 ; // Subtract RSP only, and do not allocate memory for dynamic
            else
            {
                switchPass = 2 ; // Do not subtract RSP, allocate memory if dynamic AND restore parent
                node->parent = node->next->parent ;
            }

            int endOffset, declCount = 1 ;
            astNode *idListHead = node->next->child ;
            astNode *dtNode = node->next->next ;

            while (idListHead->next != NULL)
            {
                declCount++ ;
                idListHead = idListHead->next ;
            }

            searchedVar = searchVar (realBase, lst, idListHead->tok->lexeme) ;
            endOffset = searchedVar->offset ;
            if (endOffset > 0 && (switchPass == 0 || switchPass == 1))
            {   
                fprintf (fp, "\tSUB RSP, %d\t\t\t;Updating RSP\n\n", (endOffset - rspDepth)) ;
                rspDepth = endOffset ;
            }

            if (dtNode->dtTag == ARRAY && !isVarStaticArr(searchedVar) && (switchPass == 0 || switchPass == 2))
                dynamicDeclareGeneration (lst, searchedVar, declCount, fp) ;

            
            break ;                                         

        case TK_ASSIGNOP :
            if (node->child->next == NULL)
                printf ("Unary\n") ;
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
            IDGeneration (node, lst, fp) ;
            break ;
        
        case TK_FOR :
            node=node->next;

            fprintf (fp, "\tMOV CX,%s\n", node->next->tok->lexeme);
            fprintf (fp, "\tMOV [RBP - %d], CX\t\t;for loop lower lim\n" , searchVar(realBase, lst, node->tok->lexeme)->offset);

            start_label = get_label();
            end_label = get_label();
            fprintf (fp, "\nLABEL%d:\n",start_label);

            fprintf (fp, "\tMOV AX, %s\n", node->next->next->tok->lexeme) ;
            fprintf (fp, "\tCMP CX,AX\n");
            fprintf (fp, "\tJG LABEL%d\n",end_label);

            moduleGeneration(node->next->next->next, rspDepth, rspDepth, lst, vst, fp);     // Statements

            fprintf (fp, "\n\tMOV CX, [RBP - %d]\t\t;Ending increment\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
            fprintf (fp, "\tINC CX\n");
            fprintf (fp, "\tMOV [RBP - %d],CX\n", searchVar(realBase, lst, node->tok->lexeme)->offset);
            fprintf (fp, "\tJMP LABEL%d",start_label);
            fprintf (fp, "\n\nLABEL%d:\n",end_label);
            break ;

        
        case TK_WHILE :
            node=node->next;
            start_label = get_label();
            end_label =  get_label();
            fprintf (fp, "\t\nLABEL%d:\n",start_label);

            moduleGeneration(node, localBase, rspDepth, lst, vst, fp);  // expression

            fprintf (fp, "\tPOP AX\n");
            fprintf (fp, "\tCMP AX,00000001h\n");
            fprintf (fp, "\tJNE LABEL%d\n",end_label);

            moduleGeneration(node->next, rspDepth, rspDepth, lst, vst, fp);     // statements

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
            node = node->next ;
            //printf ("here\n") ;
            printGeneration (node, lst, fp) ;
            break ;

        case TK_GET_VALUE :
            searchedVar = searchVar (realBase, lst, node->next->tok->lexeme) ;
            getValueGeneration (lst, searchedVar, rspDepth, fp) ;
            break ;

        case TK_SWITCH :
            ;
            int i, caseCount, savedRspDepth, def_label ;
            int *caseLabels ;
            astNode *statementsNode = node->next->next->next->next ;

            savedRspDepth = rspDepth ;

            rspDepth = switchDeclareVars (node->next->next->next->next, vst, rspDepth, fp) ;
            caseCount = getCaseCount (node->next->next->next->next) ;
            caseLabels = (int *) malloc (sizeof(int) * caseCount) ;
            def_label = switchCaseLabels (node , lst, caseCount, caseLabels, fp) ;

            end_label = get_label () ;
            i = 0 ;
            while (statementsNode != NULL)
            {
                fprintf (fp, "\nLABEL%d:\n", caseLabels[i]) ;
                moduleGeneration (statementsNode, savedRspDepth, rspDepth, lst, vst, fp) ;
                fprintf (fp ,"\n\tJMP LABEL%d\n", end_label) ;

                i++ ;
                if (statementsNode->next != NULL)
                {
                    if (statementsNode->next->id == TK_DEFAULT)
                    {
                        statementsNode = statementsNode->next->next ;
                        fprintf (fp, "\nLABEL%d:\n", def_label) ;
                        moduleGeneration (statementsNode, savedRspDepth, rspDepth, lst, vst, fp) ;

                        break ;
                    }
                    statementsNode = statementsNode->next->next->next ;
                }
                else
                    statementsNode = NULL ;
            }


            fprintf (fp ,"\nLABEL%d:\n", end_label) ;
            fprintf (fp, "\n\tADD RSP, %d\n", rspDepth - savedRspDepth) ;
            rspDepth = savedRspDepth ;

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
            codeGeneration (node->child->next, fp) ;        // <otherModules>
            break ;

        case otherModules :
            codeGeneration (node->child, fp) ;                  // Do module definitions
            codeGeneration (node->next, fp) ;                   // <driverModule> or NULL
            break ;

        case driverModule :
            moduleGeneration(node->child, 0, 0, NULL, NULL, fp);                // <statements>
            codeGeneration(node->next, fp);                 // Move to the next child of program
            break ;
    }
}

void lexCapability (twinBuffer* twinBuf)
{
    while (printNextToken(twinBuf))
    { ;
    }
}

int main(int argc, char *argv[]){

    if(argc<3){
		printf("Insufficient arguments\n");
		exit(0);
	}
	else if(argc > 3){
		printf("Too many arguments\n");
		exit(0);
	}

    twinBuffer * thisBuff ;
    treeNode * PTRoot ;
    astNode * ASTRoot ;
    baseST * thisbase ;
    int numberPTNode, numberASTNode, sizePT, sizeAST ;
    double compression ;

    int isDone[10] ; 
        for ( int i= 0 ; i < 10 ; i++ ) {
            isDone[i] = 0 ;
        }

    char chosen ;    
    do {
        printf ( "Enter Option for :\n\t0 : Exit ( enjoy your next 24 hours )\n\t1 : Lexer - Print Token List\n\t2 : Parser - Printing in INORDER TRAVERSAL\n\t3 : AST - Print Abstrat Syntax Tree (Reverse Post-Order)\n\t4 : Memory\n\t5 : Symbol Table - Prints the symbol Table\n\t6 : Activation Record Size\n\t7 : Static and dynamic arrays\n\t8 : Errors reporting and total compiling time\n\t9 : Code generation\n") ;    
        chosen = getchar() ;
        getchar() ;
        printf ("We got option : %c \n", chosen) ;
        switch (chosen-'0') {
            case 0:
                printf ("> Exiting Program\n") ;
                exit(1) ;

                setDone(0) ;
                break ;
            case 1:
                thisBuff = lexer_init ( argv[1] ) ;
                printf("> Printing all Tokens\n") ;
                printf ("%15s%15s%15s\n", "Line No.", "Lexeme", "Token") ;
                lexCapability ( thisBuff ) ;
                
                
                setDone(1) ;
                break ;
            case 2:
                init_parser() ;
                PTRoot = parseTree ( argv[1] ) ;

                if (PTRoot->syntax_error == 0)
                {
                    printf ("%20s%20s%20s%20s%20s%20s%20s\n", "Lexeme", "Line Number", "Token", "Value", "Parent Symbol", "isLeaf", "Node Symbol") ;
                    printf ("----------------------------------------------------------------------------------------------------------------------------------------------------------\n") ;
                    inorderTraversalonConsole(PTRoot) ;
                }
                else
                    printf ("Please rectify the above lexical/syntax errors\n") ;
                
                setDone(2) ;
                break ;
            case 3:
                if ( isDone[2] == 0) {
                    PTRoot = parseTree ( argv[1] ) ;
                }
                    
                ASTRoot = applyASTRule ( PTRoot ) ;
                inorderAST ( ASTRoot , 0 ) ;
                
                setDone(3) ;
                break ;
            case 4:
                if ( isDone[2] == 0 ) {
                    PTRoot = parseTree ( argv[1] ) ;
                }
                    
                if (isDone[3] == 0 ) {
                    ASTRoot = applyASTRule ( PTRoot ) ;
                }

                numberPTNode = getNumberPTNodes (PTRoot) ;
                sizePT = (int)sizeof(treeNode)*numberPTNode ;
                printf ("\nNo of parse tree nodes = %d and size = %d\n", numberPTNode, sizePT) ;

                numberASTNode = getNumberASTNodes (ASTRoot) ;
                sizeAST = (int)sizeof(astNode)*numberASTNode ;
                printf ("No of AST Nodes = %d and size = %d\n", numberASTNode , sizeAST) ;

                compression = (sizePT - sizeAST)/(1.0*sizePT) * 100.0 ;
                printf ("Compresson percentage = %lf%%\n", compression) ;


                setDone(4) ;
                break ;
            case 5:
                if ( isDone[2] == 0 ) {
                    PTRoot = parseTree ( argv[1] ) ;
                }
                
                if ( isDone[3] == 0 ) {
                    ASTRoot = applyASTRule ( PTRoot ) ;
                }

                thisbase = fillSymbolTable(ASTRoot , 0 ) ;

                printVars (thisbase) ;

                setDone(5) ;
                break ;                
            case 6:
                break ;
            case 7:
                break ;  
            case 8:
                break ;
            case 9:
                break ;  
            
            default :
                printf ( "Please Enter a valid Command\n") ;

        }
        

        printf ( "---------------------------------------------------------------\n") ;
    } while ( 1 ) ;


}