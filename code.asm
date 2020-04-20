section .data
	boundPrint : db "Array out of bounds" , 10, 0
	declPrint : db "Invalid order of bounds in dynamic array declaration" , 10, 0
	declNeg : db "Negative bound in dynamic array declaration" , 10, 0
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

declNegERROR:
	PUSH RBP
	MOV RBP, RSP
	MOV RDI, declNeg
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV EDI, 0
	call exit
main:
	PUSH RBP
	PUSH RBP
	MOV RBP, RSP
	SUB RSP, 6			;Updating RSP

	SUB RSP, 2			;Updating RSP

	SUB RSP, 2			;Updating RSP

	MOV BX, 19
	PUSH BX
	POP AX
	MOV [rbp - 4],AX		;Store

	MOV BX, 56
	PUSH BX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV CX,3
	MOV [RBP - 10], CX		;for loop lower lim

LABEL1:
	MOV AX, 6
	CMP CX,AX
	JG LABEL2

	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 22
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 2], AX
	ADD RSP, 22


	MOV AX, [RBP - 2]

	CMP AX, 1
	JE LABEL3

	CMP AX, 2
	JE LABEL4

	JMP LABEL5

LABEL3:
	MOV BX, [RBP - 4]
	PUSH BX
	MOV BX, 2
	PUSH BX
	MOV AX, [rbp - 2]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	MOV BX, [RBP - 10]
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 4],AX		;Store

	MOV AX, [RBP - 4]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	JMP LABEL6

LABEL4:
	MOV BX, [RBP - 6]
	PUSH BX
	MOV BX, 3
	PUSH BX
	MOV AX, [rbp - 2]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	MOV BX, [RBP - 10]
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV AX, [RBP - 6]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	JMP LABEL6

LABEL5:
	MOV AX, [RBP - 2]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

LABEL6:

	ADD RSP, 0
	MOV AX, [RBP - 4]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 6]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	MOV CX, [RBP - 10]		;Ending increment
	INC CX
	MOV [RBP - 10],CX
	JMP LABEL1

LABEL2:

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
