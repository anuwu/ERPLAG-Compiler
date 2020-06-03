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

		SUB RSP, 2											; making space for declaration


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -2
		CALL @getValuePrimitive


		MOV RDI, @inputIntPrompt									; get_value
		MOV RBX, -4
		CALL @getValuePrimitive


		MOV RDI, @inputBoolPrompt									; get_value
		MOV RBX, -6
		CALL @getValuePrimitive

		SUB RSP, 24											; declaring before switch

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
		MOV RBX, -30

		MOV CX, 0

	.declLoop:
		PUSH CX
		PUSH RDI
		PUSH RBX
		SUB RSP, 10
		CALL malloc
		ADD RSP, 10
		POP RBX
		MOV [RBP + RBX], RAX
		POP RDI
		POP CX
		POP SI
		MOV [RBP + RBX + 10], SI
		POP DI
		MOV [RBP + RBX + 8], DI
		PUSH DI
		PUSH SI
		ADD RBX, 12
		INC CX
		CMP CX, 2
		JL .declLoop
		ADD RSP, 4								; Restoring array limits

		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		SUB RSP, 12											; declaring before switch

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV AX, 0
		MOV BX, [RBP - 4]
		CALL @dynamicDeclCheck										; checking dynamic array declaration limits


		PUSH BX												; saving register for malloc
		PUSH AX												; saving register for malloc
		SUB RSP, 12
		CALL malloc
		ADD RSP, 12
		MOV [RBP - 42], RAX
		POP SI
		MOV [RBP - 32], SI
		POP DI
		MOV [RBP - 34], DI


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment

		MOV AX, [RBP - 6]										; loading switch variable

		CMP AX, 0
		JNE @SWITCH1											; true case
		JMP @SWITCH2											; false case

@SWITCH1:

		JMP @SWITCH3

@SWITCH2:

		JMP @SWITCH3

@SWITCH3:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RDI, [RBP - 42]
		CALL free								; freeing local dynamic arrays


		MOV RDI, [RBP - 30]
		CALL free								; freeing local dynamic arrays


		MOV RDI, [RBP - 18]
		CALL free								; freeing local dynamic arrays


		POP RAX
		ADD RSP, RAX									; Restoring Stack Alignment
		ADD RSP, 36											; restoring to parent scope

		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

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

;--------------------------------------------------------------------------------------------------

section .data
		@declPrint : db "[1m[31mRuntime Error [0m[1m--> [0mInvalid order of bounds in dynamic array declaration. Halt!" , 10, 0
		@declNeg : db "[1m[31mRuntime Error [0m[1m--> [0mNegative bound in dynamic array declaration. Halt!" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputBoolPrompt : db "Enter a boolean (0 for false, non-zero for true) : " , 0
		@inputInt : db "%d", 0
