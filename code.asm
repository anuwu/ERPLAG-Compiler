section .data
	printFormat : db "Output :  %d" , 10, 0
	printTrue : db "Output : true" , 10, 0
	printFalse : db "Output : false" , 10, 0
	inputPrompt : db "Enter an integer : " , 0
	inputInt : db "%d", 0
global main
extern scanf
extern printf

section .text
main:
	PUSH RBP
	PUSH RBP
	MOV RBP, RSP
	SUB RSP, 6			;Updating RSP

	SUB RSP, 6			;Updating RSP

	MOV BX, 5
	PUSH BX
	POP AX
	MOV [rbp - 8],AX		;Store

	MOV BX, 9
	PUSH BX
	POP AX
	MOV [rbp - 10],AX		;Store


	MOV RDI, inputPrompt		;get_value prompt
	XOR RAX, RAX
	MOV RSI, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 4
	MOV RSI, RSP
	SUB RSI, 4
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP-4]
	MOV [RBP - 2], AX
	ADD RSP, 4


	MOV RDI, inputPrompt		;get_value prompt
	XOR RAX, RAX
	MOV RSI, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 4
	MOV RSI, RSP
	SUB RSI, 4
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP-4]
	MOV [RBP - 4], AX
	ADD RSP, 4

	MOV BX, [rbp - 2]
	PUSH BX
	MOV AX, [rbp - 10]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	MOV AX, [rbp - 2]
	PUSH AX
	MOV BX, [rbp - 10]
	PUSH BX
	MOV AX, [rbp - 4]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV BX, [rbp - 4]
	PUSH BX
	MOV BX, [rbp - 10]
	PUSH BX
	MOV AX, [rbp - 8]
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV BX, 2
	PUSH BX
	MOV AX, [rbp - 8]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV AX, [rbp - 6]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	XOR RAX, RAX

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
