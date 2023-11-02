cd gen_asm
del *.asm
cd ..
atasm -gout\boot_part1.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part1.xex -lout\boot_part1.lab asm\boot\boot_part1.asm
atasm -gout\boot_part2.lst -Iasm -Iasm/includes -Iasm/boot -oout/boot_part2.xex -lout\boot_part2.lab asm\boot\boot_part2.asm
bin\merge-xex out/boot_part1.xex out/boot_part2.xex out/boot.xex

atasm -gout\retromaz.lst -Iasm -Iasm/includes -Iasm/fonts -Iasm/music -oout/fixed.xex -hcfixed.h -lfixed.lab asm\fixed.asm

:: -S to generate the assembler file
c:\progs\cc65\bin\cl65 -S -C atari.cfg -t atari -T -Cl -Oirs -O -vm -v -o gen_asm/maze65.asm maze65.c
@if %errorlevel% neq 0 goto error
:: 
c:\progs\cc65\bin\cl65 -S -C atari.cfg -t atari -T -Cl -Oirs -O -vm -v -o gen_asm/hall.asm hall.c
@if %errorlevel% neq 0 goto error
:: 
c:\progs\cc65\bin\cl65 -S -C atari.cfg -t atari -T -Cl -Oirs -Ln -O -vm -v -o gen_asm/maze_generator.asm maze_generator.c
@if %errorlevel% neq 0 goto error
:: 
c:\progs\cc65\bin\cl65 -S -C atari.cfg -t atari -T -Cl -Oirs -O -vm -v -o gen_asm/draw_map.asm draw_map.c
@if %errorlevel% neq 0 goto error
:: 
c:\progs\cc65\bin\cl65 -S -C atari.cfg -t atari -T -Cl -Oirs -O -vm -v -o gen_asm/map_gui.asm map_gui.c
@if %errorlevel% neq 0 goto error

goto end
:error
pause
:end
pause
exit /b %errorlevel%