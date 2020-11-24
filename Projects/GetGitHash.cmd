@ECHO OFF
SETLOCAL EnableExtensions
SETLOCAL EnableDelayedExpansion

SET Header=%~dp0..\Library\include\Private\CommitID.h

echo !Header!

echo|set /p="#define Version_CommitID " !CommitID_U! >!Header!

FOR /F %%A IN ('git branch --show-current') DO SET GitBranch=%%A

FOR /F %%A IN ('git rev-parse !GitBranch!') DO SET CommitID=%%A

SET CommitID_U=!CommitID!
CALL :HexUpper CommitID_U
echo !CommitID_U!>>!Header!
exit /b

:HexUpper
FOR %%i IN ("0=0" "1=1" "2=2" "3=3" "4=4" "5=5" "6=6" "7=7" "8=8" "9=9" "a=A" "b=B" "c=C" "d=D" "e=E" "f=F") DO CALL SET "%1=%%%1:%%~i%%"
exit /b

ENDLOCAL
