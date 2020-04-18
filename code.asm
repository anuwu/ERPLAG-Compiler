section .data
	printFormatArray : db "Output : " , 0
	printInt : db "%d ", 0
	printNewLine : db 10, 0
	printFormat : db "Output :  %d" , 10, 0
	printTrue : db "Output : true" , 10, 0
	printFalse : db "Output : false" , 10, 0
	true : db "true " , 0
	false : db "false " , 0
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
	SUB RSP, 2			;Updating RSP

	SUB RSP, 12			;Updating RSP

	MOV BX,00000001H
	PUSH BX
	POP AX
	MOV [RBP - 4], AX		;Store

	MOV BX,00000000H
	PUSH BX
	POP AX
	MOV [RBP - 6], AX		;Store

	MOV BX,00000001H
	PUSH BX
	POP AX
	MOV [RBP - 8], AX		;Store

	MOV BX,00000000H
	PUSH BX
	POP AX
	MOV [RBP - 10], AX		;Store

	MOV BX,00000001H
	PUSH BX
	POP AX
	MOV [RBP - 12], AX		;Store

	MOV BX,00000000H
	PUSH BX
	POP AX
	MOV [RBP - 14], AX		;Store


	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RCX, 0
LABEL1:			;printing array
	MOV RBX, 4
	ADD RBX, RCX

	NEG RBX
	MOV AX, [RBP + RBX]

	CMP AX, 01
	JE LABEL2
	MOV RDI, false
	JMP LABEL3
LABEL2:
	MOV RDI, true

LABEL3:
	XOR RAX, RAX
	PUSH RCX
	PUSH RBX
	CALL printf
	POP RBX
	POP RCX

	ADD RCX, 2
	CMP RCX, 12
	JNE LABEL1


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
