section .data
	boundPrint : db "Array out of bounds" , 10, 0
	declPrint : db "Invalid order of bounds in dynamic array declaration" , 10, 0
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
	rightRange : db "%d" ,10, 0
	inputInt : db "%d", 0
global main
extern scanf
extern exit
extern malloc
extern printf

section .text
boundERROR:
	PUSH RBP
	MOV RBP, RSP
	MOV RDI, boundPrint
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV EDI, 0
	call exit
declERROR:
	PUSH RBP
	MOV RBP, RSP
	MOV RDI, declPrint
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV EDI, 0
	call exit
main:
	PUSH RBP
	PUSH RBP
	MOV RBP, RSP
	SUB RSP, 4			;Updating RSP


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 28
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 2], AX
	ADD RSP, 28


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 28
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 4], AX
	ADD RSP, 28

	SUB RSP, 12			;Updating RSP

	MOV AX, [RBP-2]
	MOV [RBP-6], AX
	MOV BX, [RBP-4]
	MOV [RBP-8], BX
	CMP BX, AX
	JGE LABEL1
	CALL declERROR

LABEL1:
	SUB BX, AX
	ADD BX, 1
	ADD BX, BX
	MOVSX RDI, BX
	CALL malloc
	MOV [RBP-16], RAX

	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-6]
	MOV BX, [RBP-8]
	MOV SI, BX
	SUB SI, AX
	ADD SI, 1
	MOVSX RSI, SI
	PUSH AX
	XOR RAX, RAX
	CALL printf
	POP AX

	MOV RDI, leftRange
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	MOV RDI, rightRange
	MOVSX RSI, BX
	XOR RAX, RAX
	CALL printf

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
