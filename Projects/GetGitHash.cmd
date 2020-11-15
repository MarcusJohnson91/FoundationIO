echo|set /p="#define Version_CommitID " >"%~dp0\..\Library\include\Private\CommitID.h"

FOR /F %%A IN ('git.exe branch --show-current') DO git.exe rev-parse %%A >>"%~dp0\..\Library\include\Private\CommitID.h"
