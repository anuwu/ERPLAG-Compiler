#ifndef _parser
#define _parser
#include "parserDef.h"
#include "lexerDef.h"

typedef enum _tag
{
	TERMINAL, NON_TERMINAL
} nodeTag ;

typedef union _TNT {
	token* term ;
	tokenID nonTerm ;
} TNT ;

struct _astNode ;
typedef struct _astNode astNode;

typedef struct treeNode
{
	nodeTag tag ;
	TNT tnt ;

	astNode *syn, *inh ;
	int gcode ; // which rule was used 

	int syntax_error ;

	struct treeNode *child ;
	struct treeNode *next ;
	struct treeNode *parent ;
} treeNode ;

typedef struct _stacknode{
	int key;
	struct _stacknode* next;
} stacknode;

#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule

extern Rule rule[SIZE];
extern node* firsts[SIZE];
extern node* follows[SIZE];
extern location heads[SIZE];
extern int parsetable[150][150];
extern int rule_count;
extern char utility_array[150][40] ; 
static int counter = 0 ;


char* nodeSymbol (treeNode* node) ;
char* isLeafNode (treeNode* node) ;
char* tokenIDToSting (tokenID id) ;
void init_parser() ;
treeNode* create_root () ;
treeNode* create_node (nodeTag tag, TNT tnt) ;
void add_child (treeNode *parent, treeNode *node) ;
void print_node (treeNode *node) ;
void print_siblings (treeNode *node) ;
stacknode * initStack() ;
stacknode * push(stacknode * stc , tokenID token) ;
stacknode * pop(stacknode * stc) ;
stacknode * pushRule(stacknode * stc,Rule *allRules, int rule_index) ;
void printSL(stacknode * stc) ;
void test_tree () ;
void addRulesParseTree (treeNode *parent, Rule *allRules, int rule_index) ;
treeNode * nextTreeNode(treeNode* current_node) ;

treeNode* parse (FILE *fp) ;
void deletePT (treeNode *node) ;

#endif