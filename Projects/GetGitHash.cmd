@ECHO OFF
SETLOCAL EnableExtensions
SETLOCAL EnableDelayedExpansion

SET CommitID_Header="%~dp0\..\Library\include\Private\CommitID.h"

FOR /F %%A IN ('git branch --show-current') DO ( SET GitBranch=%%A )

FOR /F %%A IN ('git rev-parse !GitBranch!') DO ( SET CommitID=%%A )

CALL :HexUpper CommitID
ECHO #define Version_CommitID !CommitID! >!CommitID_Header!
ECHO #define Version_CommitIDMin !CommitID:~36,4! >>!CommitID_Header!

GOTO :EOF

:HexUpper
FOR %%i IN ("0=0" "1=1" "2=2" "3=3" "4=4" "5=5" "6=6" "7=7" "8=8" "9=9" "a=A" "b=B" "c=C" "d=D" "e=E" "f=F") DO ( CALL SET "%1=%%%1:%%~i%%" )
EXIT /b

ENDLOCAL
