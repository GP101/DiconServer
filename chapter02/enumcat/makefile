!include <win32.mak>

objs=enumcat.obj

all: enumcat.exe

.cpp.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

enumcat.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:enumcat.exe $(objs) $(conlibsmt) ws2_32.lib ole32.lib

clean:
    del *.obj
    del *.exe
    del *.pdb
