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


	MOV RDI, inputBoolPrompt		;get_value
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

	SUB RSP, 24			;Updating RSP


	MOV AX, [RBP - 2]

	CMP AX, 1
	JE LABEL1

	CMP AX, 0
	JE LABEL2

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

	JMP LABEL3

LABEL2:

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

	MOV BX, 9
	PUSH BX
	POP AX
	MOV [rbp - 6],AX		;Store

	MOV AX, [RBP-4]
	CMP AX, 0
	JGE LABEL4
	CALL declNegERROR

LABEL4:
	MOV BX, [RBP-6]
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
	MOV RBX, 30
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

	MOV RDI, inputIntArrPrompt
	MOV AX, [RBP-8]
	MOV BX, [RBP-10]
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
	SUB RSP, 18
	MOV RCX, 0

LABEL8:			;getting array
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
	MOV RDI, [RBP-18]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-8]
	MOV BX, [RBP-10]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL8

	ADD RSP, 18
	
LABEL9:
	MOV AX, [rbp - 4]
	PUSH AX
	MOV BX, [RBP - 6]
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JLE LABEL11
	MOV AX,0
	PUSH AX
	JMP LABEL12

LABEL11:

	MOV AX,1
	PUSH AX


LABEL12:

	POP AX
	CMP AX,00000001h
	JNE LABEL10
	SUB RSP, 2			;Updating RSP


	MOV RDI, inputIntPrompt		;get_value
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RDI, inputInt		;get_value
	SUB RSP, 32
	MOV RSI, RSP
	PUSH RSI
	CALL scanf
	POP RSI
	MOV AX, WORD [RSP]
	MOV [RBP - 32], AX
	ADD RSP, 32

	MOV BX, [RBP - 32]
	PUSH BX
	MOV AX, [RBP-4]
	MOV BX, [RBP-10]
	CMP BX, AX
	JGE LABEL13
	CALL boundERROR

LABEL13:
	MOV BX, [RBP-8]
	CMP AX, BX
	JGE LABEL14
	CALL boundERROR

LABEL14:
	MOV RDI, [RBP-18]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX
	MOV AX, [RDI + RBX]
	PUSH AX
	POP AX
	POP BX
	IMUL BX
	PUSH AX
	MOV AX, [RBP-4]
	MOV BX, [RBP-22]
	CMP BX, AX
	JGE LABEL15
	CALL boundERROR

LABEL15:
	MOV BX, [RBP-20]
	CMP AX, BX
	JGE LABEL16
	CALL boundERROR

LABEL16:
	MOV RDI, [RBP-30]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX

	POP AX
	MOV[RDI + RBX], AX

	MOV AX, [rbp - 4]
	PUSH AX
	MOV BX, 1
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 4],AX		;Store

	ADD RSP, 2			;Restoring RSP to previous scope
	JMP LABEL9

LABEL10:
	MOV AX, [RBP - 30]

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL17:			;printing array
	MOV RDI, [RBP - 30]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	MOV RDI, printInt
	MOVSX RSI, AX
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-20]
	MOV BX, [RBP-22]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL17


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	JMP LABEL3

LABEL3:

	ADD RSP, 28
	SUB RSP, 20			;Updating RSP

	MOV RDI, inputBoolArrPrompt
	MOV RSI, 10
	XOR RAX, RAX
	CALL printf

	MOV RDI, leftRange
	MOV RSI, 0
	XOR RAX, RAX
	CALL printf

	MOV RDI, rightRange
	MOV RSI, 9
	XOR RAX, RAX
	CALL printf
	SUB RSP, 26
	MOV RCX, 0
LABEL18:			;getting array
	MOV RBX, 4
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
	CMP RCX, 20
	JNE LABEL18

	ADD RSP, 26
	MOV AX, [RBP - 22]

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RCX, 0
LABEL19:			;printing array
	MOV RBX, 4
	ADD RBX, RCX

	NEG RBX
	MOV AX, [RBP + RBX]
	XOR RSI, RSI

	CMP AX, 01
	JE LABEL20
	MOV RDI, false
	JMP LABEL21
LABEL20:
	MOV RDI, true

LABEL21:
	XOR RAX, RAX
	PUSH RCX
	PUSH RBX
	CALL printf
	POP RBX
	POP RCX

	ADD RCX, 2
	CMP RCX, 20
	JNE LABEL19


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	SUB RSP, 6			;Updating RSP

	SUB RSP, 6			;Updating RSP

	MOV BX, 0
	PUSH BX
	POP AX
	MOV [rbp - 32],AX		;Store

	MOV BX, 10
	PUSH BX
	POP AX
	MOV [rbp - 34],AX		;Store

	MOV CX,0
	MOV [RBP - 30], CX		;for loop lower lim

LABEL22:
	MOV AX, 9
	CMP CX,AX
	JG LABEL23
	MOV AX, [RBP-30]
	MOV BX, 0
	CMP AX, BX
	JGE LABEL24
	CALL boundERROR

LABEL24:
	MOV BX, 9
	CMP BX, AX
	JGE LABEL25
	CALL boundERROR

LABEL25:
	MOV BX, 4
	MOV AX, [RBP-30]
	SUB AX, 0
	ADD AX, AX
	ADD BX, AX
	NEG BX
	MOVSX RBX, BX
	MOV AX, [RBP + RBX]
	PUSH AX
	POP AX
	MOV [rbp - 24],AX		;Store


	MOV AX, [RBP - 24]

	CMP AX, 1
	JE LABEL26

	CMP AX, 0
	JE LABEL27

LABEL26:
	MOV AX, [rbp - 32]
	PUSH AX
	MOV BX, 1
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 32],AX		;Store


	JMP LABEL28

LABEL27:

	JMP LABEL28

LABEL28:

	ADD RSP, 0

	MOV CX, [RBP - 30]		;Ending increment
	INC CX
	MOV [RBP - 30],CX
	JMP LABEL22

LABEL23:
	SUB RSP, 24			;Updating RSP

	MOV AX, [RBP-32]
	CMP AX, 0
	JGE LABEL29
	CALL declNegERROR

LABEL29:
	MOV BX, [RBP-34]
	CMP BX, 0
	JGE LABEL30
	CALL declNegERROR

LABEL30:
	CMP BX, AX
	JGE LABEL31
	CALL declERROR

LABEL31:
	PUSH BX
	PUSH AX
	SUB BX, AX
	ADD BX, 1
	ADD BX, BX
	MOVSX RDI, BX

	MOV CX, 0
	MOV RBX, 58
	NEG RBX

LABEL32:
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
	JL LABEL32
	ADD RSP, 4

	MOV RDI, inputBoolArrPrompt
	MOV AX, [RBP-36]
	MOV BX, [RBP-38]
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
	SUB RSP, 22
	MOV RCX, 0

LABEL33:			;getting array
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
	MOV RDI, [RBP-46]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-36]
	MOV BX, [RBP-38]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL33

	ADD RSP, 22

	MOV RDI, inputBoolArrPrompt
	MOV AX, [RBP-48]
	MOV BX, [RBP-50]
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
	SUB RSP, 22
	MOV RCX, 0

LABEL34:			;getting array
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
	MOV RDI, [RBP-58]
	MOV [RDI + RBX], AX
	ADD RCX, 2
	MOV AX, [RBP-48]
	MOV BX, [RBP-50]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	MOVSX RDX, DX
	CMP RCX, RDX
	JNE LABEL34

	ADD RSP, 22
	MOV AX, [rbp - 32]
	PUSH AX
	POP AX
	MOV [rbp - 30],AX		;Store

	
LABEL35:
	MOV AX, [rbp - 30]
	PUSH AX
	MOV BX, [RBP - 34]
	PUSH BX
	POP BX
	POP AX
	CMP AX,BX

	JLE LABEL37
	MOV AX,0
	PUSH AX
	JMP LABEL38

LABEL37:

	MOV AX,1
	PUSH AX


LABEL38:

	POP AX
	CMP AX,00000001h
	JNE LABEL36
	MOV AX, [RBP-30]
	MOV BX, [RBP-38]
	CMP BX, AX
	JGE LABEL39
	CALL boundERROR

LABEL39:
	MOV BX, [RBP-36]
	CMP AX, BX
	JGE LABEL40
	CALL boundERROR

LABEL40:
	MOV RDI, [RBP-46]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX
	MOV AX, [RDI + RBX]
	PUSH AX
	MOV AX, [RBP-30]
	MOV BX, [RBP-50]
	CMP BX, AX
	JGE LABEL41
	CALL boundERROR

LABEL41:
	MOV BX, [RBP-48]
	CMP AX, BX
	JGE LABEL42
	CALL boundERROR

LABEL42:
	MOV RDI, [RBP-58]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX
	MOV BX, [RDI + RBX]
	PUSH BX
	POP BX
	POP AX
	CMP AX,00000001h
	JE LABEL43
	MOV AX,00000000h
	PUSH AX
	JMP LABEL45
	

LABEL43:
	CMP BX,00000001h
	JNE LABEL44
	MOV AX,00000001h
	PUSH AX
	JMP LABEL45
LABEL44:
	MOV AX,00000000h
	PUSH AX
LABEL45:
	POP AX
	MOV [rbp - 26],AX		;Store

	MOV AX, [RBP-30]
	MOV BX, [RBP-38]
	CMP BX, AX
	JGE LABEL46
	CALL boundERROR

LABEL46:
	MOV BX, [RBP-36]
	CMP AX, BX
	JGE LABEL47
	CALL boundERROR

LABEL47:
	MOV RDI, [RBP-46]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX
	MOV AX, [RDI + RBX]
	PUSH AX
	MOV AX, [RBP-30]
	MOV BX, [RBP-50]
	CMP BX, AX
	JGE LABEL48
	CALL boundERROR

LABEL48:
	MOV BX, [RBP-48]
	CMP AX, BX
	JGE LABEL49
	CALL boundERROR

LABEL49:
	MOV RDI, [RBP-58]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX
	MOV BX, [RDI + RBX]
	PUSH BX
	POP BX
	POP AX
	CMP AX,00000001h
	JNE LABEL50
	MOV AX,00000001h
	PUSH AX
	JMP LABEL52
	

LABEL50:
	CMP BX,00000001h
	JNE LABEL51
	MOV AX,00000001h
	PUSH AX
	JMP LABEL52
LABEL51:
	MOV AX,00000000h
	PUSH AX
LABEL52:
	POP AX
	MOV [rbp - 28],AX		;Store

	MOV AX, [rbp - 26]
	PUSH AX
	MOV AX, [RBP-30]
	MOV BX, [RBP-38]
	CMP BX, AX
	JGE LABEL53
	CALL boundERROR

LABEL53:
	MOV BX, [RBP-36]
	CMP AX, BX
	JGE LABEL54
	CALL boundERROR

LABEL54:
	MOV RDI, [RBP-46]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX

	POP AX
	MOV[RDI + RBX], AX

	MOV AX, [rbp - 28]
	PUSH AX
	MOV AX, [RBP-30]
	MOV BX, [RBP-50]
	CMP BX, AX
	JGE LABEL55
	CALL boundERROR

LABEL55:
	MOV BX, [RBP-48]
	CMP AX, BX
	JGE LABEL56
	CALL boundERROR

LABEL56:
	MOV RDI, [RBP-58]
	SUB AX, BX
	ADD AX, AX
	MOVSX RBX, AX

	POP AX
	MOV[RDI + RBX], AX

	MOV AX, [rbp - 30]
	PUSH AX
	MOV BX, 1
	PUSH BX
	POP AX
	POP BX
	ADD AX,BX
	PUSH AX
	POP AX
	MOV [rbp - 30],AX		;Store

	JMP LABEL35

LABEL36:
	MOV AX, [RBP - 46]

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL57:			;printing array
	MOV RDI, [RBP - 46]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	XOR RSI, RSI

	CMP AX, 01
	JE LABEL58
	MOV RDI, false
	JMP LABEL59
LABEL58:
	MOV RDI, true

LABEL59:
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-36]
	MOV BX, [RBP-38]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL57


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf
	MOV AX, [RBP - 58]

	MOV RDI, printFormatArray		;printing array output prompt
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV CX, 0
LABEL60:			;printing array
	MOV RDI, [RBP - 58]
	MOVSX RBX, CX
	MOV AX, [RDI + RBX]
	XOR RSI, RSI

	CMP AX, 01
	JE LABEL61
	MOV RDI, false
	JMP LABEL62
LABEL61:
	MOV RDI, true

LABEL62:
	XOR RAX, RAX
	PUSH CX
	CALL printf
	POP CX

	ADD CX, 2
	MOV AX, [RBP-48]
	MOV BX, [RBP-50]
	MOV DX, BX
	SUB DX, AX
	ADD DX, 1
	ADD DX, DX
	CMP CX, DX
	JNE LABEL60


	MOV RDI, printNewLine		; newline after array print
	XOR RSI, RSI
	XOR RAX, RAX
	CALL printf

	MOV RSP, RBP
	POP RBP
	POP RBP
	ret