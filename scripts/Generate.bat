@echo off
pushd %~dp0
cd ../
call vendor\Premake\premake5.exe vs2026
popd
PAUSE