# ERPLAG

Welcome to the ERPLAG compiler. This is a toy language that was given as a semester project from the Compilers Construction course at BITS Pilani, in the year 2020. 

## Pre-requisites

### For Linux/macOS
In Linux, dependencies are taken care of by the installation script. 
In macOS, please ensure that homebrew is installed (if not, the script attempts to install homebrew)

### For Windows
Please install gcc, nasm and add them to the environment PATH variable.

## Installation
To install the compiler, clone this repository, open a terminal in its directory and run the install script
   - `git clone https://github.com/anwesh0304/ERPLAG-Compiler.git`
   - `cd ERPLAG-Compiler`
   - `./install` for **Linux/macOS**
   - `install` for **Windows**

If the installation fails, you have to complete the following steps -
   - `gcc -o .compiler erplag.c codegen.c ast.c lexer.c parser.c symbolTable.c typeChecker.c error.c`
### In Linux/macOS
   - `sudo cp .compiler /usr/local/bin`
   - `sudo cp erplag /usr/local/bin`
   - `sudo cp erpclr /usr/local/bin`
      If the above steps fail, you have to add the repository directory to the PATH variable.
### In Windows
   Inside the repository, if there does not a batch file named 'erplag.bat', run in command prompt -
   - `rename .erplag erplag.bat`
   
   Add the repository directory to the environment PATH variable.
   
## Info
   
1. Create your own erplag files using an extension of .erp. 
2. Use erplag -h for help on using the compiler.
3. The language specification document is "ERPLAG-Specifications.pdf".
4. My personal modifications to the original specification is listed in "Notes.txt"

## Note
**The compiler has been tested build for 64-bit operating systems and has been tested on Linux (Ubuntu 18.04 LTS), macOS (Catalina) and Windows 10. It will probably not work for 32-bit machines/OS. However, it may just work for other 64-bit OSs apart from the ones mentioned.**

**It could also occur that the compiler isn't able to process the .asm files to output the executable. This is due to an incompatible version of NASM being installed by the installation script. If this occurs, please install a version of NASM that is compatible with your machine architecture. Then, the .asm files can be converted to executables in the following manner -**

First assemble using nasm -
   - `nasm -felf64 <filename>.asm` for **Linux**
   - `nasm -fmacho64 <filename>.asm` for **macOS**
   - `nasm -fwin64 <filename>.asm` for **Windows**
   
Then link with gcc -
   - `gcc -no-pie <filename>.o -o <executable>` for **Linux/macOS**
   - `gcc <filename>.obj -o <executable>.exe` for **Windows**

Thank you!
