@echo off

qemu-system-mipsel -M malta -kernel %BASEDIR%\base\ke\obj\mipsel32\kernel --serial vc --serial vc %1 %2 %3 %4 %5 %6 %7 %8 %9
