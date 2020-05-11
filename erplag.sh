#! /bin/bash

# erplag -a file.erp 			--> Generates file.asm
# erplag file.asm				--> Generates file executable 
# erplag -o testExec file.erp   --> Generates executable with the name (Creates temp files)
# erplag file.erp 				--> Generates file executable directly (Creates temp files)

optflag=0
while getopts ":a:o:" opt; do
	optflag=1
	case ${opt} in
		a)	compiler $OPTARG
			;;
		o)	target=$OPTARG 
			srcname=$3 
			asmname=${srcname}.asm
			compiler $srcname $asmname 
			status=$? 
			if [ $status -eq 0 ]
			then
				objname=${srcname}.o 
				nasm -felf64 $asmname -o $objname 
				gcc -no-pie $objname -o $target 
			fi
			rm -f $asmname
			rm -f $objname
			;;
		\?) echo "ERPLAG : Invalid option. Use -a to generate x86 assembly or use -o along with output name to generate executable" ;;
		:) echo "ERPLAG : Please provide output file name" 
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
			fname=${srcname:0:${#srcname}-4} 
			objname=${srcname}.o
			nasm -felf64 $srcname -o $objname
			gcc -no-pie $objname -o $fname 
			rm -f $objname
		else
			echo "ERPLAG : ASM file $srcname does not exist" 
		fi
	elif [[ $srcname =~ \.erp$ ]] 
	then
		fname=${srcname:0:${#srcname}-4} 
		asmname=${srcname}.asm
		objname=${srcname}.o
		compiler $srcname $asmname
		status=$? 
		if [ $status -eq 0 ]
		then
			nasm -felf64 $asmname
			gcc -no-pie $objname -o $fname
		fi
		rm -f $asmname
		rm -f $objname
	else
		echo "ERPLAG : Unsupported file format. Use .erp for source files and .asm for x86 assembly files" ;
	fi
fi