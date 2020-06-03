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

		SUB RSP, 6											; making space for declaration

		SUB RSP, 4											; making space for declaration

		SUB RSP, 12											; making space for declaration

		MOV AX, 1
		MOV [RBP - 16], AX										; store variable

		MOV AX, 5
		MOV [RBP - 10], AX										; store variable

		MOV AX, 9
		MOV [RBP - 12], AX										; store variable


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -4
		CALL @getValuePrimitive

		MOV BX, [RBP - 2]
		PUSH BX
		MOV AX, [RBP - 12]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, 2
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 4]
		PUSH BX
		MOV BX, [RBP - 12]
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 12]
		PUSH BX
		MOV AX, [RBP - 4]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
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
		MOV [RBP - 6], AX										; store variable

		MOV SI, [RBP - 6]
		CALL @printInteger

		MOV CX,10
		MOV [RBP - 8], CX
	@FOR1:
		MOV AX, 15
		CMP CX, AX
		JG @FOR2

		SUB RSP, 4											; making space for declaration

		MOV AX, 7
		MOV [RBP - 32], AX										; store variable


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -34
		CALL @getValuePrimitive

		MOV BX, [RBP - 34]
		PUSH BX
		MOV AX, [RBP - 32]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 32], AX										; store variable

		MOV SI, [RBP - 32]
		CALL @printInteger

		MOV BX, 2
		PUSH BX
		MOV AX, [RBP - 32]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 8]
		PUSH BX
		MOV AX, [RBP - 6]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 6], AX										; store variable

		MOV SI, [RBP - 6]
		CALL @printInteger


		ADD RSP, 4										; restoring to parent scope

		MOV CX, [RBP - 8]								; For loop increment
		INC CX
		MOV [RBP - 8],CX
		JMP @FOR1

	@FOR2:
		MOV BX, [RBP - 2]
		PUSH BX
		MOV AX, [RBP - 12]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, 2
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 4]
		PUSH BX
		MOV BX, [RBP - 12]
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 12]
		PUSH BX
		MOV AX, [RBP - 4]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
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
		MOV [RBP - 6], AX										; store variable

		MOV BX, [RBP - 16]
		PUSH BX
		MOV BX, [RBP - 4]
		PUSH BX
		MOV AX, [RBP - 2]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETL AL
		MOVSX AX, AL
		PUSH AX
		MOV BX, [RBP - 12]
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETLE AL
		MOVSX AX, AL
		PUSH AX
		MOV BX, 10
		PUSH BX
		MOV AX, [RBP - 6]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETG AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		POP BX
		OR AX, BX
		PUSH AX
		POP AX
		POP BX
		AND AX, BX
		PUSH AX
		POP AX
		POP BX
		AND AX, BX
		PUSH AX
		POP AX
		MOV [RBP - 18], AX										; store variable

		MOV SI, [RBP - 6]
		CALL @printInteger

		MOV SI, [RBP - 16]
		CALL @printBoolean

		MOV RDI, @inputIntArrPrompt
		MOV BX, 10
		MOV CX, 15
		CALL @printGetArrPrompt

		MOV DX, CX
		SUB DX, BX
		ADD DX, 1
		ADD DX, DX
		MOVSX RDX, DX
		MOV RDI, RBP
		SUB RDI, 30
		CALL @getArr

		MOV BX, [RBP - 20]
		PUSH BX
		MOV BX, 3
		PUSH BX
		MOV AX, [RBP - 22]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, 2
		PUSH BX
		MOV AX, [RBP - 24]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 2]
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 4], AX										; store variable

		MOV SI, [RBP - 4]
		CALL @printInteger

		MOV RDI, RBP
		SUB RDI, 30
		MOV CX, 10
		MOV DX, 15
		SUB DX, CX
		ADD DX, 1
		ADD DX, DX
		CALL @printIntegerArr


		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

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
		@printFormatArray : db "Output : " , 0
		@printInt : db "%d ", 0
		@printNewLine : db 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@printTrue : db "Output : true" , 10, 0
		@printFalse : db "Output : false" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputIntArrPrompt : db "Enter %d array elements of integer type for range ", 0
		@leftRange : db "%d to " , 0
		@rightRange : db "%d" ,10, 0
		@inputInt : db "%d", 0
