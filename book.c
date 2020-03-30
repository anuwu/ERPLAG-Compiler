/*
l = e
e = e1 + t
e = t
t = t1 * f
t = f
f = ( e )
f = digit
*/

typedef struct _astNode {
	node * val ;
	struct _astNode * left ;
	struct _astNode * right ;
}astNode ;

astNode* fillNode ( node* root ) {
	if ( root == NULL )
		return NULL ;
	
	// post-order
	node * tmp = root->child ;

	while ( tmp != NULL ) {
		// processing first child
		fillNode
		
	}

}