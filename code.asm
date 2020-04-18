section .data
	boundEPrint : db "Array out of bounds" , 10, 0
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
extern printf

section .text
boundError:
	PUSH RBP
	MOV RBP, RSP
	MOV RDI, boundEPrint
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

	SUB RSP, 14			;Updating RSP

	MOV BX, 4
	PUSH BX
	POP AX
	MOV [RBP - 6], AX		;Store

	MOV CX,5
	MOV [RBP - 2], CX		;for loop lower lim

LABEL1:
	MOV AX, 10
	CMP CX,AX
	JG LABEL2
	MOV BX, 1
	PUSH BX
	MOV AX, [rbp - 2]
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 4],AX		;Store

	MOV AX, [rbp - 2]
	PUSH AX
	MOV AX, [RBP-4]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL3
	CALL boundError

LABEL3:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL4
	CALL boundError

LABEL4:
	MOV BX, 6
	MOV AX, [RBP-4]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV BX, [RBP+RBX]
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV AX, [RBP-2]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL5
	CALL boundError

LABEL5:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL6
	CALL boundError

LABEL6:
	MOV BX, 6
	MOV AX, [RBP-2]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX

	POP AX
	MOV[RBP+RBX], AX


	MOV CX, [RBP - 2]		;Ending increment
	INC CX
	MOV [RBP - 2],CX
	JMP LABEL1

LABEL2:

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RCX, 0
LABEL7:			;printing array
	MOV RBX, 6
	ADD RBX, RCX

	NEG RBX
	MOV AX, [RBP + RBX]
	MOV RDI, printInt
	MOVSX RSI, AX
	XOR RAX, RAX
	PUSH RCX
	PUSH RBX
	CALL printf
	POP RBX
	POP RCX

	ADD RCX, 2
	CMP RCX, 14
	JNE LABEL7


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP-4]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL8
	CALL boundError

LABEL8:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL9
	CALL boundError

LABEL9:
	MOV BX, 6
	MOV AX, [RBP-4]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV AX, [RBP + RBX]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
