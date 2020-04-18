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
	rightRange : db "%d" ,10, 0
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

	SUB RSP, 4			;Updating RSP

	SUB RSP, 12			;Updating RSP

	MOV BX,00000001H
	PUSH BX
	POP AX
	MOV [rbp - 14],AX		;Store

	MOV BX, 5
	PUSH BX
	POP AX
	MOV [rbp - 8],AX		;Store

	MOV BX, 9
	PUSH BX
	POP AX
	MOV [rbp - 10],AX		;Store


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 20
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 2], AX
	ADD RSP, 20


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 20
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 4], AX
	ADD RSP, 20

	MOV BX, [RBP - 2]
	PUSH BX
	MOV AX, [rbp - 10]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	MOV AX, [rbp - 2]
	PUSH AX
	MOV BX, [RBP - 10]
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
	MOV BX, [RBP - 4]
	PUSH BX
	MOV BX, [RBP - 10]
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

	MOV AX, [rbp - 6]
	PUSH AX
	MOV BX, 10
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JG LABEL1
	MOV AX,0
	PUSH AX
	JMP LABEL2

LABEL1:

	MOV AX,1
	PUSH AX


LABEL2:

	MOV AX, [rbp - 8]
	PUSH AX
	MOV BX, [RBP - 10]
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JLE LABEL3
	MOV AX,0
	PUSH AX
	JMP LABEL4

LABEL3:

	MOV AX,1
	PUSH AX


LABEL4:

	POP BX
	POP AX
	CMP AX,00000001h
	JNE LABEL5
	MOV AX,00000001h
	PUSH AX
	JMP LABEL7
	

LABEL5:
	CMP BX,00000001h
	JNE LABEL6
	MOV AX,00000001h
	PUSH AX
	JMP LABEL7
LABEL6:
	MOV AX,00000000h
	PUSH AX
LABEL7:
	MOV AX, [rbp - 2]
	PUSH AX
	MOV BX, [RBP - 4]
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JL LABEL8
	MOV AX,0
	PUSH AX
	JMP LABEL9

LABEL8:

	MOV AX,1
	PUSH AX


LABEL9:

	POP BX
	POP AX
	CMP AX,00000001h
	JE LABEL10
	MOV AX,00000000h
	PUSH AX
	JMP LABEL12
	

LABEL10:
	CMP BX,00000001h
	JNE LABEL11
	MOV AX,00000001h
	PUSH AX
	JMP LABEL12
LABEL11:
	MOV AX,00000000h
	PUSH AX
LABEL12:
	MOV BX, [RBP - 14]
	PUSH BX
	POP BX
	POP AX
	CMP AX,00000001h
	JE LABEL13
	MOV AX,00000000h
	PUSH AX
	JMP LABEL15
	

LABEL13:
	CMP BX,00000001h
	JNE LABEL14
	MOV AX,00000001h
	PUSH AX
	JMP LABEL15
LABEL14:
	MOV AX,00000000h
	PUSH AX
LABEL15:
	POP AX
	MOV [rbp - 16],AX		;Store

	MOV AX, [RBP - 6]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 14]		;printing boolean
	CMP AX, 01
	JE LABEL16
	MOV RDI, printFalse
	JMP LABEL17
LABEL16:
	MOV RDI, printTrue
LABEL17:
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

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
	SUB RSP, 20
	MOV RCX, 0
LABEL18:			;getting array
	MOV RBX, 18
	ADD RBX, RCX

	NEG RBX

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
	MOV [RBP+RBX], AX
	ADD RCX, 2
	CMP RCX, 12
	JNE LABEL18

	ADD RSP, 20
	MOV AX, [rbp - 8]
	PUSH AX
	MOV BX, [RBP - 2]
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV BX, 2
	PUSH BX
	MOV AX, [RBP-24]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV BX, 3
	PUSH BX
	MOV AX, [RBP-26]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	MOV BX, [RBP - 28]
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 4],AX		;Store

	MOV AX, [RBP - 4]		;printing integer
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RCX, 0
LABEL19:			;printing array
	MOV RBX, 18
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
	CMP RCX, 12
	JNE LABEL19


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
