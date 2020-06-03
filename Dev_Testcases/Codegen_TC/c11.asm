;;;;;;;;;;; Compiled for 64-bit Linux ;;;;;;;;;;;

extern printf
extern scanf
extern malloc
extern free
extern exit

global main
section .text

array_parameter_demo:
		PUSH RBP
		MOV RBP, RSP

		MOV RDI, [RBP + 16]
		MOV CX, 10
		MOV DX, 14
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV RDI, [RBP + 24]
		MOV CX, [RBP + 34]
		MOV DX, [RBP + 32]
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		SUB RSP, 12											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -6
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -8
		CALL @getValuePrimitive

		SUB RSP, 20											; making space for declaration

		SUB RSP, 12											; making space for declaration

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV AX, [RBP - 6]
		MOV BX, [RBP - 8]
		CALL @dynamicDeclCheck										; checking dynamic array declaration limits


		PUSH BX												; saving register for malloc
		PUSH AX												; saving register for malloc
		SUB RSP, 12
		CALL malloc
		ADD RSP, 12
		MOV [RBP - 44], RAX
		POP SI
		MOV [RBP - 34], SI
		POP DI
		MOV [RBP - 36], DI


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		MOV RDI, @inputIntArrPrompt
		MOV BX, 10
		MOV CX, 14
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, RBP
		SUB RDI, 22
		CALL @getArr

		MOV RDI, @inputIntArrPrompt
		MOV BX, [RBP - 34]
		MOV CX, [RBP - 36]
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, [RBP - 44]
		CALL @getArr

		MOV RDI, RBP
		SUB RDI, 22
		MOV CX, 10
		MOV DX, 14
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV AX, 0
		MOV [RBP + 36], AX										; store variable

		MOV CX,10
		MOV [RBP - 2], CX
	@FOR1:
		MOV AX, 14
		CMP CX, AX
		JG @FOR2

		MOV RDI, RBP
		SUB RDI, 22
		MOV BX, 10
		MOV CX, [RBP - 2]
		MOV DX, 14
		CALL @boundCheck										; checking array index bound
		MOV SI, [RDI + RBX]
		CALL @printInteger

		MOV RDI, RBP
		SUB RDI, 22
		MOV BX, 10
		MOV CX, [RBP - 2]
		MOV DX, 14
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV AX, [RBP + 36]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP + 36], AX										; store variable

		MOV SI, [RBP + 36]
		CALL @printInteger


		MOV CX, [RBP - 2]								; For loop increment
		INC CX
		MOV [RBP - 2],CX
		JMP @FOR1

	@FOR2:
		MOV RDI, RBP
		SUB RDI, 22
		MOV CX, 10
		MOV DX, 14
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV SI, [RBP + 36]
		CALL @printInteger

		MOV RDI, [RBP - 44]
		MOV BX, [RBP - 34]
		MOV CX, 7
		MOV DX, [RBP - 36]
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV RDI, [RBP - 44]
		MOV BX, [RBP - 34]
		MOV CX, 6
		MOV DX, [RBP - 36]
		CALL @boundCheck										; checking array index bound
		MOV AX, [RDI + RBX]

		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV RDI, [RBP - 44]
		MOV BX, [RBP - 34]
		MOV CX, 5
		MOV DX, [RBP - 36]
		CALL @boundCheck										; checking array index bound
		MOV [RDI + RBX], AX										; store variable

		MOV RDI, [RBP - 44]
		MOV CX, [RBP - 34]
		MOV DX, [RBP - 36]
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV AX, 0
		MOV [RBP + 38], AX										; store variable

		MOV CX, 10
		MOV DX, 14
		MOV RDI, RBP
		SUB RDI, 32
		MOV RSI, [RBP + 16]
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @asgnArr

		MOV RDI, RBP
		SUB RDI, 32
		MOV CX, 10
		MOV DX, 14
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV CX,10
		MOV [RBP - 4], CX
	@FOR3:
		MOV AX, 14
		CMP CX, AX
		JG @FOR4

		MOV RDI, RBP
		SUB RDI, 32
		MOV BX, 10
		MOV CX, [RBP - 4]
		MOV DX, 14
		CALL @boundCheck										; checking array index bound
		MOV SI, [RDI + RBX]
		CALL @printInteger

		MOV RDI, RBP
		SUB RDI, 32
		MOV BX, 10
		MOV CX, [RBP - 4]
		MOV DX, 14
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV AX, [RBP + 38]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP + 38], AX										; store variable

		MOV SI, [RBP + 38]
		CALL @printInteger


		MOV CX, [RBP - 4]								; For loop increment
		INC CX
		MOV [RBP - 4],CX
		JMP @FOR3

	@FOR4:
		MOV RDI, RBP
		SUB RDI, 32
		MOV CX, 10
		MOV DX, 14
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV SI, [RBP + 38]
		CALL @printInteger

		MOV AX, 0
		MOV [RBP + 40], AX										; store variable

		MOV RDI, [RBP + 24]
		MOV CX, [RBP + 34]
		MOV DX, [RBP + 32]
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr

		MOV CX,1
		MOV [RBP - 10], CX
	@FOR5:
		MOV AX, 4
		CMP CX, AX
		JG @FOR6


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -12
		CALL @getValuePrimitive

		MOV RDI, [RBP + 24]
		MOV BX, [RBP + 34]
		MOV CX, [RBP - 12]
		MOV DX, [RBP + 32]
		CALL @boundCheck										; checking array index bound
		MOV BX, [RDI + RBX]

		PUSH BX
		MOV AX, [RBP + 40]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP + 40], AX										; store variable

		MOV SI, [RBP + 40]
		CALL @printInteger


		MOV CX, [RBP - 10]								; For loop increment
		INC CX
		MOV [RBP - 10],CX
		JMP @FOR5

	@FOR6:
		MOV SI, [RBP + 40]
		CALL @printInteger

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RDI, [RBP - 44]
		CALL free								; freeing local dynamic arrays


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		MOV RSP, RBP
		POP RBP
		ret

;--------------------------------------------------------------------------------------------------

main:
		PUSH RBP
		MOV RBP, RSP

		SUB RSP, 4											; making space for declaration

		SUB RSP, 6											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -4
		CALL @getValuePrimitive

		SUB RSP, 10											; making space for declaration

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
		MOV [RBP - 32], RAX
		POP SI
		MOV [RBP - 22], SI
		POP DI
		MOV [RBP - 24], DI


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		MOV RDI, @inputIntArrPrompt
		MOV BX, 10
		MOV CX, 14
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, RBP
		SUB RDI, 20
		CALL @getArr

		MOV RDI, @inputIntArrPrompt
		MOV BX, [RBP - 22]
		MOV CX, [RBP - 24]
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, [RBP - 32]
		CALL @getArr

		SUB RSP, 6
		MOV AX, [RBP - 22]
		PUSH AX
		MOV BX, [RBP - 24]
		PUSH BX
		MOV RAX, [RBP - 32]
		PUSH RAX
		MOV RAX, RBP
		SUB RAX, 20
		PUSH RAX
		CALL array_parameter_demo											; calling user function
		ADD RSP, 20
		POP AX
		MOV [RBP - 6], AX
		POP AX
		MOV [RBP - 8], AX
		POP AX
		MOV [RBP - 10], AX

		MOV SI, [RBP - 6]
		CALL @printInteger

		MOV SI, [RBP - 8]
		CALL @printInteger

		MOV SI, [RBP - 10]
		CALL @printInteger

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RDI, [RBP - 32]
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

@asgnArr:
		MOV CX, 0

	.assnLoop:
		MOVSX RBX, CX
		MOV AX, [RSI + RBX]
		MOV [RDI + RBX], AX

		ADD CX, 2
		CMP CX, DX
		JNE .assnLoop

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
