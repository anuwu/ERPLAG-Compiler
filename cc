#! /bin/bash

BOLD='\033[1m'
RED='\033[1;31m'
RESET='\033[0m'

ccErr () {
	printf "\n${BOLD}${RED}ERPLAG re-compilation failed.${RESET}\n"
	printf "Press any key to continue...\n"
}

sudo make

sudo cp erplag /usr/local/bin
stat=$?
if [ $stat -ne 0 ]
then
	ccErr
	exit
fi
sudo cp .compiler /usr/local/bin
stat=$?
if [ $stat -ne 0 ]
then
	ccErr
	exit
fi

make clean