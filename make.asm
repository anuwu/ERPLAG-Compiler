output : code.asm
	nasm -felf64 code.asm
	gcc -no-pie code.o