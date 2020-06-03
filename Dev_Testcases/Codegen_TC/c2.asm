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

		SUB RSP, 6											; making space for declaration

		SUB RSP, 4											; making space for declaration

		MOV AX, 1
		MOV [RBP - 14], AX										; store variable

		MOV AX, 5
		MOV [RBP - 8], AX										; store variable

		MOV AX, 9
		MOV [RBP - 10], AX										; store variable


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -4
		CALL @getValuePrimitive

		MOV BX, [RBP - 2]
		PUSH BX
		MOV AX, [RBP - 10]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, 2
		PUSH BX
		MOV AX, [RBP - 8]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 4]
		PUSH BX
		MOV BX, [RBP - 10]
		PUSH BX
		MOV AX, [RBP - 8]
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		MOV BX, [RBP - 10]
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

		MOV BX, [RBP - 14]
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
		MOV BX, [RBP - 10]
		PUSH BX
		MOV AX, [RBP - 8]
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
		MOV [RBP - 16], AX										; store variable

		MOV SI, [RBP - 6]
		CALL @printInteger

		MOV SI, [RBP - 14]
		CALL @printBoolean


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
		@printFormat : db "Output :  %d" , 10, 0
		@printTrue : db "Output : true" , 10, 0
		@printFalse : db "Output : false" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputInt : db "%d", 0
