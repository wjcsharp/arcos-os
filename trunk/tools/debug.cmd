@echo off

start qemu-system-mipsel -M malta -kernel %BASEDIR%\base\ke\obj\mipsel32\kernel -s -S

start gdb -tui --eval-command="target remote :1234" %BASEDIR%\base\ke\obj\mipsel32\kernel
