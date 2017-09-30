!include <win32.mak>

objs=tcpserver.obj

all: tcpserver.exe

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

tcpserver.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:tcpserver.exe $(objs) $(conlibsmt) ws2_32.lib

clean:
    del *.obj
    del *.exe
