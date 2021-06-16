set MODNAME=QtAV
nmake distclean
rd  /s /q lib
rd  /s /q mkspecs
cd src/%MODNAME%
rd  /s /q .moc
rd  /s /q .obj
rd  /s /q .pch
rd  /s /q .uic
del /s /q *_resource.rc
cd ../../
