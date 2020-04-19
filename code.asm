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

	SUB RSP, 36			;Updating RSP

	MOV AX, [RBP-2]
	CMP AX, 0
	JGE LABEL1
	CALL declNegERROR

LABEL1:
	MOV [RBP-30], AX
	MOV BX, [RBP-4]
	CMP BX, 0
	JGE LABEL2
	CALL declNegERROR

LABEL2:
	MOV [RBP-32], BX
	CMP BX, AX
	JGE LABEL3
	CALL declERROR

LABEL3:
	SUB BX, AX
	ADD BX, 1
	ADD BX, BX
	MOVSX RDI, BX

	MOV CX, 0
	MOV RBX, 40
	NEG RBX

LABEL4:
	CALL malloc
	MOV [RBP+RBX], RAX
	ADD RBX, 12
	INC CX
	CMP CX, 3
	JL LABEL4

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
	SUB RSP, 24
	MOV RCX, 0

LABEL5:			;getting array
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
	MOV RDI, [RBP-16]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-6]
	MOV BX, [RBP-8]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL5

	ADD RSP, 24

	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-18]
	MOV BX, [RBP-20]
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
	SUB RSP, 24
	MOV RCX, 0

LABEL6:			;getting array
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
	MOV RDI, [RBP-28]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-18]
	MOV BX, [RBP-20]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL6

	ADD RSP, 24

	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-30]
	MOV BX, [RBP-32]
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
	SUB RSP, 24
	MOV RCX, 0

LABEL7:			;getting array
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
	MOV RDI, [RBP-40]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-30]
	MOV BX, [RBP-32]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL7

	ADD RSP, 24

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL8:			;printing array
	MOV RDI, [RBP - 16]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	MOV RDI, printInt
	MOVSX RSI, AX
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-6]
	MOV BX, [RBP-8]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL8


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL9:			;printing array
	MOV RDI, [RBP - 28]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	MOV RDI, printInt
	MOVSX RSI, AX
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-18]
	MOV BX, [RBP-20]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL9


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL10:			;printing array
	MOV RDI, [RBP - 40]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	MOV RDI, printInt
	MOVSX RSI, AX
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-30]
	MOV BX, [RBP-32]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL10


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret
