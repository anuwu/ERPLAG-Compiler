#! /bin/bash 
BOLD='\033[1m'
RED='\033[1;31m'
RESET='\033[0m'
CYAN='\033[1;36m'

input () {
	while :
	do
		read ans
		if test "$ans" = "y" -o "$ans" = "Y"
		then
			find . -type f  ! -name "*.*" | xargs rm -f -v
			find . -type f  -name "*.asm" | xargs rm -f -v
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

if ! [ "$l1" = "" -a "$l2" == "" ]
then
	printf "These are the files that will be deleted - \n\n"
	printf "${BOLD}${RED}"
	printf "$l1"
	printf "${CYAN}"
	printf "$l2"
	printf "${RESET}\nAre you sure you want to delete the above files? (y/n) "
	input
else
	printf "No .asm or executable files found\n"
fi