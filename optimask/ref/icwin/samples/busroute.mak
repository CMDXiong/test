CC=cl
CFLAGS=-c -DSTRICT -J -G3 -Og -W3 -Zp1 -MT
LINKER=link
GUIFLAGS=-SUBSYSTEM:console
DLLFLAGS=-SUBSYSTEM:console -DLL
GUILIBS=-DEFAULTLIB:user32.lib gdi32.lib winmm.lib comdlg32.lib comctl32.lib mpr.lib
RC=rc
RCVARS=-r -DWIN32

busroute.exe : busroute.obj showlib.obj
     $(LINKER) $(GUIFLAGS) -OUT:busroute.exe -MAP:busroute.map \
     busroute.obj showlib.obj\
     $(GUILIBS)

busroute.obj : busroute.c showlib.h
     $(CC) $(CFLAGS) busroute.c 

showlib.obj : showlib.c showlib.h
     $(CC) $(CFLAGS) showlib.c 
