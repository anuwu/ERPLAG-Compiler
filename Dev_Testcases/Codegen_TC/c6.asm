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

		SUB RSP, 14											; making space for declaration

		SUB RSP, 30											; making space for declaration

		MOV RDI, @inputIntArrPrompt
		MOV BX, 6
		MOV CX, 10
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, RBP
		SUB RDI, 24
		CALL @getArr

		MOV RDI, @inputIntArrPrompt
		MOV BX, 6
		MOV CX, 10
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, RBP
		SUB RDI, 34
		CALL @getArr

		MOV AX, 7
		MOV [RBP - 6], AX										; store variable

		MOV CX,1
		MOV [RBP - 8], CX
	@FOR1:
		MOV AX, 3
		CMP CX, AX
		JG @FOR2


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -4
		CALL @getValuePrimitive

		MOV RDI, RBP
		SUB RDI, 34
		MOV BX, 6
		MOV CX, [RBP - 4]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, RBP
		SUB RDI, 24
		MOV BX, 6
		MOV CX, [RBP - 2]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 10], AX										; store variable

		MOV RDI, RBP
		SUB RDI, 34
		MOV BX, 6
		MOV CX, [RBP - 4]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, RBP
		SUB RDI, 24
		MOV BX, 6
		MOV CX, [RBP - 6]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 12], AX										; store variable

		MOV RDI, RBP
		SUB RDI, 34
		MOV BX, 6
		MOV CX, [RBP - 6]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, RBP
		SUB RDI, 24
		MOV BX, 6
		MOV CX, [RBP - 2]
		MOV DX, 10
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 14], AX										; store variable

		MOV SI, [RBP - 10]
		CALL @printInteger

		MOV SI, [RBP - 12]
		CALL @printInteger

		MOV SI, [RBP - 14]
		CALL @printInteger


		MOV CX, [RBP - 8]								; For loop increment
		INC CX
		MOV [RBP - 8],CX
		JMP @FOR1

	@FOR2:

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

;--------------------------------------------------------------------------------------------------

section .data
		@boundPrint : db "[1m[31mRuntime Error [0m[1m--> [0mArray out of bounds. Halt!" , 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputIntArrPrompt : db "Enter %d array elements of integer type for range ", 0
		@leftRange : db "%d to " , 0
		@rightRange : db "%d" ,10, 0
		@inputInt : db "%d", 0
