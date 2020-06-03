;;;;;;;;;;; Compiled for 64-bit Linux ;;;;;;;;;;;

extern printf
extern scanf
extern malloc
extern free
extern exit

global main
section .text

main:
		PUSH RBP
		MOV RBP, RSP

		SUB RSP, 8											; making space for declaration

		SUB RSP, 4											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive

		MOV AX, 10
		MOV [RBP - 4], AX										; store variable

		MOV BX, [RBP - 4]
		PUSH BX
		MOV AX, [RBP - 4]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV [RBP - 4], AX										; store variable

		MOV SI, [RBP - 4]
		CALL @printInteger

		MOV AX, 10
		MOV [RBP - 4], AX										; store variable

		MOV AX, [RBP - 2]
		MOV [RBP - 8], AX										; store variable

		MOV AX, 0
		MOV [RBP - 6], AX										; store variable


	@WHILE1:
		MOV BX, [RBP - 4]
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETLE AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		CMP AX, 0											; checking while loop condition
		JE @WHILE2

		SUB RSP, 2											; making space for declaration

		MOV BX, 1
		PUSH BX
		MOV AX, [RBP - 6]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 6], AX										; store variable

		MOV BX, [RBP - 4]
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETE AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		MOV [RBP - 12], AX										; store variable

		MOV BX, 5
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETE AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		MOV [RBP - 10], AX										; store variable

		SUB RSP, 2											; declaring before switch

		SUB RSP, 12											; declaring before switch

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV AX, 1
		MOV BX, [RBP - 6]
		CALL @dynamicDeclCheck										; checking dynamic array declaration limits


		PUSH BX												; saving register for malloc
		PUSH AX												; saving register for malloc
		SUB RSP, 12
		CALL malloc
		ADD RSP, 12
		MOV [RBP - 28], RAX
		POP SI
		MOV [RBP - 18], SI
		POP DI
		MOV [RBP - 20], DI


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		MOV AX, [RBP - 10]										; loading switch variable

		CMP AX, 0
		JNE @SWITCH1											; true case
		JMP @SWITCH2											; false case

@SWITCH1:
		MOV RDI, @inputIntArrPrompt
		MOV BX, 1
		MOV CX, [RBP - 20]
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, [RBP - 28]
		CALL @getArr

		MOV AX, 1
		MOV [RBP - 12], AX										; store variable


		JMP @SWITCH3

@SWITCH2:
		SUB RSP, 2											; declaring before switch


		MOV AX, [RBP - 12]										; loading switch variable

		CMP AX, 0
		JNE @SWITCH4											; true case
		JMP @SWITCH5											; false case

@SWITCH4:
		MOV AX, 1
		MOV [RBP - 30], AX										; store variable


	@WHILE3:
		MOV BX, [RBP - 16]
		PUSH BX
		MOV AX, [RBP - 30]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETLE AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		CMP AX, 0											; checking while loop condition
		JE @WHILE4

		MOV RDI, [RBP - 28]
		MOV BX, 1
		MOV CX, [RBP - 30]
		MOV DX, [RBP - 20]
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, [RBP - 28]
		MOV BX, 1
		MOV CX, [RBP - 30]
		MOV DX, [RBP - 20]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV RDI, [RBP - 28]
		MOV BX, 1
		MOV CX, [RBP - 30]
		MOV DX, [RBP - 20]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV BX, 1
		PUSH BX
		MOV AX, [RBP - 30]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 30], AX										; store variable

		JMP @WHILE3

	@WHILE4:
		MOV RDI, [RBP - 28]
		MOV CX, 1
		MOV DX, [RBP - 20]
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr


		JMP @SWITCH6

@SWITCH5:

		JMP @SWITCH6

@SWITCH6:
		ADD RSP, 2											; restoring to parent scope

		JMP @SWITCH3

@SWITCH3:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RDI, [RBP - 28]
		CALL free								; freeing local dynamic arrays


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		ADD RSP, 14											; restoring to parent scope
		MOV BX, 1
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 2], AX										; store variable


		ADD RSP, 2										; restoring to parent scope
		JMP @WHILE1

	@WHILE2:
		MOV SI, [RBP - 6]
		CALL @printInteger


		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

@boundCheck:
		CMP CX, BX
		JGE .leftLim
		CALL @boundERROR

	.leftLim:
		CMP DX, CX
		JGE .rightLim
		CALL @boundERROR

	.rightLim:
		SUB CX, BX
		ADD CX, CX
		MOVSX RBX, CX

		ret

@dynamicDeclCheck:
		CMP AX, 0
		JGE .leftNotNeg
		CALL @declNegERROR

	.leftNotNeg:
		CMP BX, 0
		JGE .rightNotNeg
		CALL @declNegERROR

	.rightNotNeg:
		CMP BX, AX
		JGE .dynChecked
		CALL @declERROR

	.dynChecked:
		MOV DX, BX
		SUB DX, AX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDI, DX

		ret

@boundERROR:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV RDI, @boundPrint
		XOR RSI, RSI
		XOR RAX, RAX
		CALL printf
		MOV RDI, 1
		CALL exit

@declERROR:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV RDI, @declPrint
		XOR RSI, RSI
		XOR RAX, RAX
		CALL printf
		MOV RDI, 1
		CALL exit

@declNegERROR:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV RDI, @declNeg
		XOR RSI, RSI
		XOR RAX, RAX
		CALL printf
		MOV RDI, 1
		CALL exit

@getValuePrimitive:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		XOR RSI, RSI
		XOR RAX, RAX
		PUSH RBX
		PUSH RCX
		CALL printf
		POP RCX
		POP RBX

		MOV RDI, @inputInt										; get_value
		MOV RSI, RSP
		SUB RSI, 16
		PUSH RBX
		PUSH RSI
		CALL scanf
		POP RSI
		POP RBX
		MOV AX, [RSP - 16]
		MOV [RBP + RBX], AX

		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		ret

@printGetArrPrompt:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV SI, CX
		SUB SI, BX
		ADD SI, 1
		MOVSX RSI, SI
		XOR RAX, RAX
		PUSH RSI
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		CALL printf
		POP DX
		POP CX
		POP BX
		POP AX
		POP RSI

		MOV RDI, @leftRange
		MOVSX RSI, BX
		XOR RAX, RAX
		PUSH RSI
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		CALL printf
		POP DX
		POP CX
		POP BX
		POP AX
		POP RSI

		MOV RDI, @rightRange
		MOVSX RSI, CX
		XOR RAX, RAX
		PUSH RSI
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		CALL printf
		POP DX
		POP CX
		POP BX
		POP AX
		POP RSI

		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		ret

@getArr:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		PUSH RDI
		MOV RCX, 0

	.getArrLoop:								; getting array
		MOV RDI, @inputInt
		MOV RSI, RSP
		SUB RSI, 24
		PUSH RCX
		PUSH RDX
		PUSH RSI
		CALL scanf
		POP RSI
		POP RDX
		POP RCX
		MOV RBX, RCX
		MOV AX, [RSP - 24]
		POP RDI
		PUSH RDI
		MOV [RDI + RBX], AX

		ADD RCX, 2
		CMP RCX, RDX
		JNE .getArrLoop

		POP RDI
		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		ret

@printInteger:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV RDI, @printFormat
		MOVSX RSI, SI
		XOR RAX, RAX
		CALL printf

		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		ret

@printIntegerArr:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		PUSH RDI
		MOV RDI, @printFormatArray
		XOR RSI, RSI
		XOR RAX, RAX
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		CALL printf
		POP DX
		POP CX
		POP BX
		POP AX
		POP RDI

		MOV CX, 0

	.printArr:								; printing array
		PUSH RDI
		MOVSX RBX, CX
		MOV SI, [RDI + RBX]
		MOV RDI, @printInt
		MOVSX RSI, SI
		XOR RAX, RAX
		PUSH AX
		PUSH BX
		PUSH CX
		PUSH DX
		CALL printf
		POP DX
		POP CX
		POP BX
		POP AX
		POP RDI

		ADD CX, 2
		CMP CX, DX
		JNE .printArr

		MOV RDI, @printNewLine
		XOR RSI, RSI
		XOR RAX, RAX
		CALL printf

		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		ret

;--------------------------------------------------------------------------------------------------

section .data
		@boundPrint : db "[1m[31mRuntime Error [0m[1m--> [0mArray out of bounds. Halt!" , 10, 0
		@declPrint : db "[1m[31mRuntime Error [0m[1m--> [0mInvalid order of bounds in dynamic array declaration. Halt!" , 10, 0
		@declNeg : db "[1m[31mRuntime Error [0m[1m--> [0mNegative bound in dynamic array declaration. Halt!" , 10, 0
		@printFormatArray : db "Output : " , 0
		@printInt : db "%d ", 0
		@printNewLine : db 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputIntArrPrompt : db "Enter %d array elements of integer type for range ", 0
		@leftRange : db "%d to " , 0
		@rightRange : db "%d" ,10, 0
		@inputInt : db "%d", 0
