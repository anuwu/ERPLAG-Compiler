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


	@WHILE1:
		MOV BX, 1
		PUSH BX
		POP AX
		CMP AX, 0											; checking while loop condition
		JE @WHILE2

		JMP @WHILE1

	@WHILE2:

		MOV RSP, RBP
		POP RBP
		MOV RAX, 60
		XOR RDI, RDI
		syscall

;--------------------------------------------------------------------------------------------------

;--------------------------------------------------------------------------------------------------
