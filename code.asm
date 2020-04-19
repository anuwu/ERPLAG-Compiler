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
	SUB RSP, 14			;Updating RSP

	SUB RSP, 4			;Updating RSP


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 30
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 16], AX
	ADD RSP, 30


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 30
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 18], AX
	ADD RSP, 30

	SUB RSP, 36			;Updating RSP


	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-20]
	MOV BX, [RBP-22]
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
	SUB RSP, 26
	MOV RCX, 0

LABEL1:			;getting array
	MOV RBX, RCX

	MOV RDI, inputInt		;get_value
	MOV RSI, RSP
	PUSH RBX
	PUSH RCX
	PUSH RSI
	CALL scanf
	POP RSI
	POP RCX
	POP RBX
	MOV AX, [RSP]
	MOV RDI, [RBP-30]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-20]
	MOV BX, [RBP-22]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL1

	ADD RSP, 26

	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-32]
	MOV BX, [RBP-34]
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
	SUB RSP, 26
	MOV RCX, 0

LABEL2:			;getting array
	MOV RBX, RCX

	MOV RDI, inputInt		;get_value
	MOV RSI, RSP
	PUSH RBX
	PUSH RCX
	PUSH RSI
	CALL scanf
	POP RSI
	POP RCX
	POP RBX
	MOV AX, [RSP]
	MOV RDI, [RBP-42]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-32]
	MOV BX, [RBP-34]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL2

	ADD RSP, 26
	MOV BX, 7
	PUSH BX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV CX,1
	MOV [RBP - 8], CX		;for loop lower lim

LABEL3:
	MOV AX, 3
	CMP CX,AX
	JG LABEL4

	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 26
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 2], AX
	ADD RSP, 26


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 26
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 4], AX
	ADD RSP, 26

	PUSH AX
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 10],AX		;Store

	PUSH AX
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 12],AX		;Store

	PUSH AX
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 14],AX		;Store

	MOV AX, [RBP - 10]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 12]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 14]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	MOV CX, [RBP - 8]		;Ending increment
	INC CX
	MOV [RBP - 8],CX
	JMP LABEL3

LABEL4:

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
