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
	SUB RSP, 2			;Updating RSP

	SUB RSP, 4			;Updating RSP

	SUB RSP, 6			;Updating RSP

	SUB RSP, 28			;Updating RSP

	SUB RSP, 12			;Updating RSP

	MOV AX, [RBP-4]
	CMP AX, 0
	JGE LABEL1
	CALL declNegERROR

LABEL1:
	MOV BX, [RBP-6]
	CMP BX, 0
	JGE LABEL2
	CALL declNegERROR

LABEL2:
	CMP BX, AX
	JGE LABEL3
	CALL declERROR

LABEL3:
	PUSH BX
	PUSH AX
	SUB BX, AX
	ADD BX, 1
	ADD BX, BX
	MOVSX RDI, BX

	CALL malloc
	MOV [RBP-52], RAX
	POP AX
	MOV [RBP-42], AX
	POP BX
	MOV [RBP-44], BX

	SUB RSP, 24			;Updating RSP

	MOV AX, [RBP-8]
	CMP AX, 0
	JGE LABEL4
	CALL declNegERROR

LABEL4:
	MOV BX, [RBP-12]
	CMP BX, 0
	JGE LABEL5
	CALL declNegERROR

LABEL5:
	CMP BX, AX
	JGE LABEL6
	CALL declERROR

LABEL6:
	PUSH BX
	PUSH AX
	SUB BX, AX
	ADD BX, 1
	ADD BX, BX
	MOVSX RDI, BX

	MOV CX, 0
	MOV RBX, 76
	NEG RBX

LABEL7:
	PUSH CX
	PUSH RBX
	PUSH RDI
	CALL malloc
	POP RDI
	POP RBX
	POP CX
	MOV [RBP+RBX], RAX
	POP AX
	MOV [RBP + RBX + 10], AX
	POP DX
	MOV [RBP + RBX + 8], DX
	PUSH DX
	PUSH AX
	ADD RBX, 12
	INC CX
	CMP CX, 2
	JL LABEL7
	ADD RSP, 4

	ADD RSP, 74

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
