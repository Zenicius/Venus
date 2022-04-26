@echo off
pushd %~dp0\..\
call vendor\bin\premake5.exe vs2022
pushd Hazelnut\SandboxProject
call Win-CreateScriptProjects.bat
popd
popd
PAUSE
