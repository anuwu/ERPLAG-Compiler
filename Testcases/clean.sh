#! /bin/bash 
BOLD='\033[1m'
RED='\033[1;31m'
CYAN='\033[1;36m'
GREEN='\033[1;32m'
MAGENTA='\033[1;35m'
RESET='\033[0m'

input () {
	while :
	do
		read ans
		if test "$ans" = "y" -o "$ans" = "Y"
		then
			find . -type f  ! -name "*.*" | xargs rm -f -v
			find . -type f  -name "*.asm" | xargs rm -f -v
			find . -type f  -name "*.o" | xargs rm -f -v
			exit
		elif  [ "$ans" = "n" -o "$ans" = "N" ]
		then
			exit
		else
			printf "\nPlease re-enter correct option (y/n) "
			input
		fi
	done
}

l1=$(find . -type f  ! -name "*.*")
l2=$(find . -type f  -name "*.asm")
l3=$(find . -type f  -name "*.o")

if ! [ "$l1" = "" -a "$l2" == "" -a "$l3" == "" ]
then
	printf "These are the files that will be deleted - \n\n"
	printf "${BOLD}"
	if ! [ "$l1" = "" ] 
	then
		printf "${RED}$l1"
		printf "\n"
	fi
	if ! [ "$l2" = "" ]
	then
		printf "${CYAN}$l2"
		printf "\n"
	fi
	if ! [ "$l3" = "" ]
	then
		printf "${MAGENTA}$l3"
		printf "\n"
	fi
	printf "${RESET}Are you sure you want to delete the above files? (y/n) "
	input
else
	printf "${BOLD}${GREEN}Clean! ${RESET}\nNo .asm, .o or executables found\n"
fi
