@echo off
echo �������Vc rubbish,���Ե�......

rem c
rd /s /q vc\debug
rd /s /q vc\release
del /S /f /q vc\*.opt
del /S /f /q vc\*.ncb
del /S /f /q vc\*.aps
del /S /f /q vc\*.plg

rem prg
del /S /f /q Project\*.bak
del /S /f /q Project\*.a??
del /S /f /q Project\*.d??
del /S /f /q Project\*.scvd
del /S /f /q Project\Listings\*.*
del /S /f /q Project\Objects\*.*

echo ���Vc rubbish���!
echo. & pause

