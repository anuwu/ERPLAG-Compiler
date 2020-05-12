# ERPLAG

Welcome to the ERPLAG compiler. This is a toy language that was given as a semester project from the Compilers Construction course at BITS Pilani, in the year 2020. 

## Intallation
To install the compiler, run the following commands after opening a terminal in a suitable folder -

   - `git clone https://github.com/anwesh0304/ERPLAG-Compiler.git`
   - `cd ERPLAG-Compiler`
   - `./install`

If the installation fails, you can do either of the following -
1. Edit the ~/.bashrc file and export the repository directory to the PATH variable. Then execute
   - `source ~/.bashrc`
   - Restart bash
2. Copy the following files to the standard bin directory of your system -
   - .`/erplag`
   - `./.compiler`
   
## Info
   
Create your own erplag files using an extension of .erp. Use erplag -h for help on using the compiler.
The language specification document is present in the repository. Its name is "ERPLAG-Specifications.pdf".
My personal modifications to the original specification is listed in "Notes.txt"

If you wish to make changes to the compiler source, or the compiler script file `erplag`, then run the following command after doing so -
   - `./cc`

If you wish to remove any .asm, .o or executable files recursively in the Testcase folder, run the clean.sh script in that folder.
Do not run the clean.sh script in the repository folder, it will delete the compiler source files.

## Note
**It could occur that the compiler isn't able to process the .asm files to output the executable. This is due to an incompatible version of NASM being installed by the installation script. If this occurs, please install a version of NASM that is compatible with your machine architecture. Then, the .asm files can be converted to executables in the following manner -**

   - `nasm -felf64 <filename>.asm`
   - `gcc -no-pie <filename>.o -o <executable>`

Thank you!
