section .data
	printFormatArray : db "Output : " , 0
	printInt : db "%d ", 0
	printNewLine : db 10, 0
	printFormat : db "Output :  %d" , 10, 0
	printTrue : db "Output : true" , 10, 0
	printFalse : db "Output : false" , 10, 0
	true : db "true " , 0
	false : db "false " , 0
	inputIntPrompt : db "Enter an integer : " , 0
	inputBoolPrompt : db "Enter a boolean (0 or 1) : " , 0
	inputIntArrPrompt : db "Enter %d array elements of integer type for range ", 0
	inputBoolArrPrompt : db "Enter %d array elements of boolean type for range ", 0
	leftRange : db "%d to " , 0
	rightRange : db "%d" , 0
	inputInt : db "%d", 0
global main
extern scanf
extern printf

section .text
main:
	PUSH RBP
	PUSH RBP
	MOV RBP, RSP
	SUB RSP, 2			;Updating RSP

	SUB RSP, 12			;Updating RSP

	MOV RDI, inputIntArrPrompt
	MOV RSI, 6
	XOR RAX, RAX
	CALL printf

	MOV RDI, leftRange
	MOV RSI, 10
	XOR RAX, RAX
	CALL printf

	MOV RDI, rightRange
	MOV RSI, 15
	XOR RAX, RAX
	CALL printf
	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
