output :
	nasm -felf64 $(FILE).asm
	gcc -no-pie $(FILE).o -o $(FILE)