#!/bin/bash
echo "1. check 8dot3name(long ot short path) status"
fsutil 8dot3name query
echo "If 8dot3name is disabled (1), enable the short name feature and create short name using the command:"
echo 'enable cmd        : fsutil behavior set disable8dot3 0'
echo 'create short name : fsutil file setshortname "Program Files" PROGRA~1'
echo 'check short path  : cd /x'

echo $VCVARS64
pushd Build
cmd '/c %VCVARS64% && msbuild /p:Configuration=Debug Arkanoid.sln -fl -flp:logfile=Arkanoid_MSBuild.log;verbosity=n'
#popd