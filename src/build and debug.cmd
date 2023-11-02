cd out
del *.o
del *.xex
cd ..

atasm -gout\boot_part1.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part1.xex -lout\boot_part1.lab asm\boot\boot_part1.asm
atasm -gout\boot_part2.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part2.xex -lout\boot_part2.lab asm\boot\boot_part2.asm
bin\merge-xex out/boot_part1.xex out/boot_part2.xex out/boot.xex

atasm -gout\retromaz.lst -Iasm -Iasm/includes -Iasm/fonts -Iasm/music -oout/fixed.xex -hcfixed.h -lfixed.lab asm\fixed.asm
@if %errorlevel% neq 0 goto error

c:\progs\cc65\bin\cl65 -DDEMO_VERSION --debug-info -Wl --dbgfile,"out\retromaz.dbg" --lib-path C:\progs\cc65\lib -C atari.cfg -t atari -T -Cl -Oirs -O -vm -v --mapfile retromaze.map -l a.txt -o out\main.xex maze65.c hall.c maze_generator.c draw_map.c map_gui.c asm\ataristd.s

@if %errorlevel% neq 0 goto error
bin\joinxex out/boot_part1.xex out/boot_part2.xex out/fixed.xex out/main.xex -j -oout/retromaz.xex
goto end
:error
pause
exit /b %errorlevel%
:end
REM start out\maze65.xex
start "" "D:\Altirra\3.90\altirra64.exe" /run out\retromaz.xex /debug /debugcmd:.loadsym