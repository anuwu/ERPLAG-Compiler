typedef struct _astNode astNode ;

typedef enum _operation {
	PLUS , MINUS , MUL , DIV , ASSOP , EQUAL, NOT_EQUAL , GREATER_THAN , GREATER_EQUAL , LESS_THAN , LESS_EQUAL
} opType ;
typedef enum _structType {
	structPLUS 
} structType ;

struct _astNode {

	opType op ;
	dataType dt ;

	struct _astNode * left ; 
	struct _astNode * right ;
	/* copying relevant info */
}


astNode * fillBinaryOp (opType op , astNode * left , astNode * right ) {
	astNode * tmp = ( astNode * ) malloc ( sizeof ( astNode )) ;

	tmp->op = op ;
	tmp->dt = dt ; // real <- real + int

	tmp->left = left ; 
	tmp->right = right ;

	

}

astNode * 

astNode * fillASTNode ( opType op , dataType dt ,astNode left , astNode right ,  void* parameters ) {
	astNode * tmp = ( astNode * ) malloc ( sizeof ( astNode )) ;
	
	tmp->op = op ;

	
	


}

