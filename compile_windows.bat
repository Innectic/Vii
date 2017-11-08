
@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
echo Compiling %2
cd /D %1

cl %1\%2 /nologo /std:c++latest /EHsc /Gr
