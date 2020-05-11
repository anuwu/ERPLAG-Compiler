#! /bin/bash

# erplag -a file.erp 			--> Generates file.asm (Done)
# erplag file.asm				--> Generates file executable
# erplag -o testExec file.erp   --> Generates executable with the name (Done)
# erplag file.erp 				--> Generates file executable directly (Done)

optflag=0
while getopts ":a:o:" opt; do
	optflag=1
	case ${opt} in
		a)	compiler $OPTARG ;
			;;
		o)	target=$OPTARG ; 
			srcname=$3 ;
			fname=${srcname:0:${#srcname}-4} ;
			asmname=${fname}.asm ;
			compiler $srcname ;
			status=$? ;
			if [ $status -eq 0 ]
			then
				objname=${fname}.o ;
				nasm -felf64 $asmname ;
				gcc -no-pie $objname -o $target ;
				rm $asmname
				rm $objname
			fi
			;;
		\?) echo "ERPLAG : Invalid option. Use -a to generate x86 assembly or use -o along with output name to generate executable" ;;
		:) echo "ERPLAG : Please provide output file name" ;
	esac
done
shift $((OPTIND-1))

if [ $optflag -eq 0 ]
then
	srcname=$1 ;
	if [[ $srcname =~ \.asm$ ]] 
	then
		# Need to check if file exists or not in bash
		if [ -f $srcname ] 
		then
			fname=${srcname:0:${#srcname}-4} ;
			objname=${fname}.o
			nasm -felf64 $srcname ;
			gcc -no-pie $objname -o $fname ;
		else
			echo "ERPLAG : ASM file $srcname does not exist" ;
		fi
	elif [[ $srcname =~ \.erp$ ]] 
	then
		fname=${srcname:0:${#srcname}-4} ;
		asmname=${fname}.asm
		objname=${fname}.o
		compiler $srcname
		status=$? ;
		if [ $status -eq 0 ]
		then
			nasm -felf64 $asmname ;
			gcc -no-pie $objname -o $fname ;
			rm $asmname
			rm $objname
		fi
	else
		echo "ERPLAG : Unsupported file format. Use .erp for source files and .asm for x86 assembly files" ;
	fi
fi