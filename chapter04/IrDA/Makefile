# Nmake macros for building Windows 32-Bit apps
TARGETOS=BOTH

!include <win32.mak>


objs_irclient=irclient.obj

objs_irserver=irserver.obj

cflags=$(cflags) /D"_WIN32_WINNT"

all: irclient.exe irserver.exe

# Update the resource if necessary

# Update the object file if necessary

.cpp.obj: 
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

# Update the executable file if necessary, and if so, add the resource back in.

irclient.exe: $(objs_irclient) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:irclient.exe $(objs_irclient) $(conlibsmt) ws2_32.lib

irserver.exe: $(objs_irserver) $(common_objs) 
    $(link) $(linkdebug) $(conlflags) -out:irserver.exe $(objs_irserver) $(conlibsmt) ws2_32.lib


clean:
	del *.obj
	del *.exe
