output : $(FILE).o
	gcc -no-pie $(FILE).o -o $(FILE)

$(FILE).o : $(FILE).asm
	nasm -felf64 $(FILE).asm
