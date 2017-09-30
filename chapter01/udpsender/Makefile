!include <win32.mak>

objs=udpsender.obj

all: udpsender.exe

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

udpsender.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:udpsender.exe $(objs) $(conlibsmt) ws2_32.lib

clean:
    del *.obj
    del *.exe
