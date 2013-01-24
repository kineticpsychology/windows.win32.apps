@@echo off
REG ADD HKCR\*\shell\Change Times\command /v x /t REG_SZ /d "%CD%\Date Changer.exe" "%1"
pause
exit
