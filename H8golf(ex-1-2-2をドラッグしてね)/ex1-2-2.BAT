@REM "ex1-2-2.c"をコンパイルするためのバッチファイルです。
@REM 本バッチはGDLから使用されるものではありません。
@REM GDLを使用しないで単体でコンパイルする際に利用してください。
cd %~dp0
@SET PATH=C:\PROGRA~3\BestTech\GCCDEV~1\GCC\H8\bin;c:\PROGRA~3\BestTech\GCCDEV~1\GCC\H8\libexec\gcc\H8300-~1\47-GNU~1.02;c:\PROGRA~3\BestTech\GCCDEV~1\GCC\H8\H8300-~1\bin
h8300-elf-gcc -I C:\PROGRA~3\BestTech\GCCDEV~1\TARGET\3048F -L C:\PROGRA~3\BestTech\GCCDEV~1\TARGET\3048F -mh -nostartfiles -fsigned-char -Wall -Wno-main -pipe -O0 -TC:\PROGRA~3\BestTech\GCCDEV~1\TARGET\3048F\h8ram.x -Xlinker -Map -Xlinker "ex1-2-2.map" C:\PROGRA~3\BestTech\GCCDEV~1\TARGET\3048F\h8crt0.s "ex1-2-2.c" -ladd3048 -lm -o "ex1-2-2.exe" 
h8300-elf-objcopy -O srec  "ex1-2-2.exe" "ex1-2-2.mot"
PAUSE
