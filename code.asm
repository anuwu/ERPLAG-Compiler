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
	MOV [RBP - 2], AX
	ADD RSP, 30

	SUB RSP, 4			;Updating RSP

	MOV BX, 999
	PUSH BX
	POP AX
	MOV [rbp - 4],AX		;Store

	MOV BX, 999
	PUSH BX
	POP AX
	MOV [rbp - 6],AX		;Store

	SUB RSP, 4			;Updating RSP

	SUB RSP, 2			;Updating RSP

	SUB RSP, 4			;Updating RSP

	SUB RSP, 14			;Updating RSP


	MOV AX, [RBP - 2]

	CMP AX, 1
	JE LABEL1

	CMP AX, 2
	JE LABEL2

	CMP AX, 3
	JE LABEL3

	JMP LABEL4

LABEL1:

	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 18
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 8], AX
	ADD RSP, 18


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 18
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 10], AX
	ADD RSP, 18

	MOV AX, [rbp - 8]
	PUSH AX
	MOV BX, 0
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JG LABEL6
	MOV AX,0
	PUSH AX
	JMP LABEL7

LABEL6:

	MOV AX,1
	PUSH AX


LABEL7:

	MOV AX, [rbp - 10]
	PUSH AX
	MOV BX, 0
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JG LABEL8
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
	MOV AX, [rbp - 10]
	PUSH AX
	MOV BX, [RBP - 8]
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JGE LABEL13
	MOV AX,0
	PUSH AX
	JMP LABEL14

LABEL13:

	MOV AX,1
	PUSH AX


LABEL14:

	POP BX
	POP AX
	CMP AX,00000001h
	JE LABEL15
	MOV AX,00000000h
	PUSH AX
	JMP LABEL17
	

LABEL15:
	CMP BX,00000001h
	JNE LABEL16
	MOV AX,00000001h
	PUSH AX
	JMP LABEL17
LABEL16:
	MOV AX,00000000h
	PUSH AX
LABEL17:
	POP AX
	MOV [rbp - 12],AX		;Store


	MOV AX, [RBP - 12]

	CMP AX, 1
	JE LABEL18

	CMP AX, 0
	JE LABEL19

LABEL18:
	MOV AX, [RBP - 12]
	CMP AX, 01
	JE LABEL21
	MOV RDI, printFalse
	JMP LABEL22
LABEL21:
	MOV RDI, printTrue
LABEL22:
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV AX, [RBP - 8]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 10]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	JMP LABEL20

LABEL19:
	MOV AX, [RBP - 12]
	CMP AX, 01
	JE LABEL23
	MOV RDI, printFalse
	JMP LABEL24
LABEL23:
	MOV RDI, printTrue
LABEL24:
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV AX, [RBP - 10]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 8]
	MOV RDI, printFormat
	MOVSX RSI, AX
	XOR RAX, RAX
	CALL printf

	JMP LABEL20

LABEL20:

	ADD RSP, 0

	JMP LABEL5

LABEL2:
	MOV RDI, inputIntArrPrompt
	MOV RSI, 7
	XOR RAX, RAX
	CALL printf

	MOV RDI, leftRange
	MOV RSI, 4
	XOR RAX, RAX
	CALL printf

	MOV RDI, rightRange
	MOV RSI, 10
	XOR RAX, RAX
	CALL printf
	SUB RSP, 18
	MOV RCX, 0
LABEL25:			;getting array
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
	CMP RCX, 14
	JNE LABEL25

	ADD RSP, 18
	MOV CX,4
	MOV [RBP - 4], CX		;for loop lower lim

LABEL26:
	MOV AX, 10
	CMP CX,AX
	JG LABEL27
	MOV BX, [RBP - 4]
	PUSH BX
	MOV AX, 14
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV AX, [RBP-6]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL28
	CALL boundERROR

LABEL28:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL29
	CALL boundERROR

LABEL29:
	MOV BX, 18
	MOV AX, [RBP-6]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV BX, [RBP + RBX]
	PUSH BX
	MOV AX, [RBP-4]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL30
	CALL boundERROR

LABEL30:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL31
	CALL boundERROR

LABEL31:
	MOV BX, 18
	MOV AX, [RBP-4]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV AX, [RBP + RBX]
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	MOV AX, [RBP-4]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL32
	CALL boundERROR

LABEL32:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL33
	CALL boundERROR

LABEL33:
	MOV BX, 18
	MOV AX, [RBP-4]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX

	POP AX
	MOV[RBP + RBX], AX

	MOV AX, [RBP-4]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL34
	CALL boundERROR

LABEL34:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL35
	CALL boundERROR

LABEL35:
	MOV BX, 18
	MOV AX, [RBP-4]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV BX, [RBP + RBX]
	PUSH BX
	MOV AX, 0
	PUSH AX
	POP AX
	POP BX
	SUB AX,BX
	PUSH AX
	MOV AX, [RBP-6]
	MOV BX, 4
	CMP AX, BX
	JGE LABEL36
	CALL boundERROR

LABEL36:
	MOV BX, 10
	CMP BX, AX
	JGE LABEL37
	CALL boundERROR

LABEL37:
	MOV BX, 18
	MOV AX, [RBP-6]
	SUB AX, 4
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX

	POP AX
	MOV[RBP + RBX], AX


	MOV CX, [RBP - 4]		;Ending increment
	INC CX
	MOV [RBP - 4],CX
	JMP LABEL26

LABEL27:
	MOV AX, [RBP - 30]

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RCX, 0
LABEL38:			;printing array
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
	CMP RCX, 14
	JNE LABEL38


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	JMP LABEL5

LABEL3:

	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 18
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 4], AX
	ADD RSP, 18


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 18
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 6], AX
	ADD RSP, 18


	JMP LABEL5

LABEL4:
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

LABEL5:

	ADD RSP, 24
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

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
