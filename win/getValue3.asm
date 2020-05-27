extern printf
extern scanf
extern malloc
extern exit

global WinMain
section .text

WinMain:
		PUSH RBP
		MOV RBP, RSP

		SUB RSP, 4											; making space for declaration


		MOV RCX, @inputIntPrompt									; get_value ===> Changed
		MOV RBX, -2
		CALL @getValuePrimitive										


		MOV RCX, @inputIntPrompt									; get_value ===> Changed
		MOV RBX, -4
		CALL @getValuePrimitive

		SUB RSP, 12											; making space for declaration

		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV AX, [RBP - 2]
		MOV BX, [RBP - 4]
		CALL @dynamicDeclCheck								; checking dynamic array declaration limits ===> Changed


		PUSH BX												; saving register for malloc
		PUSH AX												; saving register for malloc
		SUB RSP, 48
		CALL malloc											; ===> Changed
		ADD RSP, 48
		MOV [RBP - 16], RAX
		POP AX
		MOV [RBP - 6], AX
		POP BX
		MOV [RBP - 8], BX

		MOV RCX, @inputIntArrPrompt							;  ===> Changed
		MOV SI, [RBP - 6]
		MOV DI, [RBP - 8]
		CALL @printGetArrPrompt

															;  ===> Changed
		MOV BX, DI
		SUB BX, SI
		ADD BX, 1
		ADD BX, BX
		MOVSX RBX, BX
		MOV RDI, [RBP - 16]
		CALL @getArr

		MOV RDI, [RBP - 16]
		MOV BX, [RBP - 6]
		MOV CX, [RBP - 2]
		MOV DX, [RBP - 8]
		CALL @boundCheck										; checking array index bound 
		MOV DX, [RDI + RBX]										; ===> Changed
		CALL @printInteger

		MOV RDI, [RBP - 16]
		MOV BX, [RBP - 6]
		MOV CX, [RBP - 4]
		MOV DX, [RBP - 8]
		CALL @boundCheck										; checking array index bound
		MOV DX, [RDI + RBX]
		CALL @printInteger

		SUB RSP, 2											; making space for declaration


		MOV RCX, @inputIntPrompt									; get_value
		MOV RBX, -18
		CALL @getValuePrimitive

		MOV RDI, [RBP - 16]
		MOV BX, [RBP - 6]
		MOV CX, [RBP - 18]
		MOV DX, [RBP - 8]
		CALL @boundCheck										; checking array index bound
		MOV DX, [RDI + RBX]
		CALL @printInteger

		MOV RDI, [RBP - 16]
		MOV AX, [RBP - 6]
		MOV BX, [RBP - 8]
		SUB BX, AX
		ADD BX, 1
		ADD BX, BX
		CALL @printIntegerArr


		MOV RSP, RBP
		POP RBP
		ret

;--------------------------------------------------------------------------------------------------

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
		MOV CX, BX
		SUB CX, AX
		ADD CX, 1
		ADD CX, CX
		MOVSX RCX, CX

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
		XOR RDX, RDX
		PUSH RBX
		SUB RSP, 40
		CALL printf
		ADD RSP, 40
		POP RBX

		MOV RCX, @inputInt										; get_value
		MOV RDX, RSP
		SUB RDX, 16
		PUSH RBX
		SUB RSP, 56
		CALL scanf
		ADD RSP, 56
		POP RBX
		MOV AX, [RSP - 16]
		MOV [RBP + RBX], AX

		POP RAX
		ADD RSP, RAX

		ret

@printGetArrPrompt:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV DX, DI
		SUB DX, SI
		ADD DX, 1
		MOVSX RDX, DX
		PUSH SI
		PUSH DI
		SUB RSP, 44
		CALL printf
		ADD RSP, 44
		POP DI
		POP SI


		MOV RCX, @leftRange
		MOVSX RDX, SI
		PUSH SI
		PUSH DI
		SUB RSP, 44
		CALL printf
		ADD RSP, 44
		POP DI
		POP SI

		MOV RCX, @rightRange
		MOVSX RDX, DI
		PUSH SI
		PUSH DI
		SUB RSP, 44
		CALL printf
		ADD RSP, 44
		POP DI
		POP SI

		POP RAX
		ADD RSP, RAX

		ret

@getArr:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		PUSH RDI
		MOV RAX, 0

	.getArrLoop:								; getting array
		MOV RCX, @inputInt
		MOV RDX, RSP
		SUB RDX, 24
		PUSH RAX
		PUSH RBX
		SUB RSP, 72
		CALL scanf
		ADD RSP, 72
		POP RBX
		POP RAX
		MOV CX, [RSP - 24]

		POP RDI
		PUSH RBX
		MOV RBX, RAX
		MOV [RDI + RBX], CX
		POP RBX
		PUSH RDI

		ADD RAX, 2
		CMP RAX, RBX
		JNE .getArrLoop

		POP RDI
		POP RAX
		ADD RSP, RAX

		ret

@printInteger:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RCX, @printFormat
		MOVSX RDX, DX
		SUB RSP, 32
		CALL printf
		ADD RSP, 32

		POP RAX
		ADD RSP, RAX

		ret

@printIntegerArr:
		MOV RAX, RSP									; Stack Alignment
		AND RAX, 15
		ADD RAX, 8
		SUB RSP, RAX
		PUSH RAX

		MOV RCX, @printFormatArray
		XOR RDX, RDX
		PUSH RDI
		PUSH BX
		SUB RSP, 38
		CALL printf
		ADD RSP, 38
		POP BX
		POP RDI

		MOV AX, 0

	.printArr:
		MOV RCX, @printInt

		PUSH BX
		MOVSX RBX, AX
		MOV DX, [RDI + RBX]
		MOVSX RDX, DX
		POP BX

		PUSH RDI
		PUSH AX
		PUSH BX
		SUB RSP, 36
		CALL printf
		ADD RSP, 36
		POP BX
		POP AX
		POP RDI

		ADD AX, 2
		CMP AX, BX
		JNE .printArr

		MOV RCX, @printNewLine
		XOR RDX, RDX
		SUB RSP, 32
		CALL printf
		ADD RSP, 32

		POP RAX
		ADD RSP, RAX

		ret

;--------------------------------------------------------------------------------------------------

section .data
		@boundPrint : db "ERPLAG : Runtime Error --> Array out of bounds. Halt!" , 10, 0
		@declPrint : db "ERPLAG : Runtime Error --> Invalid order of bounds in dynamic array declaration. Halt!" , 10, 0
		@declNeg : db "ERPLAG : Runtime Error --> Negative bound in dynamic array declaration. Halt!" , 10, 0
		@printFormatArray : db "Output : " , 0
		@printInt : db "%d ", 0
		@printNewLine : db 10, 0
		@printFormat : db "Output :  %d" , 10, 0
		@inputIntPrompt : db "Enter an integer : " , 0
		@inputIntArrPrompt : db "Enter %d array elements of integer type for range ", 0
		@leftRange : db "%d to " , 0
		@rightRange : db "%d" ,10, 0
		@inputInt : db "%d", 0
