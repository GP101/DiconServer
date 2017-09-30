!include <win32.mak>

objs=udpreceiver.obj

all: udpreceiver.exe

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

udpreceiver.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:udpreceiver.exe $(objs) $(conlibsmt) ws2_32.lib

clean:
    del *.obj
    del *.exe
