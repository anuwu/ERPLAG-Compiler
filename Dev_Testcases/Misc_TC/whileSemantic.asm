;;;;;;;;;;; Compiled for 64-bit Linux ;;;;;;;;;;;

extern printf
extern scanf
extern malloc
extern free
extern exit

global main
section .text

test:
		PUSH RBP
		MOV RBP, RSP

		MOV BX, 1
		PUSH BX
		MOV AX, [RBP + 16]
		PUSH AX
		POP AX
		POP BX
		ADD AX,BX
		PUSH AX
		POP AX
		MOV [RBP + 16], AX										; store variable

		MOV SI, [RBP + 16]
		CALL @printInteger


		MOV RSP, RBP
		POP RBP
		ret

;--------------------------------------------------------------------------------------------------

main:
		PUSH RBP
		MOV RBP, RSP

		SUB RSP, 4											; making space for declaration


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

		MOV SI, [RBP - 2]
		CALL @printInteger

		JMP @WHILE1

	@WHILE2:

	@WHILE3:
		MOV BX, 1
		PUSH BX
		POP AX
		CMP AX, 0											; checking while loop condition
		JE @WHILE4

		JMP @WHILE3

	@WHILE4:

		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

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
		@printFormat : db "Output :  %d" , 10, 0
