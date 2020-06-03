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

		SUB RSP, 12											; making space for declaration

		SUB RSP, 2											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -14
		CALL @getValuePrimitive

		MOV AX, 4
		MOV RDI, RBP
		SUB RDI, 12
		MOV BX, 5
		MOV CX, [RBP - 14]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV RDI, RBP
		SUB RDI, 12
		MOV BX, 5
		MOV CX, [RBP - 14]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, RBP
		SUB RDI, 12
		MOV BX, 5
		MOV CX, [RBP - 14]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV RDI, RBP
		SUB RDI, 12
		MOV BX, 5
		MOV CX, [RBP - 14]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV RDI, RBP
		SUB RDI, 12
		MOV BX, 5
		MOV CX, [RBP - 14]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV SI, [RDI + RBX]
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

;--------------------------------------------------------------------------------------------------

section .data
		@boundPrint : db "[1m[31mRuntime Error [0m[1m--> [0mArray out of bounds. Halt!" , 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputInt : db "%d", 0
