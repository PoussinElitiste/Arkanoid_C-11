#!/bin/bash
cd dev
echo "1. check 8dot3name status"
echo "-------------------------" 
fsutil 8dot3name query
echo
echo "If 8dot3name is disabled (1) follow these steps:"
echo 'step 1. Open an admin cmd'
echo 'step 2. Enable admin cmd and execute -> fsutil behavior set disable8dot3 0'
echo 'step 3. Create short name -> fsutil file setshortname "Program Files (x86)" PROGRA~2'
echo 'step 4. Check short path -> dir /x'
echo
echo "2. check vcvars64 env"
echo "---------------------" 
f() { find c -name "*$1*"; }

if [ -z "$VCVARS64" ]
then
	#echo "\$VCVARS64 not defined"
	#f vcvars64.bat

	#export path_tmp="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build"

	
	#cmd '/c for %A in (%path_tmp%) do @echo %~sA'
	export VCVARS64="C:\PROGRA~2\MICROS~1\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
else
	echo $VCVARS64
fi
echo
echo "3. launch build"
echo "---------------------" 
pushd Build
cmd '/c %VCVARS64% && msbuild /p:Configuration=Debug Arkanoid.sln -fl -flp:logfile=Arkanoid_MSBuild.log;verbosity=n'
read -rsp $'Press any key to continue...\n' -n1 key
#popd