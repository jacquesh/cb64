@echo off

cl -nologo -Zi -W4 -std:c++17 -I. -I../ -DCATCH_CONFIG_MAIN -EHsc cb64_test.cpp

IF %ERRORLEVEL% EQU 0 (
    cb64_test.exe
)
