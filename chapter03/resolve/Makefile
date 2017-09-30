!include <win32.mak>

objs=resolver.obj

all: resolver.exe

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvarsmt) $*.cpp

resolver.exe: $(objs) $(common_objs)
    $(link) $(linkdebug) $(conlflags) -out:resolver.exe $(objs) $(conlibsmt) ws2_32.lib

clean:
    del *.obj
    del *.exe
