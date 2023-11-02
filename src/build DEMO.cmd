cd out
del *.o
del *.xex
cd ..
atasm -gout\boot_part1.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part1.xex -lout\boot_part1.lab asm\boot\boot_part1.asm
atasm -gout\boot_part2.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part2.xex -lout\boot_part2.lab asm\boot\boot_part2.asm

atasm -gout\retromaz.lst -Iasm -Iasm/includes -Iasm/fonts -Iasm/music -oout/fixed.xex -hcfixed.h -lfixed.lab asm\fixed.asm
@if %errorlevel% neq 0 goto error

c:\progs\cc65\bin\cl65 -D DEMO_VERSION -Ln retromaze.lbl --lib-path C:\progs\cc65\lib -C ./atari.cfg -t atari -T -Cl -Oirs -O -vm -v --mapfile maze65.map -l a.txt -o out\main.xex maze65.c hall.c maze_generator.c draw_map.c map_gui.c asm\ataristd.s

@if %errorlevel% neq 0 goto error
bin\joinxex out/boot_part1.xex out/boot_part2.xex out/fixed.xex out/main.xex -j -oout/retromaz.xex
::bin\merge-xex out/fixed.xex out/main.xex out/retromaz.xex
goto end
:error
pause
exit /b %errorlevel%
:end
start out\retromaz.xex

