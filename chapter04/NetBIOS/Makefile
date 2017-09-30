# Nmake macros for building Windows 32-Bit apps
TARGETOS=BOTH

!include <win32.mak>


objs_wsnbclnt=wsnbclnt.obj wsnbdef.obj

objs_wsnbsvr=wsnbsvr.obj wsnbdef.obj

all: wsnbclnt.exe wsnbsvr.exe

# Update the resource if necessary

# Update the object file if necessary

.cpp.obj: 
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

# Update the executable file if necessary, and if so, add the resource back in.

wsnbclnt.exe: $(objs_wsnbclnt) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:wsnbclnt.exe $(objs_wsnbclnt) $(conlibsmt) ws2_32.lib

wsnbsvr.exe: $(objs_wsnbsvr) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:wsnbsvr.exe $(objs_wsnbsvr) $(conlibsmt) ws2_32.lib

clean:
	del *.obj
	del *.exe
	del *.pdb
