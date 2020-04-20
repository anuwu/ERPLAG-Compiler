/*
Group #52 -
Anwesh Bhattacharya (2016B5A70590P)
Deepak Chahar (2017A7PS0147P)
Rohan Kela (2016B1A70822P)
Komal Vasudeva (2017A7PS0103P)
*/


#ifndef _parserDef
#define _parserDef
#include "lexerDef.h"


typedef struct Node{
	tokenID key ;
	struct Node* next;		
}node;

typedef struct locs
{
	int ind;
	struct locs* next;
}location;
 
typedef struct Gram_Rule{
	int LHS;
	node* RHS;
}Rule;

location* init () ;
void populate_hash(int index,int enum_value) ;
tokenID grammarStringToTokenID (char *str) ;
node * initnode(int l) ;

void fillUtilityArray () ;

void recur(node* root) ;
void printrule() ;
void printfirst() ;
void printfollow();
void traverse_grammar() ;
void print_parse_table() ;
void testManualFF() ;

void addUniquely(tokenID target, tokenID insert_it) ;
void addUniquelyFollow(tokenID target, tokenID insert_it) ;
void unionFirst(tokenID target, tokenID source) ;
int containsEpsilon(tokenID id) ;
void printFirstList (tokenID id) ;
void populateFirst (tokenID id) ;
void printFollowSet (tokenID id) ;
void populateFollow (tokenID id) ;
void UnionFollowFirst (tokenID target, tokenID source) ;
void unionFollow(tokenID target, tokenID source) ;

#endif