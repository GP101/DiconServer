!include <win32.mak>

objs=tcpclient.obj

all: tcpclient.exe

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

tcpclient.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:tcpclient.exe $(objs) $(conlibsmt) ws2_32.lib

clean:
    del *.obj
    del *.exe
