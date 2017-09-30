# Nmake macros for building Windows 32-Bit apps
TARGETOS=BOTH

!include <win32.mak>


objs= atalk.obj

all: atalk.exe

# Update the resource if necessary

# Update the object file if necessary

.cpp.obj: 
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

# Update the executable file if necessary, and if so, add the resource back in.

atalk.exe: $(objs) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:atalk.exe $(objs) $(conlibsmt) wsock32.lib


clean:
	del *.obj
	del *.exe
	del *.pdb