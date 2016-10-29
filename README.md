# C-experiments-for-msx
Tests and demos for msx in C for Hitech C cross compiler v7.8pl2

Compile using this:

C:\HTC\BIN\asz80  BMOVE.ASM 
C:\HTC\BIN\asz80  SC5.ASM
C:\HTC\BIN\ZC.EXE DEMOFSM.C VDPIO.C DSKIO.C BMOVE.OBJ SC5.OBJ -ASMLIST -PSECTMAP -Bc -P8 -O -Zg  -HSTART.SYM -OSTART.COM -MSTART.MAP
