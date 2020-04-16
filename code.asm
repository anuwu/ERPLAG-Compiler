SECTION .bss
	lpBuffer: resb 18
	Buf_Len: equ $-lpBuffer

SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:

MOV AX, 5
PUSH AX
MOV BX, 7
PUSH BX
POP AX
POP BX
ADD AX,BX
PUSH AX
MOV AX, 4
PUSH AX
POP AX
POP BX
SUB AX,BX
PUSH AX
POP AX
MOV [i_cb],AX
MOV BX, [i_cb]
PUSH BX
POP AX
call outPut
call printInt

call exit

outPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msg	
   mov	edx, len
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
   pop edx
    pop ecx
    pop ebx
    pop eax
    
    ret

inPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgi	
   mov	edx, leni
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
trUE:  
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgt	
   mov	edx, lent
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
    
faLSE:
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgf	
   mov	edx, lenf
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
erROR:
	push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgerror	
   mov	edx, lenerror
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

printStr:
    push    eax
    push    ebx
    mov     ebx, 1
    mov     eax, 4
    int     80H
    pop     ebx
    pop     eax
    ret
     
readStr:
    push    ebx
    mov     ebx, 0
    mov     eax, 3
    int     80H
    pop     ebx
    ret

printInt:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    movsx eax,ax
    mov esi, lpBuffer
    add esi, Buf_Len
    mov ecx,10
    mov edx, eax 
    cmp eax,0
    jge .non_neg
    mov eax,0
    sub eax,edx
    push '-'
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    mov ecx,10

.non_neg:
    mov edx,0
    idiv ecx
    add dl,0x30
    mov [esi],dl
    dec esi
    cmp eax,0
    jnz .non_neg

    mov edx, lpBuffer
    add edx, Buf_Len
    mov ecx,esi
    sub edx,ecx
    inc ecx
    call printStr
    push 10
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

readInt:
    push ecx
    push edx
    push esi
    mov     ecx, lpBuffer
    mov     edx, Buf_Len
    call    readStr
    mov     esi,eax
    mov     byte [lpBuffer+esi], 0
    mov     eax, lpBuffer
    call    atoi
    pop     esi
    pop     edx
    pop     ecx
    ret

atoi:
    push    ebx
    push    ecx
    push    edx
    push    esi
    mov     esi, eax        
    mov     eax, 0          
    mov     ecx, 0          
    
    mov     bl, [esi+ecx]   
    cmp     bl,'-'          
    jne     .multiplyLoop
    inc     ecx

.multiplyLoop:
    mov     ebx, 10         
    mul     ebx             
    mov     bl, [esi+ecx]   
    cmp     bl, 48          
    jl      .finished       
    cmp     bl, 57          
    jg      .finished       
 
    sub     bl, 48          
    add     eax, ebx        
    inc     ecx             
    jmp     .multiplyLoop   

.finished:
    mov     ebx,10
    div     ebx
    mov     bl, [esi]
    cmp     bl,'-'
    jne     .fin2
    mov     ebx,eax
    mov     eax,0
    sub     eax,ebx
    
.fin2:
    pop     esi             
    pop     edx
    pop     ecx             
    pop     ebx             
    ret
    
exit: 
    mov     eax,1
    xor     ebx,ebx
    int     80H
SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:

MOV BX, 4
PUSH BX
POP AX
MOV [i_cb],AX

call exit

outPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msg	
   mov	edx, len
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
   pop edx
    pop ecx
    pop ebx
    pop eax
    
    ret

inPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgi	
   mov	edx, leni
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
trUE:  
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgt	
   mov	edx, lent
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
    
faLSE:
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgf	
   mov	edx, lenf
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
erROR:
	push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgerror	
   mov	edx, lenerror
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

printStr:
    push    eax
    push    ebx
    mov     ebx, 1
    mov     eax, 4
    int     80H
    pop     ebx
    pop     eax
    ret
     
readStr:
    push    ebx
    mov     ebx, 0
    mov     eax, 3
    int     80H
    pop     ebx
    ret

printInt:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    movsx eax,ax
    mov esi, lpBuffer
    add esi, Buf_Len
    mov ecx,10
    mov edx, eax 
    cmp eax,0
    jge .non_neg
    mov eax,0
    sub eax,edx
    push '-'
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    mov ecx,10

.non_neg:
    mov edx,0
    idiv ecx
    add dl,0x30
    mov [esi],dl
    dec esi
    cmp eax,0
    jnz .non_neg

    mov edx, lpBuffer
    add edx, Buf_Len
    mov ecx,esi
    sub edx,ecx
    inc ecx
    call printStr
    push 10
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

readInt:
    push ecx
    push edx
    push esi
    mov     ecx, lpBuffer
    mov     edx, Buf_Len
    call    readStr
    mov     esi,eax
    mov     byte [lpBuffer+esi], 0
    mov     eax, lpBuffer
    call    atoi
    pop     esi
    pop     edx
    pop     ecx
    ret

atoi:
    push    ebx
    push    ecx
    push    edx
    push    esi
    mov     esi, eax        
    mov     eax, 0          
    mov     ecx, 0          
    
    mov     bl, [esi+ecx]   
    cmp     bl,'-'          
    jne     .multiplyLoop
    inc     ecx

.multiplyLoop:
    mov     ebx, 10         
    mul     ebx             
    mov     bl, [esi+ecx]   
    cmp     bl, 48          
    jl      .finished       
    cmp     bl, 57          
    jg      .finished       
 
    sub     bl, 48          
    add     eax, ebx        
    inc     ecx             
    jmp     .multiplyLoop   

.finished:
    mov     ebx,10
    div     ebx
    mov     bl, [esi]
    cmp     bl,'-'
    jne     .fin2
    mov     ebx,eax
    mov     eax,0
    sub     eax,ebx
    
.fin2:
    pop     esi             
    pop     edx
    pop     ecx             
    pop     ebx             
    ret
    
exit: 
    mov     eax,1
    xor     ebx,ebx
    int     80H
SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:

MOV BX, 4
PUSH BX
POP AX
MOV [i_cb],AX
MOV BX, [i_cb]
PUSH BX
POP AX
call outPut
call printInt

call exit

outPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msg	
   mov	edx, len
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
   pop edx
    pop ecx
    pop ebx
    pop eax
    
    ret

inPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgi	
   mov	edx, leni
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
trUE:  
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgt	
   mov	edx, lent
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
    
faLSE:
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgf	
   mov	edx, lenf
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
erROR:
	push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgerror	
   mov	edx, lenerror
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

printStr:
    push    eax
    push    ebx
    mov     ebx, 1
    mov     eax, 4
    int     80H
    pop     ebx
    pop     eax
    ret
     
readStr:
    push    ebx
    mov     ebx, 0
    mov     eax, 3
    int     80H
    pop     ebx
    ret

printInt:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    movsx eax,ax
    mov esi, lpBuffer
    add esi, Buf_Len
    mov ecx,10
    mov edx, eax 
    cmp eax,0
    jge .non_neg
    mov eax,0
    sub eax,edx
    push '-'
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    mov ecx,10

.non_neg:
    mov edx,0
    idiv ecx
    add dl,0x30
    mov [esi],dl
    dec esi
    cmp eax,0
    jnz .non_neg

    mov edx, lpBuffer
    add edx, Buf_Len
    mov ecx,esi
    sub edx,ecx
    inc ecx
    call printStr
    push 10
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

readInt:
    push ecx
    push edx
    push esi
    mov     ecx, lpBuffer
    mov     edx, Buf_Len
    call    readStr
    mov     esi,eax
    mov     byte [lpBuffer+esi], 0
    mov     eax, lpBuffer
    call    atoi
    pop     esi
    pop     edx
    pop     ecx
    ret

atoi:
    push    ebx
    push    ecx
    push    edx
    push    esi
    mov     esi, eax        
    mov     eax, 0          
    mov     ecx, 0          
    
    mov     bl, [esi+ecx]   
    cmp     bl,'-'          
    jne     .multiplyLoop
    inc     ecx

.multiplyLoop:
    mov     ebx, 10         
    mul     ebx             
    mov     bl, [esi+ecx]   
    cmp     bl, 48          
    jl      .finished       
    cmp     bl, 57          
    jg      .finished       
 
    sub     bl, 48          
    add     eax, ebx        
    inc     ecx             
    jmp     .multiplyLoop   

.finished:
    mov     ebx,10
    div     ebx
    mov     bl, [esi]
    cmp     bl,'-'
    jne     .fin2
    mov     ebx,eax
    mov     eax,0
    sub     eax,ebx
    
.fin2:
    pop     esi             
    pop     edx
    pop     ecx             
    pop     ebx             
    ret
    
exit: 
    mov     eax,1
    xor     ebx,ebx
    int     80H
SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:


SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:


SECTION .data
    msg db "The OUTPUT is:  " 
    len equ $- msg
 	
    msgi db "INPUT: ENTER AN INTEGER VALUE:" 
    leni equ $- msgi
    
    msgt db "true  " 
    lent equ $- msgt
    
    msgf db "false  " 
    lenf equ $- msgf	
     
    msgerror db "run-time error detected"
    lenerror equ $- msgerror		
SECTION .text
global _start

_start:


call exit

outPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msg	
   mov	edx, len
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
   pop edx
    pop ecx
    pop ebx
    pop eax
    
    ret

inPut:
     push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgi	
   mov	edx, leni
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
trUE:  
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgt	
   mov	edx, lent
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
    
faLSE:
    push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgf	
   mov	edx, lenf
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
erROR:
	push eax
    push ebx
    push ecx
    push edx
    push esi
   mov	ecx, msgerror	
   mov	edx, lenerror
   mov	ebx,1	        ;file descriptor (stdout)
   mov	eax,4
   int	0x80
   pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

printStr:
    push    eax
    push    ebx
    mov     ebx, 1
    mov     eax, 4
    int     80H
    pop     ebx
    pop     eax
    ret
     
readStr:
    push    ebx
    mov     ebx, 0
    mov     eax, 3
    int     80H
    pop     ebx
    ret

printInt:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    movsx eax,ax
    mov esi, lpBuffer
    add esi, Buf_Len
    mov ecx,10
    mov edx, eax 
    cmp eax,0
    jge .non_neg
    mov eax,0
    sub eax,edx
    push '-'
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    mov ecx,10

.non_neg:
    mov edx,0
    idiv ecx
    add dl,0x30
    mov [esi],dl
    dec esi
    cmp eax,0
    jnz .non_neg

    mov edx, lpBuffer
    add edx, Buf_Len
    mov ecx,esi
    sub edx,ecx
    inc ecx
    call printStr
    push 10
    mov ecx,esp
    mov edx,1
    call printStr
    add esp, 4
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

readInt:
    push ecx
    push edx
    push esi
    mov     ecx, lpBuffer
    mov     edx, Buf_Len
    call    readStr
    mov     esi,eax
    mov     byte [lpBuffer+esi], 0
    mov     eax, lpBuffer
    call    atoi
    pop     esi
    pop     edx
    pop     ecx
    ret

atoi:
    push    ebx
    push    ecx
    push    edx
    push    esi
    mov     esi, eax        
    mov     eax, 0          
    mov     ecx, 0          
    
    mov     bl, [esi+ecx]   
    cmp     bl,'-'          
    jne     .multiplyLoop
    inc     ecx

.multiplyLoop:
    mov     ebx, 10         
    mul     ebx             
    mov     bl, [esi+ecx]   
    cmp     bl, 48          
    jl      .finished       
    cmp     bl, 57          
    jg      .finished       
 
    sub     bl, 48          
    add     eax, ebx        
    inc     ecx             
    jmp     .multiplyLoop   

.finished:
    mov     ebx,10
    div     ebx
    mov     bl, [esi]
    cmp     bl,'-'
    jne     .fin2
    mov     ebx,eax
    mov     eax,0
    sub     eax,ebx
    
.fin2:
    pop     esi             
    pop     edx
    pop     ecx             
    pop     ebx             
    ret
    
exit: 
    mov     eax,1
    xor     ebx,ebx
    int     80H