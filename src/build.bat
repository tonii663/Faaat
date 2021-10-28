
@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set libs=user32.lib gdi32.lib winmm.lib

rem -WX -W4 -wd4505 -wd4456 -wd4201 -wd4100 -wd4189
set CompilerFlags=-nologo -Od -MTd  -std:c++17 -FAsc -Zi -Oi -GR- -EHa


cl %CompilerFlags% -FmFaaat.map ..\src\Faaat.cpp /link -INCREMENTAL:no %libs%

popd
	
