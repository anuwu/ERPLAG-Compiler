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

		SUB RSP, 4											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive

		SUB RSP, 2
		MOV AX, [RBP - 2]
		PUSH AX
		CALL factorial											; calling user function
		ADD RSP, 2
		POP AX
		MOV [RBP - 4], AX

		MOV SI, [RBP - 4]
		CALL @printInteger

		SUB RSP, 4											; making space for declaration

		SUB RSP, 2											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -6
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -8
		CALL @getValuePrimitive

		SUB RSP, 4
		MOV AX, [RBP - 8]
		PUSH AX
		MOV AX, [RBP - 6]
		PUSH AX
		CALL nCr											; calling user function
		ADD RSP, 4
		POP AX
		MOV [RBP - 4], AX
		POP AX
		MOV [RBP - 10], AX

		MOV SI, [RBP - 10]
		CALL @printBoolean

		MOV SI, [RBP - 4]
		CALL @printInteger


		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

nCr:
		PUSH RBP
		MOV RBP, RSP

		MOV BX, [RBP + 18]
		PUSH BX
		MOV AX, [RBP + 16]
		PUSH AX
		POP AX
		POP BX
		CMP AX,BX
		SETGE AL
		MOVSX AX, AL
		PUSH AX
		POP AX
		MOV [RBP + 22], AX										; store variable

		SUB RSP, 6											; declaring before switch


		MOV AX, [RBP + 22]										; loading switch variable

		CMP AX, 0
		JNE @SWITCH1											; true case
		JMP @SWITCH2											; false case

@SWITCH1:
		SUB RSP, 2
		MOV AX, [RBP + 16]
		PUSH AX
		CALL factorial											; calling user function
		ADD RSP, 2
		POP AX
		MOV [RBP - 2], AX

		MOV BX, [RBP + 18]
		PUSH BX
		MOV AX, [RBP + 16]
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 4], AX										; store variable

		SUB RSP, 2
		MOV AX, [RBP - 4]
		PUSH AX
		CALL factorial											; calling user function
		ADD RSP, 2
		POP AX
		MOV [RBP - 4], AX

		SUB RSP, 2
		MOV AX, [RBP + 18]
		PUSH AX
		CALL factorial											; calling user function
		ADD RSP, 2
		POP AX
		MOV [RBP - 6], AX

		MOV BX, [RBP - 6]
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
		CMP BX, 0
		JNE @DIV_ZERO1
		CALL @divZeroERROR

	@DIV_ZERO1:
		CWD
		IDIV BX
		PUSH AX
		POP AX
		MOV [RBP + 20], AX										; store variable


		JMP @SWITCH3

@SWITCH2:
		MOV AX, 1
		NEG AX
		MOV [RBP + 20], AX										; store variable


		JMP @SWITCH3

@SWITCH3:
		ADD RSP, 6											; restoring to parent scope

		MOV RSP, RBP
		POP RBP
		ret

;--------------------------------------------------------------------------------------------------

factorial:
		PUSH RBP
		MOV RBP, RSP

		SUB RSP, 4											; declaring before switch


		MOV AX, [RBP + 16]										; loading switch variable

		CMP AX, 0											; switch case
		JE @SWITCH4

		JMP @SWITCH5											; default case

@SWITCH4:
		MOV AX, 1
		MOV [RBP + 18], AX										; store variable


		JMP @SWITCH6

@SWITCH5:
		MOV BX, 1
		PUSH BX
		MOV AX, [RBP + 16]
		PUSH AX
		POP AX
		POP BX
		SUB AX,BX
		PUSH AX
		POP AX
		MOV [RBP - 4], AX										; store variable

		SUB RSP, 2
		MOV AX, [RBP - 4]
		PUSH AX
		CALL factorial											; calling user function
		ADD RSP, 2
		POP AX
		MOV [RBP - 2], AX

		MOV BX, [RBP - 2]
		PUSH BX
		MOV AX, [RBP + 16]
		PUSH AX
		POP AX
		POP BX
		IMUL BX
		PUSH AX
		POP AX
		MOV [RBP + 18], AX										; store variable


		JMP @SWITCH6

@SWITCH6:
		ADD RSP, 4											; restoring to parent scope

		MOV RSP, RBP
		POP RBP
		ret

;--------------------------------------------------------------------------------------------------

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
		@divZero: db "[1m[31mRuntime Error [0m[1m--> [0mDivision by zero detected. Halt!" , 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@printTrue : db "Output : true" , 10, 0
		@printFalse : db "Output : false" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputInt : db "%d", 0
