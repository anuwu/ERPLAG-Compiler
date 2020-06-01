@echo off
setlocal EnableDelayedExpansion

if exist compiler.exe ( 
	del compiler.exe
	set dbgrem=1
)
if exist ast.exe ( 
	del ast.exe 
	set dbgrem=1
)
if exist ST.exe ( 
	del ST.exe 
	set dbgrem=1
)
if !dbgrem!==1 (
	echo Debugging executables found^. Removed^^!
)

if exist .compiler (
	set /p yn=Are you sure you want to uninstall ERPLAG compiler? [y/n] : 

	:rst
	set valyn=0

	IF "!yn!"=="y" (
		set valyn=1
		goto unin
	)
	IF "!yn!"=="Y" (
		set valyn=1
		goto unin
	)
	IF "!yn!"=="n" (
		EXIT /B 0
	)
	IF "!yn!"=="N" (
		EXIT /B 0
	)

	IF !valyn!==0 (
		set /p yn=Please re-enter correct option [y/n] : 
		goto rst
	)

	:unin
	echo Uninstalling ERPLAG compiler^.^.^.
	echo.
	del .compiler
	rename erplag.bat .erplag

	echo ERPLAG compiler has been successfully uninstalled^^!
	EXIT /B 0
) 

echo ERPLAG compiler installation does not exist^^!
EXIT /B 1
