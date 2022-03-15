all: 

.c.obj:
    $(cc) $(include_paths) $(cdebug) $(cflags) $(cvars) $(scall) $*.c

.cxx.obj:
    $(cc) $(include_paths) $(cdebug) $(cflags) $(cvars) $(scall) $*.cxx

*.obj : *.h 

pipes-gl:
# TODO populate me afterward
	# $(link) $(linkdebug) $(savlflags) -machine:$(CPU) -out:$*.scr $** $(guilibs) opengl32.lib glu32.lib glaux.lib scrnsave.lib comdlg32.lib comctl32.lib
