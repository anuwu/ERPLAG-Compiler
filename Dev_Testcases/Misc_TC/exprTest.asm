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

		SUB RSP, 6											; making space for declaration

		MOV AX, 4
		MOV [RBP - 2], AX										; store variable

		MOV BX, 2
		PUSH BX
		MOV BX, 5
		PUSH BX
		MOV BX, 6
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		POP BX
		CMP BX, 0
		JNE @DIV_ZERO1
		CALL @divZeroERROR

	@DIV_ZERO1:
		CWD
		IDIV BX
		PUSH AX
		POP AX
		MOV [RBP - 4], AX										; store variable

		MOV SI, [RBP - 4]
		CALL @printInteger

		SUB RSP, 12											; making space for declaration

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV AX, [RBP - 2]
		MOV BX, [RBP - 4]
		CALL @dynamicDeclCheck										; checking dynamic array declaration limits


		PUSH BX												; saving register for malloc
		PUSH AX												; saving register for malloc
		SUB RSP, 12
		CALL malloc
		ADD RSP, 12
		MOV [RBP - 18], RAX
		POP SI
		MOV [RBP - 8], SI
		POP DI
		MOV [RBP - 10], DI


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		MOV BX, 3
		PUSH BX
		MOV BX, 9
		PUSH BX
		MOV BX, 4
		PUSH BX
		MOV AX, 6
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
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, 5
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV AX, 5
		MOV [RBP - 6], AX										; store variable

		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		NEG AX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, 5
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV BX, 2
		PUSH BX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, 5
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV BX, 2
		PUSH BX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		CMP BX, 0
		JNE @DIV_ZERO2
		CALL @divZeroERROR

	@DIV_ZERO2:
		CWD
		IDIV BX
		PUSH AX
		POP AX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		SUB RSP, 2											; making space for declaration

		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV BX, 1
		PUSH BX
		MOV BX, 0
		PUSH BX
		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, [RBP - 6]
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETG AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		POP BX
		AND AX, BX
		PUSH AX
		POP AX
		MOV [RBP - 20], AX										; store variable

		MOV RDI, [RBP - 18]
		MOV BX, [RBP - 8]
		MOV CX, 5
		MOV DX, [RBP - 10]
		CALL @boundCheck										; checking array index bound
		MOV SI, [RDI + RBX]
		CALL @printInteger

		MOV SI, [RBP - 20]
		CALL @printBoolean

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RDI, [RBP - 18]
		CALL free								; freeing local dynamic arrays


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

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

@divZeroERROR:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		MOV RDI, @divZero
		XOR RSI, RSI
		XOR RAX, RAX
		CALL printf
		MOV RDI, 1
		CALL exit

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

@printBoolean:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX
		CMP SI, 0
		JE .boolFalse
		MOV RDI, @printTrue
		JMP .boolPrint

	.boolFalse:
		MOV RDI, @printFalse

	.boolPrint:
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
		@divZero: db "[1m[31mRuntime Error [0m[1m--> [0mDivision by zero detected. Halt!" , 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@printTrue : db "Output : true" , 10, 0
		@printFalse : db "Output : false" , 10, 0
