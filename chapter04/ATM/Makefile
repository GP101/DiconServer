# Nmake macros for building Windows 32-Bit apps
TARGETOS=BOTH

!include <win32.mak>


objs=wsockatm.obj support.obj

all: wsockatm.exe

# Update the resource if necessary

# Update the object file if necessary

.cpp.obj: 
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

# Update the executable file if necessary, and if so, add the resource back in.

wsockatm.exe: $(objs) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:wsockatm.exe $(objs) $(conlibsmt) ws2_32.lib


clean:
	del *.obj
	del *.exe
	del *.pdb
