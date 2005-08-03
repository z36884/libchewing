F:\mingw\bin\gcc.exe ..\src\*.c ..\src\common\*.c -I..\include -shared -o libchewing.dll -Wl,--out-implib,libchewing.a

F:\mingw\bin\gcc.exe ..\src\tools\sort_dic.c ..\src\common\*.c -I..\include -o sort_dic.exe

F:\mingw\bin\gcc.exe ..\src\tools\sort_word.c ..\src\common\*.c -I..\include -o sort_word.exe

F:\mingw\bin\gcc.exe ..\src\tools\maketree.c ..\src\common\*.c -I..\include -o maketree.exe

pause